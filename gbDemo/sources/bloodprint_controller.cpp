//
//  bloodprint_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "bloodprint_controller.h"
#include "ces_action_component.h"
#include "bloodprint.h"
#include "std_extensions.h"

#define k_max_bloodprints_per_shoot 3
#define k_min_bloodprints_position_delta 0
#define k_max_bloodptints_position_delta 24

namespace game
{
    bloodprint_controller::bloodprint_controller(const gb::game_object_shared_ptr& layer,
                                                 const gb::scene_graph_shared_ptr& scene_graph,
                                                 const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_layer(layer),
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator)
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&bloodprint_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        bloodprint_controller::add_component(action_component);
    }
    
    bloodprint_controller::~bloodprint_controller()
    {
        
    }
    
    void bloodprint_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_bloodprints.remove_if([=](const bloodprint_weak_ptr& bloodprint_weak) {
            if(!bloodprint_weak.expired())
            {
                auto bloodprint = bloodprint_weak.lock();
                if(!bloodprint->is_expired)
                {
                    return false;
                }
                else
                {
                    bloodprint->remove_from_parent();
                    return true;
                }
            }
            else
            {
                assert(false);
            }
            return true;
        });
    }
    
    void bloodprint_controller::push_bloodprint(const glm::u8vec4& color, const glm::vec2& position, f32 rotation)
    {
        for(i32 i = 0; i < k_max_bloodprints_per_shoot; ++i)
        {
            bloodprint_shared_ptr bloodprint = std::make_shared<game::bloodprint>();
            bloodprint->setup("bloodprint_01.xml",
                              m_scene_graph.lock(),
                              m_scene_fabricator.lock(),
                              nullptr);
            
            glm::vec2 current_position = position;
            f32 delta_position = std::get_random_i(k_min_bloodprints_position_delta, k_max_bloodptints_position_delta);
            current_position += glm::vec2(-sinf(glm::radians(rotation)) * delta_position,
                                          cosf(glm::radians(rotation)) * delta_position);
            
            bloodprint->position = current_position;
            m_layer.lock()->add_child(bloodprint);
            m_bloodprints.push_back(bloodprint);
        }
    }
}
