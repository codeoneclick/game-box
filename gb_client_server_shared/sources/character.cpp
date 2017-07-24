//
//  character.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "character.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_2d_component.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source_2d.h"
#include "shape_3d.h"
#include "ces_bound_touch_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_material_component.h"
#include "ces_action_component.h"

namespace game
{
    /*const std::string character::parts::k_bounds_part = "bounds";
    const std::string character::parts::k_body_part = "body";
    const std::string character::parts::k_light_source_part = "light_source";
    
    const std::string character::animations::k_idle_animation = "idle";
    const std::string character::animations::k_walk_animation = "move";
    const std::string character::animations::k_attack_animation = "attack";
    const std::string character::animations::k_search_animation = "search";
    const std::string character::animations::k_die_animation = "die";*/
    
    const std::string character::k_light_source_entity_filename = "light_01.xml";
    const std::string character::k_bounds_entity_filename = "character.bounds.2d.xml";
    
    const f32 character::k_bounds_size = 96.f;
    
#define k_blinking_count 15
#define k_blinking_timeinterval 100.f

    character::character() :
    m_animation_end_callback_id(-1),
    m_on_tap_on_character_callback(nullptr),
    m_on_death_effect_ended_callback(nullptr),
    m_blinking_count(0)
    {
        character::tag = "character";
    }
    
    character::~character()
    {
        
    }
    
    void character::setup_light(const gb::scene_graph_shared_ptr& scene_graph,
                                const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                const glm::vec4& ligth_source_color)
    {
        auto light_source = scene_fabricator->create_light_source_2d(k_light_source_entity_filename);
        light_source->radius = 512.f;
        light_source->color = ligth_source_color;
        light_source->tag = parts::k_light_source_part;
        character::add_child(light_source);
    }
    
    void character::setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage,
                          const gb::scene_graph_shared_ptr& scene_graph,
                          const gb::scene_fabricator_shared_ptr& scene_fabricator,
                          bool is_enabled_light_source,
                          const glm::vec4& ligth_source_color)
    {
        auto bounds = scene_fabricator->create_sprite(k_bounds_entity_filename);
        bounds->tag = parts::k_bounds_part;
        bounds->size = glm::vec2(k_bounds_size);
        bounds->pivot = glm::vec2(.5f, .5f);
        bounds->color = glm::u8vec4(0, 0, 0, 0);
        character::add_child(bounds);
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_bounds(glm::vec4(-k_bounds_size * .5f, -k_bounds_size * .5f,
                                                    k_bounds_size * .5f, k_bounds_size * .5f));
        bound_touch_component->enable(gb::e_input_state_pressed, gb::e_input_source_mouse_left, true);
        bound_touch_component->enable(gb::e_input_state_released, gb::e_input_source_mouse_left, true);
        bound_touch_component->add_callback(gb::e_input_state_pressed, std::bind(&character::on_touched, this,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2,
                                                                                 std::placeholders::_3,
                                                                                 std::placeholders::_4));
        bound_touch_component->add_callback(gb::e_input_state_released, std::bind(&character::on_touched, this,
                                                                                  std::placeholders::_1,
                                                                                  std::placeholders::_2,
                                                                                  std::placeholders::_3,
                                                                                  std::placeholders::_4));
        ces_entity::add_component(bound_touch_component);
        
        m_shape_2d_linkage = character_linkage.first;
        m_shape_3d_linkage = character_linkage.second;
        
        character::add_child(character_linkage.first);
        if(is_enabled_light_source)
        {
            character::setup_light(scene_graph, scene_fabricator, ligth_source_color);
        }
        character::remove_death_effect();
    }
    
    void character::play_animation(const std::string &animation_name, bool is_looped)
    {
        if(!m_shape_3d_linkage.expired())
        {
            m_shape_3d_linkage.lock()->play_animation(animation_name, is_looped);
        }
    }
    
    void character::set_animation_end_callback(const std::function<void(const std::string&, bool)> callback)
    {
        auto animation_3d_mixer_component = m_shape_3d_linkage.lock()->get_component<gb::ces_animation_3d_mixer_component>();
        if(m_animation_end_callback_id != -1)
        {
            animation_3d_mixer_component->remove_animation_end_callback(m_animation_end_callback_id);
        }
        m_animation_end_callback_id = animation_3d_mixer_component->add_animation_end_callback(callback);
    }
    
    void character::on_touched(const gb::ces_entity_shared_ptr&,
                               const glm::vec2& touch_point,
                               gb::e_input_source input_source,
                               gb::e_input_state input_state)
    {
        if(m_on_tap_on_character_callback && input_state == gb::e_input_state::e_input_state_released)
        {
            m_on_tap_on_character_callback(shared_from_this());
        }
    }
    
    void character::set_on_tap_on_character_callback(const on_tap_on_character_callback_t& callback)
    {
        m_on_tap_on_character_callback = callback;
    }
    
    void character::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        m_blinking_timeinterval -= dt * 1000.f;
        if(m_blinking_timeinterval <= 0)
        {
            if(m_blinking_count > 0)
            {
                if(m_blinking_count % 2 == 0)
                {
                    m_shape_2d_linkage.lock()->color = glm::u8vec4(64);
                }
                else
                {
                    m_shape_2d_linkage.lock()->color = glm::u8vec4(255);
                }
                m_blinking_count--;
                m_blinking_timeinterval = k_blinking_timeinterval;
            }
            else
            {
                m_shape_2d_linkage.lock()->color = glm::u8vec4(0);
                character::remove_component(gb::ces_action_component::class_guid());
                if(m_on_death_effect_ended_callback)
                {
                    m_on_death_effect_ended_callback();
                }
            }
        }
    }
    
    void character::apply_death_effect()
    {
        m_blinking_count = k_blinking_count;
        auto action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&character::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        character::add_component(action_component);
    }
    
    void character::remove_death_effect()
    {
        m_shape_2d_linkage.lock()->color = glm::u8vec4(255);
        character::remove_component(gb::ces_action_component::class_guid());
    }
    
    void character::set_on_death_effect_ended_callback(const on_death_effect_ended_callback_t& callback)
    {
        m_on_death_effect_ended_callback = callback;
    }
}

