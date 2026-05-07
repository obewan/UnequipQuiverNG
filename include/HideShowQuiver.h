#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

// See https://ng.commonlib.dev/class_r_e_1_1_actor.html

RE::FormID lastBowAmmoFormID = 0;
RE::FormID lastCrossbowAmmoFormID = 0;

/**
 * save the currently equipped ammo FormID for the weapon type
 */
void SaveLastAmmo(RE::Actor* actor, RE::TESObjectWEAP* weapon) {
    if (!actor || !weapon) return;

    auto inventory = actor->GetInventory([](RE::TESBoundObject& boundObject) { return boundObject.IsAmmo(); });
    for (const auto& [obj, data] : inventory) {
        if (data.second && data.second->IsWorn()) {
            auto dataObj = data.second->GetObject();
            if (!dataObj) continue;

            auto ammo = RE::TESForm::LookupByID<RE::TESAmmo>(dataObj->formID);
            if (!ammo) break;

            if (weapon->IsBow() && !ammo->IsBolt()) {
                lastBowAmmoFormID = dataObj->formID;
            } else if (weapon->IsCrossbow() && ammo->IsBolt()) {
                lastCrossbowAmmoFormID = dataObj->formID;
            }
            break;
        }
    }
}

/**
 * unequip quiver if its ammo is in the inventory and if it is worn
 */
void HideQuiver(RE::Actor* actor, RE::TESObjectWEAP* weapon = nullptr, bool saveLast = true) {
    if (!actor) {
        SKSE::log::error("HideQuiver: actor is null");
        return;
    }

    auto equipManager = RE::ActorEquipManager::GetSingleton();
    if (!equipManager) {
        SKSE::log::error("HideQuiver: equipManager not found");
        return;
    }

    auto inventory = actor->GetInventory([](RE::TESBoundObject& boundObject) { return boundObject.IsAmmo(); });
    for (const auto& [obj, data] : inventory) {
        if (data.second && data.second->IsWorn()) {
            auto dataObj = data.second->GetObject();
            if (!dataObj) continue;

            if (saveLast && weapon) {
                auto ammo = RE::TESForm::LookupByID<RE::TESAmmo>(dataObj->formID);
                if (ammo) {
                    if (weapon->IsBow() && !ammo->IsBolt()) {
                        lastBowAmmoFormID = dataObj->formID;
                    } else if (weapon->IsCrossbow() && ammo->IsBolt()) {
                        lastCrossbowAmmoFormID = dataObj->formID;
                    }
                }
            }

            equipManager->UnequipObject(actor, dataObj);
            break;
        }
    }
}

/**
 * equip with previous ammo if there's still such ammo in the inventory
 */
void ShowQuiver(RE::Actor* actor, RE::TESObjectWEAP* weapon) {
    if (!actor) {
        SKSE::log::error("ShowQuiver: actor is null");
        return;
    }

    if (!weapon) {
        SKSE::log::error("ShowQuiver: ammo is null");
        return;
    }

    if ((weapon->IsBow() && lastBowAmmoFormID == 0) || (weapon->IsCrossbow() && lastCrossbowAmmoFormID == 0)) {
        return;  // using default behavior.
    }

    auto equipManager = RE::ActorEquipManager::GetSingleton();
    if (!equipManager) {
        SKSE::log::error("ShowQuiver: equipManager not found");
        return;
    }

    auto inventory = actor->GetInventory([](RE::TESBoundObject& boundObject) { return boundObject.IsAmmo(); });    
    // check if it should equip previous ammo
    for (const auto& [obj, data] : inventory) {
        auto dataObj = data.second->GetObject();
        if (!dataObj) {
            continue;
        }
        
        if ((weapon->IsBow() && dataObj->formID == lastBowAmmoFormID && !data.second->IsWorn()) ||
            (weapon->IsCrossbow() && dataObj->formID == lastCrossbowAmmoFormID && !data.second->IsWorn())) {
            try {
                equipManager->EquipObject(actor, dataObj);
            } catch (const std::exception& e) {
                SKSE::log::error("ShowQuiver: Exception occurred while equipping object: {}", e.what());
            }
            return;
        }
    }
}
