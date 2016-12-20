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
#include "mesh.h"
#include "vbo.h"
#include "glm_extensions.h"

namespace gb
{
    static const i32 k_min_font_size = 8;
    static const i32 k_max_font_size = 32;
    
    text_label::text_label()
    {
        ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
        ces_geometry_freeform_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
        ces_entity::add_component(geometry_component);
        
        ces_text_component_shared_ptr text_component = std::make_shared<ces_text_component>();
        ces_entity::add_component(text_component);
        
        text.setter([=](const std::string& text) {
            text_component->set_text(text);
            
            const auto& geometry_component = text_label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(text_component->generate_geometry());
            
            text_component->reset();
        });
        text.getter([=]() {
            return unsafe_get_text_component_from_this->get_text();
        });
        
        font_size.setter([=](i32 size) {
            text_component->set_font_size(size);
            
            const auto& geometry_component = text_label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(text_component->generate_geometry());
            
            text_component->reset();
        });
        font_size.getter([=]() {
            return text_component->get_font_size();
        });
        
        text_color.setter([=](const glm::u8vec4& color) {
            text_component->set_font_color(color);
            
            const auto& geometry_component = text_label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(text_component->generate_geometry());
            
            text_component->reset();
        });
        text_color.getter([=]() {
            return text_component->get_font_color();
        });
        
        size.setter([=](const glm::vec2& size) {
            text_component->set_font_size(size.y);
            
            const auto& geometry_component = text_label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(text_component->generate_geometry());
            
            text_component->reset();
        });
        size.getter([=]() {
            return text_component->get_max_bound();
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
