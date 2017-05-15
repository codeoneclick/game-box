//
//  footprint_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "footprint_controller.h"
#include "ces_action_component.h"
#include "footprint.h"

#define k_footptints_position_delta 8

namespace game
{
    footprint_controller::footprint_controller(const gb::game_object_2d_shared_ptr& layer,
                                               const gb::scene_graph_shared_ptr& scene_graph,
                                               const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_layer(layer),
    m_scene_graph(scene_graph),
    m_scene_fabricator(scene_fabricator)
    {
        std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&footprint_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        footprint_controller::add_component(action_component);
    }
    
    footprint_controller::~footprint_controller()
    {
        
    }
    
    void footprint_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_footprints.remove_if([=](const footprint_weak_ptr& footprint_weak) {
            if(!footprint_weak.expired())
            {
                auto footprint = footprint_weak.lock();
                if(!footprint->is_expired)
                {
                    return false;
                }
                else
                {
                    footprint->remove_from_parent();
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
    
    void footprint_controller::push_footprint(const glm::u8vec4& color, const glm::vec2& position, f32 rotation)
    {
        footprint_shared_ptr footprint = std::make_shared<game::footprint>();
        footprint->setup("footprint_01.xml",
                         m_scene_graph.lock(),
                         m_scene_fabricator.lock(),
                         nullptr);
        
        glm::vec2 current_position = position;
        current_position -= glm::vec2(-sinf(glm::radians(rotation)) * k_footptints_position_delta,
                                      cosf(glm::radians(rotation)) * k_footptints_position_delta);
        
        footprint->position = current_position;
        footprint->rotation = rotation;
        footprint->visible = false;
        m_layer.lock()->add_child(footprint);
        m_footprints.push_back(footprint);
    }
    
    const std::list<game::footprint_weak_ptr>& footprint_controller::get_footprints() const
    {
        return m_footprints;
    }
}
