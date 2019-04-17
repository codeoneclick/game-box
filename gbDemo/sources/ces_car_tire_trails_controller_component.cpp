//
//  ces_car_tire_trails_controller_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/3/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_tire_trails_controller_component.h"
#include "scene_fabricator.h"
#include "ces_trail_component.h"
#include "ces_transformation_3d_component.h"
#include "trail_3d.h"

namespace game
{
    ces_car_tire_trails_controller_component::ces_car_tire_trails_controller_component()
    {
        
    }
    
    void ces_car_tire_trails_controller_component::set_parameters(const std::string& filename,
                                                                  const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                  const gb::ces_entity_shared_ptr& tire_rl,
                                                                  const gb::ces_entity_shared_ptr& tire_rr)
    {
        m_filename = filename;
        m_scene_fabricator = scene_fabricator;
        m_tire_rl = tire_rl;
        m_tire_rr = tire_rr;
    }
    
    void ces_car_tire_trails_controller_component::enable_trails()
    {
        auto trail = m_scene_fabricator.lock()->create_trail_3d(m_filename);
        auto transformation_component = m_tire_rl.lock()->get_component<gb::ces_transformation_component>()->as_3d();
        auto trail_component = trail->get_component<gb::ces_trail_component>();
        auto position = transformation_component->get_absolute_position();
        position.y += .4f;
        trail_component->set_start_position(position);
        trail_component->set_enabled(true);
        
        m_tire_rl.lock()->add_child(trail);
        m_tire_rl_trail = trail;
        
        trail = m_scene_fabricator.lock()->create_trail_3d(m_filename);
        transformation_component = m_tire_rr.lock()->get_component<gb::ces_transformation_component>()->as_3d();
        trail_component = trail->get_component<gb::ces_trail_component>();
        position = transformation_component->get_absolute_position();
        position.y += .4f;
        trail_component->set_start_position(position);
        trail_component->set_enabled(true);
        
        m_tire_rr.lock()->add_child(trail);
        m_tire_rr_trail = trail;
    }
    
    void ces_car_tire_trails_controller_component::disable_trails()
    {
        if (!m_tire_rl_trail.expired())
        {
            const auto trail_component = m_tire_rl_trail.lock()->get_component<gb::ces_trail_component>();
            trail_component->set_enabled(false);
            m_disabled_trails.push_back(m_tire_rl_trail);
            m_tire_rl_trail.reset();
        }
        if (!m_tire_rr_trail.expired())
        {
            const auto trail_component = m_tire_rr_trail.lock()->get_component<gb::ces_trail_component>();
            trail_component->set_enabled(false);
            m_disabled_trails.push_back(m_tire_rr_trail);
            m_tire_rr_trail.reset();
        }
    }
    
    gb::ces_entity_shared_ptr ces_car_tire_trails_controller_component::get_tire_rl_trail()
    {
        gb::ces_entity_shared_ptr trail = nullptr;
        if (!m_tire_rl_trail.expired())
        {
            trail = m_tire_rl_trail.lock();
        }
        return trail;
    }
    
    gb::ces_entity_shared_ptr ces_car_tire_trails_controller_component::get_tire_rr_trail()
    {
        gb::ces_entity_shared_ptr trail = nullptr;
        if (!m_tire_rr_trail.expired())
        {
            trail = m_tire_rr_trail.lock();
        }
        return trail;
    }
    
    std::list<gb::ces_entity_weak_ptr> ces_car_tire_trails_controller_component::get_disabled_trails() const
    {
        return m_disabled_trails;
    }
}
