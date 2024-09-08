#pragma once

#include "HideShowQuiver.h"
#include "RE/Skyrim.h"
#include "SKSE/API.h"
#include "SKSE/Logger.h"
#include "SKSE/SKSE.h"
#include "SKSE/Trampoline.h"

class EquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent> {
public:
    virtual RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event,
                                                  RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override {
        if (!a_event || !a_event->actor) {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto actor = RE::TESForm::LookupByID<RE::Actor>(a_event->actor->formID);
        if (actor && actor->IsPlayerRef()) {
            auto item = RE::TESForm::LookupByID<RE::TESObjectWEAP>(a_event->baseObject);
            if (item && (item->IsBow() || item->IsCrossbow())) {
                if (!a_event->equipped) {
                    // Handle bow/crossbow unequip
                    HideQuiver(actor);
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
