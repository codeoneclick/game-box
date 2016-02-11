//
//  grid.cpp
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "grid.h"
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
        
        grid::grid() :
        m_color(0.f)
        {
            ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
            ces_entity::add_component(material_component);
            
            ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
            ces_entity::add_component(geometry_component);
        }
        
        grid::~grid()
        {
            
        }
        
        void grid::set_color(const glm::vec4& color)
        {
            m_color = color;
            
            ces_material_component* material_component = unsafe_get_material_component_from_this;
            material_component->set_custom_shader_uniform(m_color, k_color_uniform);
        }
        
        glm::vec4 grid::get_color() const
        {
            return m_color;
        }
        
        glm::vec4 grid::get_bound() const
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