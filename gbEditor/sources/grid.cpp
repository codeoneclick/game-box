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

namespace gb
{
    namespace ed
    {
        static const std::string k_color_uniform = "u_color";
        
        grid::grid() :
        m_color(0.f)
        {
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
    }
}