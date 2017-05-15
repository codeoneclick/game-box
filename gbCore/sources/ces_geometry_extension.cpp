//
//  ces_geometry_extension.cpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/16/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_extension.h"
#include "ces_transformation_2d_component.h"
#include "ces_entity.h"
#include "mesh.h"
#include "vbo.h"
#include "glm_extensions.h"
#include "ces_transformation_extension.h"

namespace gb
{
    glm::vec4 ces_geometry_extension::get_absolute_bound(const ces_entity_shared_ptr& entity)
    {
        glm::vec4 bound = glm::vec4(0.f);
        auto geometry_component = entity->get_unsafe_component<ces_geometry_component>();
        auto transformation_component = entity->get_unsafe_component<ces_transformation_2d_component>();
        if(geometry_component && geometry_component->get_mesh())
        {
            glm::mat4 absolute_transformation = transformation_component->get_absolute_transformation();
            glm::vec2 min_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_min_bound(),
                                                 absolute_transformation);
            glm::vec2 max_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_max_bound(),
                                                 absolute_transformation);
            bound = glm::vec4(min_bound, max_bound);
        }
        return bound;
    }
    
    glm::vec4 ces_geometry_extension::get_absolute_position_bounds(const ces_entity_shared_ptr& entity)
    {
        glm::vec4 bound = glm::vec4(0.f);
        auto geometry_component = entity->get_component<ces_geometry_component>();
        auto transformation_component = entity->get_component<ces_transformation_2d_component>();
        if(geometry_component && geometry_component->get_mesh())
        {
            glm::mat4 absolute_transformation = transformation_component->get_absolute_transformation();
            glm::vec2 min_bound = geometry_component->get_mesh()->get_vbo()->get_min_bound();
            bound.x = min_bound.x + absolute_transformation[3][0];
            bound.y = min_bound.y + absolute_transformation[3][1];
            
            glm::vec2 max_bound = geometry_component->get_mesh()->get_vbo()->get_max_bound();
            bound.z = max_bound.x + absolute_transformation[3][0];
            bound.w = max_bound.y + absolute_transformation[3][1];
        }
        return bound;
    }
}
