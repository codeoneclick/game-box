//
//  text_label.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

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
            return text_component->get_text();
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
        
        font_color.setter([=](const glm::u8vec4& color) {
            text_component->set_font_color(color);
            
            const auto& geometry_component = text_label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(text_component->generate_geometry());
            
            text_component->reset();
        });
        font_color.getter([=]() {
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
    }
    
    text_label::~text_label()
    {
        
    }
}

#endif
