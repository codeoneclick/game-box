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
#include "text_label.h"
#include "ces_action_component.h"

#define k_information_bubble_visible_time 3333.f
#define k_information_bubble_min_font_size 12.f
#define k_information_bubble_max_font_size 24.f

namespace game
{
    information_bubble::information_bubble() :
    m_max_visible_time(k_information_bubble_visible_time),
    m_visible_time(k_information_bubble_visible_time)
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&information_bubble::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        information_bubble::add_component(action_component);
        
        is_expired.getter([=]() {
            return m_visible_time < 0.f;
        });
    }
    
    information_bubble::~information_bubble()
    {
        
    }
    
    void information_bubble::setup(const std::string& filename,
                                   const gb::scene_graph_shared_ptr& scene_graph,
                                   const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                   const std::string& message,
                                   const glm::u8vec4& color)
    {
        auto bubble = scene_fabricator->create_text_label(filename);
        bubble->tag = "information_bubble";
        bubble->text = message;
        bubble->font_size = k_information_bubble_min_font_size;
        bubble->font_color = color;
        bubble->is_luminous = true;
        information_bubble::add_child(bubble);
        
        m_bubble = bubble;
    }
    
    void information_bubble::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_visible_time -= deltatime * 1000.f;
        if(!m_bubble.expired() && m_visible_time > 0.f)
        {
            f32 delta_based_on_time = m_visible_time / k_information_bubble_visible_time;
            auto bubble = m_bubble.lock();
            f32 current_font_size = glm::mix(k_information_bubble_max_font_size, k_information_bubble_min_font_size,
                                             delta_based_on_time);
            bubble->font_size = current_font_size;
            
            glm::u8vec4 current_font_color = bubble->font_color;
            current_font_color.w = glm::mix(255, 0, delta_based_on_time);
            bubble->font_color = current_font_color;
        }
    }
}
