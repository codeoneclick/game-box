//
//  ces_ai_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//


#pragma once

#include "ces_base_system.h"
#include "ns_declarations.h"

namespace game
{
    class ces_ai_system : public gb::ces_base_system
    {
    private:
        
        glm::vec4 m_camera_2d_bounds;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_character_components_mask;
        
        path_map_shared_ptr m_path_map;
        pathfinder_shared_ptr m_pathfinder;
        
        gb::ces_entity_weak_ptr m_main_character;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_ai_characters;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_all_characters;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_ai_system, gb::ces_base_system::g_guids_container)
        ces_ai_system();
        ~ces_ai_system();
        
        void set_path_map(const path_map_shared_ptr& path_map);
    };
};

