//
//  server_character_controller.h
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
    class server_character_controller : public gb::ces_entity
    {
    public:
        
        typedef std::function<void(ui32, const glm::vec2&, const glm::vec2&, f32, bool)> on_character_moving_callback_t;
        
    private:
        
    protected:
        
        gb::game_object_shared_ptr m_character;
        ui32 m_udid;
        glm::vec2 m_delta;
        bool m_is_moving;
        
        on_character_moving_callback_t m_character_moving_callback;
        
    public:
        
        server_character_controller(ui32 udid);
        ~server_character_controller();
        
        void set_character(const gb::game_object_shared_ptr& character);
        
        void on_changed_server_transformation(const glm::vec2& delta,
                                              bool is_moving);
        void set_character_moving_callback(const on_character_moving_callback_t& callback);
        
        virtual void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
        glm::vec2 get_position() const;
        f32 get_rotation() const;
    };
};
