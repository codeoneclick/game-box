//
//  hit_bounds_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "hit_bounds_controller.h"
#include "ces_action_component.h"
#include "hit_bounds.h"

namespace game
{
    hit_bounds_controller::hit_bounds_controller(const gb::ces_entity_shared_ptr& layer,
                                                 const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_layer(layer),
    m_scene_fabricator(scene_fabricator)
    {
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
    }
    
    hit_bounds_controller::~hit_bounds_controller()
    {
        
    }
    
    void hit_bounds_controller::setup_components()
    {
        auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&hit_bounds_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void hit_bounds_controller::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        m_hit_bounds.erase(std::remove_if(m_hit_bounds.begin(), m_hit_bounds.end(), [](const hit_bounds_weak_ptr& hit_bounds_weak) {
            if(!hit_bounds_weak.expired())
            {
                            }
            else
            {
                assert(false);
            }
            return true;
        }), m_bloodprints.end());
    }
    
    void hit_bounds_controller::push_bloodprint(const glm::u8vec4& color, const glm::vec2& position, f32 rotation)
    {
        for(i32 i = 0; i < k_max_bloodprints_per_shoot; ++i)
        {
            auto bloodprint = gb::ces_entity::construct<game::bloodprint>();
            bloodprint->setup("bloodprint_01.xml",
                              m_scene_fabricator.lock(),
                              nullptr);
            
            glm::vec2 current_position = position;
            f32 delta_position = std::get_random_i(k_min_bloodprints_position_delta, k_max_bloodptints_position_delta);
            current_position += glm::vec2(-sinf(glm::radians(rotation)) * delta_position,
                                          cosf(glm::radians(rotation)) * delta_position);
            
            bloodprint->position = current_position;
            bloodprint->visible = false;
            m_layer.lock()->add_child(bloodprint);
            m_bloodprints.push_back(bloodprint);
        }
    }
    
    const std::vector<game::bloodprint_weak_ptr>& bloodprint_controller::get_bloodprints() const
    {
        return m_bloodprints;
    }
}
