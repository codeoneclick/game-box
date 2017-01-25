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
        
        typedef std::function<void(ui64, ui32, const glm::vec2&, const glm::vec2&, f32)>
        on_server_character_move_callback_t;
        
    private:
        
        struct client_character_move_history_point
        {
            ui64 m_client_tick;
            glm::vec2 m_delta;
        };
        
    protected:
        
        gb::game_object_shared_ptr m_character;
        i32 m_udid;
        
        std::queue<client_character_move_history_point> m_client_character_move_history;
        on_server_character_move_callback_t m_server_character_move_callback;
        
    public:
        
        server_character_controller(ui32 udid);
        ~server_character_controller();
        
        std::property_ro<glm::vec2> position;
        std::property_ro<f32> rotation;
        
        void set_character(const gb::game_object_shared_ptr& character);
        
        void on_client_character_move(ui64 client_tick, const glm::vec2& delta);
        void set_server_character_move_callback(const on_server_character_move_callback_t& callback);
        
        virtual void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
    };
};
