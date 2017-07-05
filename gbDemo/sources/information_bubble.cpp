//
//  information_bubble.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "information_bubble.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "label.h"
#include "sprite.h"
#include "ces_action_component.h"

#define k_information_bubble_min_font_size .1f
#define k_information_bubble_max_font_size 1.f
#define k_custom_z_order 256.f

namespace game
{
    information_bubble::information_bubble() :
    m_max_visible_time(0.f),
    m_visible_time(0.f)
    {
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
        
        is_expired.getter([=]() {
            return m_visible_time < 0.f;
        });
    }
    
    information_bubble::~information_bubble()
    {
        
    }
    
    void information_bubble::setup_components()
    {
        auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&information_bubble::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void information_bubble::setup(const std::string& filename,
                                   const gb::scene_graph_shared_ptr& scene_graph,
                                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                   const std::string& message,
                                   const glm::u8vec4& color,
                                   f32 visible_time_in_seconds)
    {
        auto bubble = scene_fabricator->create_label(filename);
        bubble->tag = "information_bubble";
        bubble->text = message;
        bubble->font_size = 18;
        bubble->font_color = color;
        bubble->z_order = k_custom_z_order;
        information_bubble::add_child(bubble);
        m_bubble = bubble;
        
        m_max_visible_time = visible_time_in_seconds * 1000.f;
        m_visible_time = visible_time_in_seconds * 1000.f;
    }
    
    void information_bubble::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_visible_time -= deltatime * 1000.f;
        if(!m_bubble.expired() && m_visible_time > 0.f)
        {
            auto bubble = m_bubble.lock();
            f32 delta_based_on_time = m_visible_time / m_max_visible_time;
            if(delta_based_on_time >= .5f)
            {
                f32 current_font_size = glm::mix(k_information_bubble_max_font_size * 2.f, k_information_bubble_min_font_size,
                                                 delta_based_on_time);
                bubble->scale = glm::vec2(current_font_size);
            }
            else
            {
                f32 current_font_size = glm::mix(k_information_bubble_max_font_size * 2.f, k_information_bubble_min_font_size,
                                                 1.f - delta_based_on_time);
                bubble->scale = glm::vec2(current_font_size);
            }
        }
    }
}
