//
//  ces_box2d_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_box2d_system.h"
#include "collision_manager.h"
#include "ces_box2d_component.h"

namespace gb
{
    ces_box2d_system::ces_box2d_system()
    {
        m_type = ces_system_type.ces_system_type_box2d;
        m_collision_manager = std::make_shared<collision_manager>();
    }
    
    ces_box2d_system::~ces_box2d_system()
    {
        
    }
    
    void ces_box2d_system::on_feed_start(f32 deltatime)
    {
        m_collision_manager->update(deltatime);
    }
    
    void ces_box2d_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_box2d_component_shared_ptr box2d_component =
        std::static_pointer_cast<ces_box2d_component>(entity->get_component(e_ces_component_type_box2d));
        
        if(box2d_component && box2d_component->get_box2d_body())
        {
            box2d_component->on_position_changed(glm::vec3(box2d_component->get_box2d_body()->GetPosition().x,
                                                           0.f,
                                                           box2d_component->get_box2d_body()->GetPosition().y));
            box2d_component->on_rotation_changed(box2d_component->get_box2d_body()->GetAngle());
        }
    }
    
    void ces_box2d_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    collision_manager_shared_ptr ces_box2d_system::get_collision_manager() const
    {
        return m_collision_manager;
    }
    
    void ces_box2d_system::set_box2d_world(const glm::vec2 &min_bound, const glm::vec2 &max_bound)
    {
        m_collision_manager->create_box2d_world(min_bound, max_bound);
    }
}