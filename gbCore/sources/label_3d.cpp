//
//  label_3d.cpp
//  gbCore
//
//  Created by serhii.s on 4/4/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "label_3d.h"
#include "ces_geometry_3d_component.h"
#include "ces_font_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_material_component.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "glm_extensions.h"

namespace gb
{
    label_3d::label_3d()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_font_component>();
        
        text.setter([=](const std::string& text) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            auto material_component = ces_entity::get_component<ces_material_component>();
            font_component->set_text(text);
            geometry_component->set_mesh(font_component->request_mesh_3d());
            material_component->set_texture(font_component->get_texture(), gb::e_shader_sampler_01);
        });
        text.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_text();
        });
        
        font_size.setter([=](f32 size) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            auto material_component = ces_entity::get_component<ces_material_component>();
            font_component->set_font_size(size);
            geometry_component->set_mesh(font_component->request_mesh_3d());
            material_component->set_texture(font_component->get_texture(), gb::e_shader_sampler_01);
        });
        font_size.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_font_size();
        });
        
        font_color.setter([=](const glm::u8vec4& color) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
            auto material_component = ces_entity::get_component<ces_material_component>();
            font_component->set_font_color(color);
            geometry_component->set_mesh(font_component->request_mesh_3d());
            material_component->set_texture(font_component->get_texture(), gb::e_shader_sampler_01);
        });
        font_color.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_font_color();
        });
        
        /*size.setter([=](const glm::vec2& size) {
            auto font_component = ces_entity::get_component<ces_font_component>();
            auto geometry_component = ces_entity::get_component<ces_geometry_freeform_component>();
            auto material_component = ces_entity::get_component<ces_material_component>();
            font_component->set_font_size(size.y);
            geometry_component->set_mesh(font_component->update());
            material_component->set_texture(font_component->get_texture(), gb::e_shader_sampler_01);
        });
        size.getter([=]() {
            auto font_component = ces_entity::get_component<ces_font_component>();
            return font_component->get_max_bound();
        });*/
    }
    
    void label_3d::set_multiline(bool value, i32 max_line_width)
    {
        auto font_component = ces_entity::get_component<ces_font_component>();
        auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
        auto material_component = ces_entity::get_component<ces_material_component>();
        font_component->set_multiline(value, max_line_width);
        geometry_component->set_mesh(font_component->request_mesh_3d());
    }
}