//
//  ces_font_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_font_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::string m_text;
        
        f32 m_font_size;
        glm::u8vec4 m_font_color;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
        mesh_2d_shared_ptr m_mesh;
        
        i32 m_atlas_width;
        i32 m_atlas_height;
        
        std::unordered_map<i32, std::tuple<i32, i32, i32, i32, f32, f32>> m_glyphs;
        
    public:
        
        CTTI_CLASS_GUID(ces_font_component, ces_base_component::g_guids_container)
        ces_font_component();
        ~ces_font_component();
        
        mesh_2d_shared_ptr generate_geometry(bool is_batching);
        
        std::property_rw<i32> atlas_width;
        std::property_rw<i32> atlas_height;
        
        void set_font_size(f32 size);
        f32 get_font_size() const;
        
        void set_font_color(const glm::u8vec4& color);
        glm::u8vec4 get_font_color() const;
        
        void set_text(const std::string& text);
        std::string get_text() const;
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        void add_glyph(i32 id, i32 position_x, i32 position_y, i32 width, i32 height,
                       f32 x_offset, f32 y_offset);
    };
};
