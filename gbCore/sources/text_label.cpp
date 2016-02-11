//
//  text_label.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "text_label.h"
#include "ces_geometry_freeform_component.h"
#include "ces_text_component.h"
#include "ces_transformation_component.h"
#include "ces_material_component.h"
#include "glm_extensions.h"

namespace gb
{
    static const i32 k_min_font_height = 1;
    static const i32 k_max_font_height = 32;
    static const std::string k_text_color_uniform = "u_color";
    
    text_label::text_label()
    {
        ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
        ces_entity::add_component(geometry_component);
        
        ces_text_component_shared_ptr text_component = std::make_shared<ces_text_component>();
        ces_entity::add_component(text_component);
        
        text.setter([=](const std::string& text) {
           unsafe_get_text_component_from_this->set_text(text);
        });
        text.getter([=]() {
            return unsafe_get_text_component_from_this->get_text();
        });
        
        font_height.setter([=](i32 height) {
            m_font_height = std::min(k_max_font_height, std::max(k_min_font_height, height));
            unsafe_get_transformation_component_from_this->set_scale(glm::vec2(static_cast<f32>(m_font_height) / static_cast<f32>(k_max_font_height)));
        });
        font_height.getter([=]() {
            return m_font_height;
        });
        
        text_color.setter([=](const glm::vec4& color) {
            m_text_color = color;
            unsafe_get_material_component_from_this->set_custom_shader_uniform(color, k_text_color_uniform);
        });
        text_color.getter([=]() {
            return m_text_color;
        });
        
        bound.getter([=]() {
            glm::vec4 bound = glm::vec4(0.f);
            ces_text_component* text_component = unsafe_get_text_component_from_this;
            if(text_component)
            {
                ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
                glm::vec2 min_bound = glm::transform(text_component->get_min_bound(),
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                glm::vec2 max_bound = glm::transform(text_component->get_max_bound(),
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                bound = glm::vec4(min_bound, max_bound);
            }
            return bound;
        });
    }
    
    text_label::~text_label()
    {
        
    }
}