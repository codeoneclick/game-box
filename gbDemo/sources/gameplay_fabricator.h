//
//  gameplay_fabricator.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "db_declarations.h"
#include "ces_level_layers_component.h"

namespace game
{
    class gameplay_fabricator
    {
    private:
        
        characters_3d_assembler_shared_ptr m_characters_3d_assembler;
        
    protected:
        
        gb::scene_fabricator_weak_ptr m_general_fabricator;
        gb::anim::anim_fabricator_weak_ptr m_anim_fabricator;
        gb::gameplay_configuration_accessor_shared_ptr m_gameplay_configuration_accessor;
        gb::db::database_coordinator_shared_ptr m_database_coordinator;
        
        gb::game_object_2d_shared_ptr create_character(const std::string& filename, const std::array<gb::ces_entity_weak_ptr, ces_level_layers_component::e_level_layer_max>& layers);
        
    public:
        
        gameplay_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                            const gb::anim::anim_fabricator_shared_ptr& anim_fabricator);
        ~gameplay_fabricator() = default;
        
        gb::game_object_2d_shared_ptr create_level(const std::string& filename);
        gb::game_object_2d_shared_ptr create_main_character(const std::string& filename, const std::array<gb::ces_entity_weak_ptr, ces_level_layers_component::e_level_layer_max>& layers);
        gb::game_object_2d_shared_ptr create_mob(const std::string& filename, const std::array<gb::ces_entity_weak_ptr, ces_level_layers_component::e_level_layer_max>& layers);
        gb::game_object_2d_shared_ptr create_npc(const std::string& filename, const std::array<gb::ces_entity_weak_ptr, ces_level_layers_component::e_level_layer_max>& layers);
    };
};

