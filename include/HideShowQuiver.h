#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

// See https://ng.commonlib.dev/class_r_e_1_1_actor.html

void HideQuiver(RE::Actor* actor) {
    if (!actor) {
        SKSE::log::error("HideQuiver: actor is null");
        return;
    }
    
    auto equipManager = RE::ActorEquipManager::GetSingleton();
    if (!equipManager) {
        SKSE::log::error("HideQuiver: equipManager not found");
        return;
    }
    
    auto inventory = actor->GetInventory([](RE::TESBoundObject& item) { return item.IsAmmo(); });  
    for (const auto& [item, data] : inventory) {
        if (data.second && data.second->IsWorn()) {                                
            equipManager->UnequipObject(actor, data.second->GetObject());
            break;
        }
    }    
}
