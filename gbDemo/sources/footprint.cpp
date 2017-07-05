//
//  footprint.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "footprint.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "ces_action_component.h"
#include "sprite.h"

#define k_footprint_visible_time 5000.f
#define k_footprint_size 32.f

namespace game
{
    footprint::footprint() :
    m_max_visible_time(k_footprint_visible_time),
    m_visible_time(k_footprint_visible_time)
    {
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
        
        is_expired.getter([=]() {
            return m_visible_time < 0.f;
        });
    }
    
    footprint::~footprint()
    {
        
    }
    
    void footprint::setup_components()
    {
        auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&footprint::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void footprint::setup(const std::string& filename,
                          const gb::scene_graph_shared_ptr& scene_graph,
                          const gb::scene_fabricator_shared_ptr& scene_fabricator,
                          const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
    {
        auto footprint = scene_fabricator->create_sprite(filename);
        footprint->tag = "footprint";
        footprint->size = glm::vec2(k_footprint_size);
        footprint->pivot = glm::vec2(.5f);
        footprint->alpha = 1.f;
        footprint::add_child(footprint);
        m_footprint = footprint;
    }
    
    void footprint::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        if(!m_footprint.expired() && m_visible_time > 0.f)
        {
            f32 delta_based_on_time = m_visible_time / k_footprint_visible_time;
            
            f32 current_alpha = glm::mix(0, 255, delta_based_on_time);
            m_footprint.lock()->alpha = current_alpha;
        }
        m_visible_time -= deltatime * 1000.f;
    }
}
