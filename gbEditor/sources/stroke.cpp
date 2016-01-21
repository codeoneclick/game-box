//
//  stroke.cpp
//  gbEditor
//
//  Created by sergey.sergeev on 1/20/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "stroke.h"
#include "ces_geometry_freeform_component.h"
#include "ces_material_component.h"
#include "ces_transformation_component.h"
#include "mesh.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ed
    {
        static const std::string k_color_uniform = "u_color";
        
        stroke::stroke() :
        m_color(0.f)
        {
            ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
            ces_entity::add_component(geometry_component);
        }
        
        stroke::~stroke()
        {
            
        }
        
        void stroke::set_color(const glm::vec4& color)
        {
            m_color = color;
            
            ces_material_component* material_component = unsafe_get_material_component_from_this;
            material_component->set_custom_shader_uniform(m_color, k_color_uniform);
        }
        
        glm::vec4 stroke::get_color() const
        {
            return m_color;
        }
        
        void stroke::set_size(const glm::vec2& size)
        {
            ces_geometry_component* geometry_component = unsafe_get_geometry_component_from_this;
            if(geometry_component && geometry_component->get_mesh())
            {
                glm::vec2 min_bound = geometry_component->get_mesh()->get_vbo()->get_min_bound();
                glm::vec2 max_bound = geometry_component->get_mesh()->get_vbo()->get_max_bound();
                glm::vec2 origin_size = glm::vec2(fabsf(min_bound.x) + fabsf(max_bound.x),
                                                  fabsf(min_bound.y) + fabsf(max_bound.y));
                unsafe_get_transformation_component_from_this->set_scale(glm::vec2(size.x / origin_size.x,
                                                                                   size.y / origin_size.y));
            }
            else
            {
                assert(false);
            }
        }
        
        glm::vec4 stroke::get_bound() const
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