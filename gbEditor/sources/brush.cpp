//
//  brush.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/5/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "brush.h"
#include "ces_geometry_quad_component.h"
#include "ces_material_component.h"
#include "ces_transformation_component.h"
#include "mesh.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ed
    {
        static const std::string k_radius_uniform = "u_radius";
        static const std::string k_center_uniform = "u_center";
        
        brush::brush() :
        m_radius(32.f)
        {
            ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_quad_component>();
            ces_entity::add_component(geometry_component);
        }
        
        brush::~brush()
        {
            
        }
        
        void brush::set_radius(f32 radius)
        {
            m_radius = radius;
            
            ces_geometry_component* geometry_component = unsafe_get_geometry_component_from_this;
            if(geometry_component && geometry_component->get_mesh())
            {
                glm::vec2 min_bound = geometry_component->get_mesh()->get_vbo()->get_min_bound();
                glm::vec2 max_bound = geometry_component->get_mesh()->get_vbo()->get_max_bound();
                glm::vec2 origin_size = glm::vec2(fabsf(min_bound.x) + fabsf(max_bound.x),
                                                  fabsf(min_bound.y) + fabsf(max_bound.y));
                unsafe_get_transformation_component_from_this->set_scale(glm::vec2(m_radius / origin_size.x,
                                                                                   m_radius / origin_size.y));
            }
            else
            {
                assert(false);
            }
            
            ces_material_component* material_component = unsafe_get_material_component_from_this;
            material_component->set_custom_shader_uniform(m_radius * .5f, k_radius_uniform);
        }
        
        f32 brush::get_radius() const
        {
            return m_radius;
        }
        
        void brush::set_position(const glm::vec2 &position)
        {
            game_object::set_position(position);
            ces_material_component* material_component = unsafe_get_material_component_from_this;
            material_component->set_custom_shader_uniform(glm::vec2(position.x + m_radius * .5f,
                                                                    position.y + m_radius * .5f), k_center_uniform);
        }
        
        glm::vec4 brush::get_bound() const
        {
            glm::vec4 bound = glm::vec4(0.f);
            ces_geometry_component* geometry_component = unsafe_get_geometry_component_from_this;
            if(geometry_component && geometry_component->get_mesh())
            {
                ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
                glm::vec2 min_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_min_bound(),
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                glm::vec2 max_bound = glm::transform(geometry_component->get_mesh()->get_vbo()->get_max_bound(),
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            }
            return bound;
        }
    }
}