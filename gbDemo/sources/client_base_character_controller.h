//
//  client_base_character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "ces_entity.h"

namespace ns
{
    class client_base_character_controller : public gb::ces_entity
    {
    private:
        
    protected:
        
        gb::game_object_shared_ptr m_character;
        
        glm::vec2 m_server_velocity;
        glm::vec2 m_server_position;
        f32 m_server_rotation;
        bool m_is_moving;
        
    public:
        
        client_base_character_controller(const gb::game_object_shared_ptr& character);
        ~client_base_character_controller();
        
        void on_changed_server_transformation(const glm::vec2& velocity,
                                              const glm::vec2& position,
                                              f32 rotation,
                                              bool is_moving);
        
        virtual void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
    };
};
