//
//  sprite.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "sprite.h"
#include "ces_geometry_quad_component.h"
#include "ces_shadow_component.h"
#include "ces_convex_hull_component.h"
#include "ces_transformation_component.h"
#include "ces_material_component.h"
#include "mesh.h"
#include "glm_extensions.h"

namespace gb
{
    sprite::sprite()
    {
        ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_quad_component>();
        ces_entity::add_component(geometry_component);
        
        size.setter([=](const glm::vec2& size) {
            unsafe_get_geometry_quad_component_from_this->set_size(size);
        });
        size.getter([=]() {
            return unsafe_get_geometry_quad_component_from_this->get_size();
        });
        
        pivot.setter([=](const glm::vec2& pivot) {
            unsafe_get_geometry_quad_component_from_this->set_pivot(pivot);
        });
        pivot.getter([=]() {
            return unsafe_get_geometry_quad_component_from_this->get_pivot();
        });
        
        cast_shadow.setter([=](bool value) {
            if(value)
            {
                ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<ces_convex_hull_component>();
                ces_geometry_component* geometry_component = unsafe_get_geometry_quad_component_from_this;
                convex_hull_component->create_convex_hull(geometry_component->get_mesh()->get_vbo()->lock(), geometry_component->get_mesh()->get_vbo()->get_used_size());
                ces_entity::add_component(convex_hull_component);
                
                ces_shadow_component_shared_ptr shadow_component = std::make_shared<ces_shadow_component>();
                ces_entity::add_component(shadow_component);
            }
            else
            {
                ces_entity::remove_component(ces_shadow_component::class_guid());
                ces_entity::remove_component(ces_convex_hull_component::class_guid());
            }
        });
        cast_shadow.getter([=]() {
            return ces_entity::is_component_exist(ces_shadow_component::class_guid());
        });
        
        bound.getter([=]() {
            glm::vec4 bound = glm::vec4(0.f);
            if(geometry_component && geometry_component->get_mesh())
            {
                ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
                glm::vec2 min_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_min_bound(),
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                glm::vec2 max_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_max_bound(),
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                bound = glm::vec4(min_bound, max_bound);
            }
            return bound;
        });
    }
    
    sprite::~sprite()
    {
        
    }
}
