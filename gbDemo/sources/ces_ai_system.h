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
        
        path_map_shared_ptr m_path_map;
        pathfinder_shared_ptr m_pathfinder;
        
        gb::ces_entity_weak_ptr m_main_character;
        std::map<std::string, gb::ces_entity_weak_ptr> m_ai_characters;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        bool line_rect_intersect(f32 x_0, f32 y_0, f32 x_1, f32 y_1, f32 left, f32 right, f32 top, f32 bottom);
        
        //bool intersect(const std::vector<glm::vec2>& bounding_box, const gb::mesh_shared_ptr& mesh);
        
        void update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_ai_system, gb::ces_base_system::g_guids_container)
        ces_ai_system();
        ~ces_ai_system();
        
        void set_path_map(const path_map_shared_ptr& path_map);
    };
};

