//
//  camera.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "camera.h"
#include "ces_camera_component.h"

namespace gb
{
    camera::camera(f32 fov, f32 near, f32 far, glm::ivec4 viewport)
    {
        ces_camera_component_shared_ptr camera_component = std::make_shared<ces_camera_component>();
        ces_entity::add_component(camera_component);
        
        camera_component->set_viewport(viewport);
        camera_component->set_near(near);
        camera_component->set_far(far);
        camera_component->set_fov(fov);
    }
    
    camera::~camera()
    {
        
    }
}