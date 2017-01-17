//
//  ces_text_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_base_component.h"

namespace gb
{
    class ces_text_component : public ces_base_component
    {
    private:
        
        static const std::array<i32, 64> get_letters_sizes();
        static i32 convert_symbol_to_index(i32 c_val);
        
    protected:
        
        std::string m_text;
        bool m_is_text_changed;
        
        ui32 m_font_size;
        glm::u8vec4 m_font_color;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
        mesh_shared_ptr m_mesh;
        
    public:
        
        CTTI_CLASS_GUID(ces_text_component, ces_base_component::g_guids_container)
        ces_text_component();
        ~ces_text_component();
        
        mesh_shared_ptr generate_geometry();
        
        void set_font_size(ui32 size);
        ui32 get_font_size() const;
        
        void set_font_color(const glm::u8vec4& color);
        glm::u8vec4 get_font_color() const;
        
        void set_text(const std::string& text);
        std::string get_text() const;
        
        bool is_text_changed() const;
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        void reset();
    };
};

#endif
