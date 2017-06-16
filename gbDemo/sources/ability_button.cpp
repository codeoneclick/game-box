//
//  ability_button.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ability_button.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_2d_component.h"

namespace game
{
    const std::string ability_button::k_pressed_state = "pressed";
    const std::string ability_button::k_released_state = "released";
    const std::string ability_button::k_background_entity_filename = "ability.button.xml";
    const f32 ability_button::k_background_size = 48.f;
    
    ability_button::ability_button() :
    m_on_tap_on_ability_callback(nullptr)
    {
        auto transformation_component = ability_button::get_component<gb::ces_transformation_2d_component>();
        transformation_component->set_is_in_camera_space(false);
    }
    ability_button::~ability_button()
    {
        
    }
    
    void ability_button::setup(const std::string& filename,
                               const gb::scene_graph_shared_ptr& scene_graph,
                               const gb::scene_fabricator_shared_ptr& scene_fabricator,
                               const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
    {
        auto background = scene_fabricator->create_sprite(k_background_entity_filename);
        background->tag = k_background_entity_filename;
        background->size = glm::vec2(k_background_size);
        background->color = glm::u8vec4(0, 0, 255, 255);
        ability_button::add_child(background);
        
        auto transformation_component = background->get_component<gb::ces_transformation_2d_component>();
        transformation_component->set_is_in_camera_space(false);
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_frame(glm::vec4(0.f, 0.f,
                                                   k_background_size, k_background_size));
        bound_touch_component->enable(gb::e_input_state_pressed, gb::e_input_source_mouse_left, true);
        bound_touch_component->enable(gb::e_input_state_released, gb::e_input_source_mouse_left, true);
        bound_touch_component->add_callback(gb::e_input_state_pressed, std::bind(&ability_button::on_touched, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        bound_touch_component->add_callback(gb::e_input_state_released, std::bind(&ability_button::on_touched, this, std::placeholders::_1,
                                                                                  std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        ability_button::add_component(bound_touch_component);
    }
    
    void ability_button::attach_sound(const std::string& filename, const std::string& state)
    {
        
    }
    
    void ability_button::on_touched(const gb::ces_entity_shared_ptr&, const glm::vec2& point,
                                    gb::e_input_source input_source, gb::e_input_state input_state)
    {
        if(m_on_tap_on_ability_callback && input_state == gb::e_input_state::e_input_state_released)
        {
            m_on_tap_on_ability_callback(shared_from_this());
        }
    }
    
    void ability_button::set_on_tap_on_ability_callback(const on_tap_on_ability_callback_t& callback)
    {
        m_on_tap_on_ability_callback = callback;
    }
}

