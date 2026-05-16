#pragma once

#include "HideShowQuiver.h"
#include "RE/Skyrim.h"
#include "SKSE/API.h"
#include "SKSE/Logger.h"
#include "SKSE/SKSE.h"
#include "SKSE/Trampoline.h"

static bool isShowingQuiver = false;

static bool HasBowEquipped(RE::Actor* a_actor) {
    if (!a_actor) return false;
    auto equipped = a_actor->GetEquippedObject(true);
    if (equipped) {
        auto weapon = equipped->As<RE::TESObjectWEAP>();
        return weapon && (weapon->IsBow() || weapon->IsCrossbow());
    }
    return false;
}

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
                    // unequip bow or crossbow, save last ammo
                    SaveLastAmmo(actor, weapon);

                    // Use a delayed task to check if we should hide the quiver.
                    // This avoids hiding it if the player is just switching to another bow.
                    auto task = SKSE::GetTaskInterface();
                    if (task) {
                        task->AddTask([actor_id = actor->formID]() {
                            auto actor = RE::TESForm::LookupByID<RE::Actor>(actor_id);
                            if (actor && !HasBowEquipped(actor)) {
                                HideQuiver(actor, nullptr, false);
                                isShowingQuiver = false;
                            }
                        });
                    } else {
                        // Fallback if task interface is not available
                        if (!HasBowEquipped(actor)) {
                            HideQuiver(actor, nullptr, false);
                            isShowingQuiver = false;
                        }
                    }
                } else {
                    // equip bow or crossbow event, so equip quiver
                    ShowQuiver(actor, weapon);
                    isShowingQuiver = true;
                }

            } else if (a_event->equipped) {
                // check if it is ammo equip event, prior to new weapon equip event
                auto ammo = RE::TESForm::LookupByID<RE::TESAmmo>(a_event->baseObject);
                if (ammo && isShowingQuiver) {
                    isShowingQuiver = false;
                    return RE::BSEventNotifyControl::kContinue;  // continue the showing quiver event
                } else if (ammo) {
                    return RE::BSEventNotifyControl::kStop;  // Cancel the primary event to avoid double equip events
                } else {
                    // equip something that's not bow/crossbow and not ammo.
                    // only hide quiver if the player actually no longer has a bow equipped.
                    // This prevents unequipping when eating food or using potions.
                    if (!HasBowEquipped(actor)) {
                        HideQuiver(actor, nullptr, false);
                        isShowingQuiver = false;
                    }
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
