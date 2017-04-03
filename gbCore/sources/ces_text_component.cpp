//
//  ces_text_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_text_component.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh.h"

namespace gb
{
    static const f32 k_text_uv_box_width = .125f;
    static const f32 k_text_default_size = 32.f;
    static const f32 k_text_spacesize = 8.f;
    static const i32 k_max_symbols = 256;
    static const i32 k_max_num_vertices = k_max_symbols * 4;
    static const i32 k_max_num_indices = k_max_symbols * 6;
    
    ces_text_component::ces_text_component() :
    m_is_text_changed(false),
    m_text("undefined"),
    m_mesh(nullptr),
    m_font_size(k_text_default_size),
    m_font_color(0.f, 0.f, 0.f, 1.f),
    m_min_bound(glm::vec2(0.f)),
    m_max_bound(glm::vec2(0.f))
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(k_max_num_vertices, GL_DYNAMIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        memset(vertices, 0x0, k_max_num_vertices * sizeof(vbo::vertex_attribute));
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW);
        ui16* indices = ibo->lock();
        memset(indices, 0x0, k_max_num_indices * sizeof(ui16));
        ibo->unlock();
        
        m_mesh = std::make_shared<gb::mesh>(vbo, ibo);
    }
    
    ces_text_component::~ces_text_component()
    {
        
    }
    
    const std::array<i32, 64> ces_text_component::get_letters_sizes()
    {
        static std::array<i32, 64> letters_sizes =
        {
            36, 29, 30, 34, 25, 25, 34, 33,
            11, 20, 31, 24, 48, 35, 39, 29,
            42, 31, 27, 31, 34, 35, 46, 35,
            31, 27, 30, 26, 28, 26, 31, 28,
            28, 28, 29, 29, 14, 24, 30, 18,
            26, 14, 14, 14, 25, 28, 31, 0,
            0, 38, 39, 12, 36, 34, 0, 0,
            0, 38, 0, 0, 0, 0, 0, 0
        };
        return letters_sizes;
    }
    
    i32 ces_text_component::convert_symbol_to_index(i32 c_val)
    {
        i32 index = -1;
        
        if(c_val > 64 && c_val < 91) // A-Z
        {
            index = c_val - 65;
        }
        else if(c_val > 96 && c_val < 123) // a-z
        {
            index = c_val - 97;
        }
        else if(c_val > 47 && c_val < 58) // 0-9
        {
            index = c_val - 48 + 26;
        }
        else if(c_val == 43) // +
        {
            index = 38;
        }
        else if(c_val == 45) // -
        {
            index = 39;
        }
        else if(c_val == 33) // !
        {
            index = 36;
        }
        else if(c_val == 63) // ?
        {
            index = 37;
        }
        else if(c_val == 61) // =
        {
            index = 40;
        }
        else if(c_val == 58) // :
        {
            index = 41;
        }
        else if(c_val == 46) // .
        {
            index = 42;
        }
        else if(c_val == 44) // ,
        {
            index = 43;
        }
        else if(c_val == 42) // *
        {
            index = 44;
        }
        else if(c_val == 36) // $
        {
            index = 45;
        }
        return index;
    }
    
    mesh_shared_ptr ces_text_component::generate_geometry()
    {
        glm::vec2 position = glm::vec2(0.f);
        
        vbo::vertex_attribute* vertices = m_mesh->get_vbo()->lock();
        ui16* indices = m_mesh->get_ibo()->lock();
        
        i32 vertices_offset = 0;
        i32 indices_offset = 0;
        
        for(const char& symbol : m_text)
        {
            i32 c_val = static_cast<i32>(symbol);
            
            i32 index = ces_text_component::convert_symbol_to_index(c_val);
            
            if(index == -1)
            {
                position.x += k_text_spacesize;
                continue;
            }
            
            i32 row = 7 - index / 8;
            i32 column = index % 8;
            
            f32 v1 = row * k_text_uv_box_width;
            f32 v2 = v1 + k_text_uv_box_width;
            f32 u1 = column * k_text_uv_box_width;
            f32 u2 = u1 + k_text_uv_box_width;
            
            vertices[vertices_offset++].m_position = glm::vec3(position.x, position.y + m_font_size, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x, position.y, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x + m_font_size, position.y, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x + m_font_size, position.y + m_font_size, 0.f);
            
            vertices_offset -= 4;
            
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u1 + .001f, v1 + .001f));
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u1 + .001f, v2 - .001f));
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u2 - .001f, v2 - .001f));
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u2 - .001f, v1 + .001f));
            
            vertices_offset -= 4;
            
            vertices[vertices_offset++].m_color = m_font_color;
            vertices[vertices_offset++].m_color = m_font_color;
            vertices[vertices_offset++].m_color = m_font_color;
            vertices[vertices_offset++].m_color = m_font_color;
            
            indices[indices_offset++] = 0 + vertices_offset - 4;
            indices[indices_offset++] = 1 + vertices_offset - 4;
            indices[indices_offset++] = 2 + vertices_offset - 4;
            indices[indices_offset++] = 0 + vertices_offset - 4;
            indices[indices_offset++] = 2 + vertices_offset - 4;
            indices[indices_offset++] = 3 + vertices_offset - 4;
            
            assert(vertices_offset < k_max_num_vertices);
            assert(indices_offset < k_max_num_indices);
            
            position.x += m_font_size;
        }
        m_max_bound = glm::vec2(position.x, m_font_size);
        
        m_mesh->get_vbo()->unlock(vertices_offset);
        m_mesh->get_ibo()->unlock(indices_offset);
        
        return m_mesh;
    }
    
    void ces_text_component::set_font_size(f32 size)
    {
        m_font_size = size;
        m_is_text_changed = true;
    }
    
    f32 ces_text_component::get_font_size() const
    {
        return m_font_size;
    }
    
    void ces_text_component::set_font_color(const glm::u8vec4& color)
    {
        m_font_color = color;
        m_is_text_changed = true;
    }
    
    glm::u8vec4 ces_text_component::get_font_color() const
    {
        return m_font_color;
    }
    
    void ces_text_component::set_text(const std::string& text)
    {
        m_text = text.length() == 0 ? "." : text;
        m_is_text_changed = true;
    }
    
    std::string ces_text_component::get_text() const
    {
        return m_text;
    }
    
    bool ces_text_component::is_text_changed() const
    {
        return m_is_text_changed;
    }
    
    void ces_text_component::reset()
    {
        m_is_text_changed = false;
    }
    
    glm::vec2 ces_text_component::get_min_bound() const
    {
        return m_min_bound;
    }
    
    glm::vec2 ces_text_component::get_max_bound() const
    {
        return m_max_bound;
    }
}

#endif
