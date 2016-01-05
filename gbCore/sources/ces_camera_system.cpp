//
//  ces_camera_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_camera_system.h"
#include "ces_camera_component.h"

namespace gb
{
    ces_camera_system::ces_camera_system()
    {
        m_type = ces_system_type.ces_system_type_camera;
    }
    
    ces_camera_system::~ces_camera_system()
    {
        
    }
    
    void ces_camera_system::on_feed_start(f32 deltatime)
    {

    }
    
    void ces_camera_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_camera_component_shared_ptr camera_component =
        std::static_pointer_cast<ces_camera_component>(entity->get_component(e_ces_component_type_camera));
        
        if(camera_component)
        {
            camera_component->update(deltatime);
        }
    }
    
    void ces_camera_system::on_feed_end(f32 deltatime)
    {
        
    }
}