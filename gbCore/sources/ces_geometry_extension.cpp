//
//  ces_geometry_extension.cpp
//  gbCore
//
//  Created by Serhii Serhiiv on 2/16/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_extension.h"
#include "ces_transformation_component.h"
#include "ces_entity.h"
#include "mesh.h"
#include "glm_extensions.h"
#include "ces_transformation_extension.h"

namespace gb
{
    glm::vec4 ces_geometry_extension::get_absolute_bound(const ces_entity_shared_ptr& entity)
    {
        glm::vec4 bound = glm::vec4(0.f);
        ces_geometry_component* geometry_component = unsafe_get_geometry_component(entity);
        ces_transformation_component* transformation_component = unsafe_get_transformation_component(entity);
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
}
