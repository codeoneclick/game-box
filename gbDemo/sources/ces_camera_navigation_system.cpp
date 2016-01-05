//
//  ces_camera_navigation_system.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_camera_navigation_system.h"
#include "ces_koth_system_types.h"
#include "ces_koth_component_types.h"
#include "ces_camera_navigation_component.h"
#include "ces_camera_component.h"

#define key_w 119
#define key_a 97
#define key_d 100
#define key_s 115

namespace koth
{
    const struct ces_koth_system_type ces_koth_system_type =
    {
        "index.01.ces_koth_system_camera_navigation"
    };
    
    ces_camera_navigation_system::ces_camera_navigation_system()
    {
        m_type = ces_koth_system_type.ces_koth_system_camera_navigation;
        
        m_keys_state[key_w] = false;
        m_keys_state[key_a] = false;
        m_keys_state[key_d] = false;
        m_keys_state[key_s] = false;
        
        m_touch_point_delta = glm::ivec2(0);
    }
    
    ces_camera_navigation_system::~ces_camera_navigation_system()
    {
        
    }
    
    void ces_camera_navigation_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_camera_navigation_system::on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        e_move_navigation_state move_state = e_move_navigation_state_none;
        if(m_keys_state[key_d])
        {
            move_state = e_move_navigation_state_right;
        }
        else if(m_keys_state[key_a])
        {
            move_state = e_move_navigation_state_left;
        }
        else if(m_keys_state[key_s])
        {
            move_state = e_move_navigation_state_backward;
        }
        else if(m_keys_state[key_w])
        {
            move_state = e_move_navigation_state_forward;
        }
        
        ces_camera_navigation_component_shared_ptr camera_navigation_component =
        std::static_pointer_cast<koth::ces_camera_navigation_component>(entity->get_component(e_ces_koth_component_type_camera_navigation));
        
        gb::ces_camera_component_shared_ptr camera_component =
        std::static_pointer_cast<gb::ces_camera_component>(entity->get_component(gb::e_ces_component_type_camera));
        
        if(camera_navigation_component && camera_component)
        {
            camera_navigation_component->set_move_navigation_state(move_state);
            camera_navigation_component->set_touch_point_delta(m_touch_point_delta);
            camera_navigation_component->update(deltatime);
            camera_component->set_position(camera_navigation_component->get_position());
            camera_component->set_look_at(camera_navigation_component->get_look_at());
            camera_component->set_up(camera_navigation_component->get_up());
        }
    }
    
    void ces_camera_navigation_system::on_feed_end(f32 deltatime)
    {
        m_touch_point_delta = glm::ivec2(0);
    }
    
    void ces_camera_navigation_system::on_key_down(i32 key)
    {
        m_keys_state[key] = true;
    }
    
    void ces_camera_navigation_system::on_key_up(i32 key)
    {
        m_keys_state[key] = false;
    }
    
    void ces_camera_navigation_system::on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, gb::e_input_element input_element)
    {
        m_touch_point_delta = delta;
    }
}