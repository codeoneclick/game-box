//
//  ces_transformation_extension.cpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_transformation_extension.h"
#include "ces_entity.h"
#include "camera.h"

namespace gb
{
    
    glm::mat4 ces_transformation_extension::get_absolute_transformation_in_ws(const ces_entity_shared_ptr& entity)
    {
        auto transformation_component = entity->get_unsafe_component<ces_transformation_component>();
        glm::mat4 matrix = transformation_component->get_absolute_transformation();
        return matrix;
    }

    glm::mat4 ces_transformation_extension::get_absolute_transformation_in_ss(const ces_entity_shared_ptr& entity, const camera_shared_ptr& camera)
    {
        glm::mat4 matrix = ces_transformation_extension::get_absolute_transformation_in_ws(entity);
        matrix = matrix * camera->get_mat_v();
        return matrix;
    }
    
    void ces_transformation_extension::update_absolute_transformation_recursively(const ces_entity_shared_ptr& entity)
    {
        ces_entity_shared_ptr parent = entity->parent;
        if(parent &&
           parent->is_component_exist<ces_transformation_component>() &&
           entity->is_component_exist<ces_transformation_component>())
        {
            auto parent_transformation_component = parent->get_unsafe_component<ces_transformation_component>();
            auto child_transformation_component = entity->get_unsafe_component<ces_transformation_component>();
            child_transformation_component->update_absolute_transformation(parent_transformation_component->get_absolute_transformation());
        }
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_transformation_extension::update_absolute_transformation_recursively(child);
        }
    }

}
