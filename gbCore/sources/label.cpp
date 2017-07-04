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
    
    label_shared_ptr label::construct()
    {
        auto entity = std::make_shared<label>();
        
#if !defined(__NO_RENDER__)
        
        auto material_component = std::make_shared<ces_material_component>();
        entity->add_component(material_component);
        
#endif
        
        auto geometry_component = std::make_shared<ces_geometry_freeform_component>();
        entity->add_component(geometry_component);
        
        auto font_component = std::make_shared<ces_font_component>();
        entity->add_component(font_component);
        
        return entity;
    }
}
