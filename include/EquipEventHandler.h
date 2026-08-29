#pragma once

#include "HideShowQuiver.h"
#include "RE/Skyrim.h"
#include "SKSE/API.h"
#include "SKSE/Logger.h"
#include "SKSE/SKSE.h"

inline bool HasBowEquipped(RE::Actor* a_actor) {
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
                                                  RE::BSTEventSource<RE::TESEquipEvent>*) override {
        if (!a_event || !a_event->actor) {
            return RE::BSEventNotifyControl::kContinue;
        }

        auto actor = a_event->actor->As<RE::Actor>();
        if (actor && actor->IsPlayerRef()) {
            // Only bow and crossbow events drive the quiver.
            //
            // Reacting to any other equip would strip a quiver the player is wearing on
            // purpose, and it is redundant anyway: swapping a bow for anything else always
            // fires the bow unequip handled below.
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
                                HideQuiver(actor);
                            }
                        });
                    } else {
                        // Fallback if task interface is not available
                        if (!HasBowEquipped(actor)) {
                            HideQuiver(actor);
                        }
                    }
                } else {
                    // equip bow or crossbow event, so equip quiver
                    ShowQuiver(actor, weapon);
                }
            }
        }

        // Always kContinue. BSTEventSource::SendEvent breaks its dispatch loop on kStop,
        // which starves every sink registered after this one -- equipment display mods
        // stop seeing equip events and their gear goes invisible. It would not cancel the
        // game's own equip in any case; the return value only controls the sink chain.
        return RE::BSEventNotifyControl::kContinue;
    }
};

inline void RegisterEquipEventHandler() {
    static EquipEventHandler equipEventHandler;
    auto scriptEventSource = RE::ScriptEventSourceHolder::GetSingleton();
    if (scriptEventSource) {
        scriptEventSource->AddEventSink(&equipEventHandler);
        SKSE::log::info("EquipEventHandler registered");
    } else {
        SKSE::log::error("Failed to get ScriptEventSourceHolder singleton");
    }
}
