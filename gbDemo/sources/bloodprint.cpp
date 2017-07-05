//
//  bloodprint.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "bloodprint.h"
#include "scene_graph.h"
#include "scene_fabricator.h"
#include "ces_action_component.h"
#include "sprite.h"
#include "std_extensions.h"

#define k_bloodprint_visible_time 10000.f
#define k_bloodprint_size_delta 24.f

namespace game
{
    bloodprint::bloodprint() :
    m_max_visible_time(k_bloodprint_visible_time),
    m_visible_time(k_bloodprint_visible_time),
    m_size(0.f)
    {
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
        
        is_expired.getter([=]() {
            return m_visible_time < 0.f;
        });
    }
    
    bloodprint::~bloodprint()
    {
        
    }
    
    void bloodprint::setup_components()
    {
        auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&bloodprint::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void bloodprint::setup(const std::string& filename,
                           const gb::scene_graph_shared_ptr& scene_graph,
                           const gb::scene_fabricator_shared_ptr& scene_fabricator,
                           const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
    {
        auto bloodprint = scene_fabricator->create_sprite(filename);
        m_size = glm::vec2(std::get_random_i(24, 48));
        bloodprint->size = m_size;
        bloodprint->rotation = std::get_random_i(0, 180);
        bloodprint->pivot = glm::vec2(.5f);
        bloodprint->tag = "bloodprint";
        bloodprint->alpha = 1.f;
        bloodprint::add_child(bloodprint);
        m_bloodprint = bloodprint;
    }
    
    void bloodprint::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        if(!m_bloodprint.expired() && m_visible_time > 0.f)
        {
            f32 delta_based_on_time = m_visible_time / k_bloodprint_visible_time;
            
            f32 current_alpha = glm::mix(0, 255, delta_based_on_time);
            m_bloodprint.lock()->alpha = current_alpha;
            
            glm::vec2 current_size = glm::vec2(glm::mix(m_size + k_bloodprint_size_delta, m_size, delta_based_on_time));
            m_bloodprint.lock()->size = current_size;
        }
        m_visible_time -= deltatime * 1000.f;
    }
}
