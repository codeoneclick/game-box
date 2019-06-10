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

namespace game
{
    class gameplay_fabricator
    {
    private:
        
        static glm::vec3 k_car_01_wheel_fl_offset;
        static glm::vec3 k_car_01_wheel_fr_offset;
        static glm::vec3 k_car_01_wheel_rl_offset;
        static glm::vec3 k_car_01_wheel_rr_offset;
        
        static glm::vec3 k_car_01_light_rl_offset;
        static glm::vec3 k_car_01_light_rr_offset;
        
        static i32 g_character_guid;
        
    protected:
        
        gb::scene_fabricator_weak_ptr m_general_fabricator;
        gb::gameplay_configuration_accessor_shared_ptr m_gameplay_configuration_accessor;
        gb::db::database_coordinator_shared_ptr m_database_coordinator;
        
        gb::game_object_3d_shared_ptr create_car(const std::string& filename);
        
    public:
        
        gameplay_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator);
        ~gameplay_fabricator() = default;
        
        void configure_levels_set(const gb::ces_entity_shared_ptr& root, const std::string& filename);
        
        gb::game_object_3d_shared_ptr create_scene(const std::string& filename);
        
        gb::game_object_3d_shared_ptr create_player_car(const std::string& filename);
        gb::game_object_3d_shared_ptr create_ai_car(const std::string& filename);
        
        void place_car_on_level(const gb::game_object_3d_shared_ptr& level, const gb::game_object_3d_shared_ptr& car, i32 spawner_position);
        void reconstruct_car_geometry(const gb::game_object_3d_shared_ptr& car, const std::string& filename);
        void reskin_car(const gb::game_object_3d_shared_ptr& car, const std::string& filename, ui32 skin_index);
    };
};

