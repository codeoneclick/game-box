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

#define k_bloodprint_visible_time 3000.f

namespace game
{
    bloodprint::bloodprint() :
    m_max_visible_time(k_bloodprint_visible_time),
    m_visible_time(k_bloodprint_visible_time)
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&bloodprint::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        bloodprint::add_component(action_component);
        
        is_expired.getter([=]() {
            return m_visible_time < 0.f;
        });
    }
    
    bloodprint::~bloodprint()
    {
        
    }
    
    void bloodprint::setup(const std::string& filename,
                           const gb::scene_graph_shared_ptr& scene_graph,
                           const gb::scene_fabricator_shared_ptr& scene_fabricator,
                           const gb::anim::anim_fabricator_shared_ptr& anim_fabricator)
    {
        auto bloodprint = scene_fabricator->create_sprite(filename);
        bloodprint->size = glm::vec2(std::get_random_i(32, 64));
        bloodprint->rotation = std::get_random_i(0, 180);
        bloodprint->pivot = glm::vec2(.5f);
        bloodprint->tag = "bloodprint";
        bloodprint->is_luminous = true;
        bloodprint::add_child(bloodprint);
    }
    
    void bloodprint::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_visible_time -= deltatime * 1000.f;
    }
}
