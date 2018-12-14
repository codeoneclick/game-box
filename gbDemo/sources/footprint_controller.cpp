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
    const f32 footprint_controller::k_timeinterval = 333.f;
    
    footprint_controller::footprint_controller(const gb::ces_entity_shared_ptr& layer,
                                               const gb::scene_fabricator_shared_ptr& scene_fabricator) :
    m_layer(layer),
    m_scene_fabricator(scene_fabricator),
    m_previous_timestamp(std::chrono::steady_clock::now())
    {
        ces_entity::add_deferred_component_constructor<gb::ces_action_component>();
    }
    
    footprint_controller::~footprint_controller()
    {
        
    }
    
    void footprint_controller::setup_components()
    {
        auto action_component = ces_entity::get_component<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&footprint_controller::update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
    }
    
    void footprint_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        m_footprints.erase(std::remove_if(m_footprints.begin(), m_footprints.end(), [](const footprint_weak_ptr& footprint_weak) {
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
        }), m_footprints.end());
    }
    
    bool footprint_controller::is_ready_to_push_footprint() const
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
        f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_previous_timestamp).count();
        if(deltatime > k_timeinterval)
        {
            return true;
        }
        
#endif
        
        return false;
    }
    
    void footprint_controller::push_footprint(const glm::u8vec4& color, const glm::vec2& position, f32 rotation)
    {
        auto footprint = gb::ces_entity::construct<game::footprint>();
        footprint->setup("footprint_01.xml",
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
        
        std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
        m_previous_timestamp = current_timestamp;
    }
    
    const std::vector<game::footprint_weak_ptr>& footprint_controller::get_footprints() const
    {
        return m_footprints;
    }
}
