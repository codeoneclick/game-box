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

namespace game
{
    information_bubble::information_bubble() :
    m_max_visible_time(3000.f),
    m_visible_time(3000.f)
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
        bubble->font_size = 32.f;
        bubble->font_color = color;
        //bubble->rotation = 90.f;
        
        information_bubble::add_child(bubble);
    }
    
    void information_bubble::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_visible_time -= deltatime * 1000.f;
    }
}
