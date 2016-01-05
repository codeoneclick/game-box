//
//  ces_skybox_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_skybox_system.h"
#include "ces_skybox_component.h"
#include "ces_transformation_component.h"
#include "scene_graph_parameters.h"
#include "camera.h"

namespace gb
{
    ces_skybox_system::ces_skybox_system()
    {
        m_type = ces_system_type.ces_system_type_skybox;
    }
    
    ces_skybox_system::~ces_skybox_system()
    {
        
    }
    
    void ces_skybox_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_skybox_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_skybox_component_shared_ptr skybox_component =
        std::static_pointer_cast<ces_skybox_component>(entity->get_component(e_ces_component_type_skybox));
        
        ces_transformation_component_shared_ptr transformation_component =
        std::static_pointer_cast<ces_transformation_component>(entity->get_component(e_ces_component_type_transformation));
        
        if(skybox_component && transformation_component)
        {
            scene_graph_parameters_shared_ptr scene_graph_parameters = skybox_component->get_scene_graph_parameters();
            glm::vec3 skybox_position = scene_graph_parameters->get_eye_position();
            transformation_component->set_position(skybox_position);
        }
    }
    
    void ces_skybox_system::on_feed_end(f32 deltatime)
    {
        
    }
}