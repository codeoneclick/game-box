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
    
    glm::mat4 ces_transformation_extension::get_absolute_transformation_in_world_space(const ces_entity_shared_ptr& entity)
    {
        ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_component>();
        glm::mat4 matrix = transformation_component->get_absolute_transformation();
        return matrix;
    }

    glm::mat4 ces_transformation_extension::get_absolute_transformation_in_camera_space(const ces_entity_shared_ptr& entity)
    {
        glm::mat4 matrix = ces_transformation_extension::get_absolute_transformation_in_world_space(entity);
        if(entity->is_component_exist(ces_scene_component::class_guid()))
        {
            matrix = matrix * unsafe_get_scene_component(entity)->get_camera()->get_mat_v();
        }
        return matrix;
    }
    
    void ces_transformation_extension::update_absolute_transformation_recursively(const ces_entity_shared_ptr& entity)
    {
        ces_entity_shared_ptr parent = entity->parent;
        if(parent)
        {
            ces_transformation_component_shared_ptr parent_transformation_component = parent->get_component<ces_transformation_component>();
            ces_transformation_component_shared_ptr child_transformation_component = entity->get_component<ces_transformation_component>();
            if(parent_transformation_component && child_transformation_component)
            {
                child_transformation_component->update_absolute_transformation(parent_transformation_component->get_absolute_transformation());
            }
        }
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_transformation_extension::update_absolute_transformation_recursively(child);
        }
    }

}
