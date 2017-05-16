//
//  label.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "label.h"
#include "ces_geometry_freeform_component.h"
#include "ces_font_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_material_component.h"
#include "ces_luminous_component.h"
#include "mesh_2d.h"
#include "vbo.h"
#include "glm_extensions.h"

namespace gb
{
    label::label()
    {
        
#if !defined(__NO_RENDER__)
        
        ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
#endif
        
        ces_geometry_freeform_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
        ces_entity::add_component(geometry_component);
        
        ces_font_component_shared_ptr font_component = std::make_shared<ces_font_component>();
        ces_entity::add_component(font_component);
        
        text.setter([=](const std::string& text) {
            font_component->set_text(text);
            
            const auto& geometry_component = label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(font_component->generate_geometry(material_component->get_is_batching()));
        });
        text.getter([=]() {
            return font_component->get_text();
        });
        
        font_size.setter([=](f32 size) {
            font_component->set_font_size(size);
            
            const auto& geometry_component = label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(font_component->generate_geometry(material_component->get_is_batching()));
        });
        font_size.getter([=]() {
            return font_component->get_font_size();
        });
        
        font_color.setter([=](const glm::u8vec4& color) {
            font_component->set_font_color(color);
            
            const auto& geometry_component = label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(font_component->generate_geometry(material_component->get_is_batching()));
        });
        font_color.getter([=]() {
            return font_component->get_font_color();
        });
        
        size.setter([=](const glm::vec2& size) {
            font_component->set_font_size(size.y);
            
            const auto& geometry_component = label::get_component<ces_geometry_freeform_component>();
            geometry_component->set_mesh(font_component->generate_geometry(material_component->get_is_batching()));
        });
        size.getter([=]() {
            return font_component->get_max_bound();
        });
        
        is_luminous.setter([=](bool value) {
            
#if !defined(__NO_RENDER__)
            
            if(value)
            {
                ces_entity::add_component_recursively<ces_luminous_component>();
            }
            else
            {
                ces_entity::remove_component_recursively<ces_luminous_component>();
            }
            
#endif
            
        });
        
        is_luminous.getter([=]() {
            
#if !defined(__NO_RENDER__)
            
            return ces_entity::is_component_exist<ces_luminous_component>();
            
#else
            
            return false;
            
#endif
            
        });
    }
    
    label::~label()
    {
        
    }
}
