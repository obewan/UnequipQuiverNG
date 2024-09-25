#pragma once

#include "HideShowQuiver.h"
#include "RE/Skyrim.h"
#include "SKSE/API.h"
#include "SKSE/Logger.h"
#include "SKSE/SKSE.h"
#include "SKSE/Trampoline.h"

static bool isShowingQuiver = false;

class EquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent> {
public:
    virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event,
                                                  RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override {
        if (!a_event || !a_event->actor) {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto actor = RE::TESForm::LookupByID<RE::Actor>(a_event->actor->formID);
        if (actor && actor->IsPlayerRef()) {
            auto weapon = RE::TESForm::LookupByID<RE::TESObjectWEAP>(a_event->baseObject);
            if (weapon && (weapon->IsBow() || weapon->IsCrossbow())) {
                if (!a_event->equipped) {
                    // unequip bow or crossbow event, so unequip quiver
                    HideQuiver(actor, weapon);
                    isShowingQuiver = false;
                } else {
                    // equip bow or crossbow event, so equip quiver
                    ShowQuiver(actor, weapon);
                    isShowingQuiver = true;
                }
                
            } else if (!weapon && a_event->equipped) {
                // check if it is ammo equip event, prior to new weapon equip event
                auto ammo = RE::TESForm::LookupByID<RE::TESAmmo>(a_event->baseObject);
                if (ammo && isShowingQuiver) {
                    isShowingQuiver = false;
                    return RE::BSEventNotifyControl::kContinue; // continue the showing quiver event
                }
                else if (ammo) {
                    return RE::BSEventNotifyControl::kStop;  // Cancel the primary event to avoid double equip events
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }
};

void RegisterEquipEventHandler() {
    static EquipEventHandler equipEventHandler;
    auto scriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();
    if (scriptEventSource) {
        scriptEventSource->AddEventSink(&equipEventHandler);
        SKSE::log::info("EquipEventHandler registered");
    } else {
        SKSE::log::error("Failed to get ScriptEventSourceHolder singleton");
    }
}
