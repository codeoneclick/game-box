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
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_freeform_component>();
        ces_entity::add_deferred_component_constructor<ces_font_component>();
        
        text.setter([=](const std::string& text) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_freeform_component>();
            font_component->set_text(text);
            geometry_component->set_mesh(font_component->update());
        });
        text.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_text();
        });
        
        font_size.setter([=](f32 size) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_freeform_component>();
            font_component->set_font_size(size);
            geometry_component->set_mesh(font_component->update());
        });
        font_size.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_font_size();
        });
        
        font_color.setter([=](const glm::u8vec4& color) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_freeform_component>();
            font_component->set_font_color(color);
            geometry_component->set_mesh(font_component->update());
        });
        font_color.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_font_color();
        });
        
        size.setter([=](const glm::vec2& size) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_freeform_component>();
            font_component->set_font_size(size.y);
            geometry_component->set_mesh(font_component->update());
        });
        size.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_max_bound();
        });
    }
    
    label::~label()
    {
        
    }
}
