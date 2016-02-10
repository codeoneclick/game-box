//
//  ces_transformation_extension.cpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_transformation_extension.h"
#include "ces_scene_component.h"
#include "ces_entity.h"
#include "camera.h"

namespace gb
{
    glm::mat4 ces_transformation_extension::get_absolute_matrix(const ces_entity_shared_ptr& entity)
    {
        glm::mat4 matrix = glm::mat4(1.f);
        ces_entity_shared_ptr parent = entity->parent;
        
        while(parent)
        {
            ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
            matrix = matrix * transformation_component->get_matrix_m();
            parent = parent->parent;
        }
        return matrix * unsafe_get_transformation_component(entity)->get_matrix_m();
    }
    
    glm::mat4 ces_transformation_extension::get_absolute_matrix_in_camera_space(const ces_entity_shared_ptr& entity)
    {
        glm::mat4 matrix = ces_transformation_extension::get_absolute_matrix(entity);
        if(entity->is_component_exist(e_ces_component_type_scene))
        {
            matrix = matrix * unsafe_get_scene_component(entity)->get_camera()->get_mat_v();
        }
        return matrix;
    }
}