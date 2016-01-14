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
#include "mesh.h"
#include "glm_extensions.h"

namespace gb
{
    sprite::sprite()
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_quad_component>();
        ces_entity::add_component(geometry_component);
    }
    
    sprite::~sprite()
    {
        
    }
    
    void sprite::set_size(const glm::vec2& size)
    {
        unsafe_get_geometry_quad_component_from_this->set_size(size);
    }
    
    glm::vec2 sprite::get_size() const
    {
        return unsafe_get_geometry_quad_component_from_this->get_size();
    }
    
    void sprite::set_pivot(const glm::vec2& pivot)
    {
        unsafe_get_geometry_quad_component_from_this->set_pivot(pivot);
    }
    
    glm::vec2 sprite::get_pivot() const
    {
        return unsafe_get_geometry_quad_component_from_this->get_pivot();
    }
    
    void sprite::set_cast_shadow(bool value)
    {
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
            ces_entity::remove_component(e_ces_component_type_shadow);
            ces_entity::remove_component(e_ces_component_type_convex_hull);
        }
    }
    
    bool sprite::is_cast_shadow() const
    {
        return ces_entity::is_component_exist(e_ces_component_type_shadow);
    }
    
    glm::vec2 sprite::get_min_bound() const
    {
        glm::vec2 min_bound = glm::vec2(0.f);
        ces_geometry_component* geometry_component = unsafe_get_geometry_component_from_this;
        if(geometry_component)
        {
            ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
            min_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_min_bound(),
                                       transformation_component->get_matrix_m()) - transformation_component->get_position();
        }
        return min_bound;
    }
    
    glm::vec2 sprite::get_max_bound() const
    {
        glm::vec2 max_bound = glm::vec2(0.f);
        ces_geometry_component* geometry_component = unsafe_get_geometry_component_from_this;
        if(geometry_component)
        {
            ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
            max_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_max_bound(),
                                       transformation_component->get_matrix_m())- transformation_component->get_position();
        }
        return max_bound;
    }
    
    glm::vec4 sprite::get_bound_frame() const
    {
        glm::vec4 bound_frame = glm::vec4(0.f);
        ces_geometry_component* geometry_component = unsafe_get_geometry_component_from_this;
        if(geometry_component && geometry_component->get_mesh())
        {
            ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
            glm::vec2 min_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_min_bound(),
                                                 transformation_component->get_matrix_m()) - transformation_component->get_position();
            glm::vec2 max_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_max_bound(),
                                                 transformation_component->get_matrix_m()) - transformation_component->get_position();
            bound_frame = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
        }
        return bound_frame;
    }
}