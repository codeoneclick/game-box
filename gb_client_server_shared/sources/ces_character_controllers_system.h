//
//  ces_character_controllers_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

namespace game
{
    class ces_character_controllers_system : public gb::ces_base_system
    {
    private:
        
        gb::mesh_shared_ptr m_camera_mesh;
        glm::vec4 m_camera_bounds;
        
        gb::ces_entity_weak_ptr m_main_character;
        std::map<std::string, gb::ces_entity_weak_ptr> m_ai_characters;
        std::map<std::string, gb::ces_entity_weak_ptr> m_all_characters;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void update_recursively(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
    public:
        
        CTTI_CLASS_GUID(ces_character_controllers_system, gb::ces_base_system::g_guids_container)
        ces_character_controllers_system();
        ~ces_character_controllers_system();
    };
};
