//
//  fullscreen_joystick.cpp
//  gbUI
//
//  Created by serhii serhiiv on 3/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "fullscreen_joystick.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_component.h"
#include "ces_material_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ui
    {
        static const f32 k_double_tap_time_interval = 333.f;
        static const f32 k_visual_size = 128.f;
        
        fullscreen_joystick::fullscreen_joystick(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_is_dragged(false),
        m_on_dragging_callback(nullptr),
        m_on_end_dragging_callback(nullptr),
        m_on_double_tap_callback(nullptr),
        m_center(glm::vec2(0.f))
        {
            position.setter([=](const glm::vec2& position) {
                
                m_center = position;
                m_elements["joystick_background"]->position = glm::vec2(position.x - k_visual_size * .5f,
                                                                        position.y - k_visual_size * .5f);
                m_elements["joystick_button"]->position = glm::vec2(position.x - k_visual_size * .25f,
                                                                    position.y - k_visual_size * .25f);
            });
            
            size.setter([=](const glm::vec2& size) {
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
                bound_touch_component->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
            });
        }
        
        fullscreen_joystick::~fullscreen_joystick()
        {
            
        }
        
        fullscreen_joystick_shared_ptr fullscreen_joystick::construct(const scene_fabricator_shared_ptr& fabricator)
        {
            auto entity = std::make_shared<fullscreen_joystick>(fabricator);
            auto bound_touch_component = std::make_shared<ces_bound_touch_component>();
            bound_touch_component->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            bound_touch_component->enable(e_input_state_released, e_input_source_mouse_left, true);
            bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, true);
            
            bound_touch_component->add_callback(e_input_state_pressed, std::bind(&fullscreen_joystick::on_pressed, entity,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2,
                                                                                 std::placeholders::_3,
                                                                                 std::placeholders::_4));
            bound_touch_component->add_callback(e_input_state_released, std::bind(&fullscreen_joystick::on_released, entity,
                                                                                  std::placeholders::_1,
                                                                                  std::placeholders::_2,
                                                                                  std::placeholders::_3,
                                                                                  std::placeholders::_4));
            bound_touch_component->add_callback(e_input_state_dragged, std::bind(&fullscreen_joystick::on_dragged, entity,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2,
                                                                                 std::placeholders::_3,
                                                                                 std::placeholders::_4));
            
            entity->add_component(bound_touch_component);
            
            return entity;
        }
        
        void fullscreen_joystick::create()
        {
            gb::sprite_shared_ptr joystick_background = control::get_fabricator()->create_sprite("joystick_background.xml");
            joystick_background->size = glm::vec2(k_visual_size);
            m_elements["joystick_background"] = joystick_background;
            fullscreen_joystick::add_child(joystick_background);
            
            gb::sprite_shared_ptr joystick_button = control::get_fabricator()->create_sprite("joystick_button.xml");
            joystick_button->size = glm::vec2(k_visual_size * .5f);
            m_elements["joystick_button"] = joystick_button;
            fullscreen_joystick::add_child(joystick_button);
            
            control::create();
        }
        
        void fullscreen_joystick::on_pressed(const ces_entity_shared_ptr&,
                                             const glm::vec2& touch_point,
                                             e_input_source input_source,
                                             e_input_state input_state)
        {
            fullscreen_joystick::position = touch_point;
        }
        
        void fullscreen_joystick::on_released(const ces_entity_shared_ptr&,
                                              const glm::vec2& touch_point,
                                              e_input_source input_source,
                                              e_input_state input_state)
        {
            fullscreen_joystick::position = m_center;
            m_is_dragged = false;
            
            if(m_on_end_dragging_callback)
            {
                m_on_end_dragging_callback(shared_from_this());
            }
            
            static ui32 releases_count = 0;
            releases_count++;
            static std::chrono::steady_clock::time_point previous_timestamp = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
            f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - previous_timestamp).count();
            previous_timestamp = current_timestamp;
            if(deltatime < k_double_tap_time_interval && releases_count >= 2)
            {
                releases_count = 0;
                if(m_on_double_tap_callback)
                {
                    m_on_double_tap_callback(shared_from_this());
                }
            }
        }
        
        void fullscreen_joystick::on_dragged(const ces_entity_shared_ptr&,
                                             const glm::vec2& touch_point,
                                             e_input_source input_source,
                                             e_input_state input_state)
        {
            gb::sprite_shared_ptr joystick_button = std::static_pointer_cast<gb::sprite>(m_elements["joystick_button"]);
            glm::vec2 element_size = joystick_button->size;
            
            glm::vec2 parent_offset = glm::vec2(0.f);
            
            ces_entity_shared_ptr parent = joystick_button->parent;
            while (parent) {
                auto transformation_component = parent->get_component<ces_transformation_2d_component>();
                glm::vec2 parent_position = transformation_component->get_position();
                parent_offset.x += parent_position.x;
                parent_offset.y += parent_position.y;
                parent = parent->parent;
            }
            
            glm::vec2 position = joystick_button->position;
            glm::vec2 delta = position;
            
            position.x = touch_point.x - parent_offset.x - element_size.x * .5f;
            position.y = touch_point.y - parent_offset.y - element_size.y * .5f;
            
            joystick_button->position = position;
            
            if(m_on_dragging_callback)
            {
                delta.x = m_center.x - position.x - element_size.x * .5f;
                delta.y = m_center.y - position.y - element_size.x * .5f;
                delta = glm::normalize(delta);
                f32 angle = atan2(delta.y, delta.x) - atan2(1.f, 0.f);
                m_on_dragging_callback(shared_from_this(), delta, angle);
            }
        }
        
        void fullscreen_joystick::set_on_dragging_callback(const on_dragging_callback_t& callback)
        {
            m_on_dragging_callback = callback;
        }
        
        void fullscreen_joystick::set_on_end_dragging_callback(const on_end_dragging_callback_t& callback)
        {
            m_on_end_dragging_callback = callback;
        }
        
        void fullscreen_joystick::set_on_double_tap_callback(const on_double_tap_callback_t& callback)
        {
            m_on_double_tap_callback = callback;
        }
    }
}
