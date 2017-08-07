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
        
        static std::unordered_map<std::string, std::tuple<ftgl::texture_font_t*, ftgl::texture_atlas_t*, texture_shared_ptr>> m_font_atlases;
        
    protected:
        
        std::string m_text;
        
        i32 m_font_size;
        std::string m_font_name;
        glm::u8vec4 m_font_color;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
        mesh_2d_shared_ptr m_mesh;
        texture_shared_ptr m_texture;
        
    public:
        
        CTTI_CLASS_GUID(ces_font_component, ces_base_component::g_guids_container)
        ces_font_component();
        ~ces_font_component();
        
        mesh_2d_shared_ptr update();
        
        void set_font_size(i32 size);
        f32 get_font_size() const;
        
        void set_font_name();
        
        void set_font_color(const glm::u8vec4& color);
        glm::u8vec4 get_font_color() const;
        
        void set_text(const std::string& text);
        std::string get_text() const;
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        texture_shared_ptr get_texture() const;
    };
};
