//
//  ces_font_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_font_component.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh_2d.h"

namespace gb
{
    static const f32 k_text_default_size = 24.f;
    static const f32 k_glyph_spacing = 2.f;
    static const i32 k_max_symbols = 256;
    static const i32 k_max_num_vertices = k_max_symbols * 4;
    static const i32 k_max_num_indices = k_max_symbols * 6;
    
    ces_font_component::ces_font_component() :
    m_text("undefined"),
    m_mesh(nullptr),
    m_font_size(k_text_default_size),
    m_font_color(0.f, 0.f, 0.f, 1.f),
    m_min_bound(glm::vec2(0.f)),
    m_max_bound(glm::vec2(0.f)),
    m_atlas_width(0),
    m_atlas_height(0)
    {
        atlas_width.setter([=](i32 width) {
            m_atlas_width = width;
        });
        
        atlas_width.getter([=]() {
            return m_atlas_width;
        });
        
        atlas_height.setter([=](i32 height) {
            m_atlas_height = height;
        });
        
        atlas_height.getter([=]() {
            return m_atlas_height;
        });
        
#if !defined(__NO_RENDER__)
        
        auto vbo = std::make_shared<gb::vbo<vertex_attribute>>(k_max_num_vertices, GL_DYNAMIC_DRAW);
        
#else
        
        auto vbo = std::make_shared<gb::vbo<vertex_attribute>>(k_max_num_vertices, 0);
        
#endif
        
        vertex_attribute *vertices = vbo->lock();
        memset(vertices, 0x0, k_max_num_vertices * sizeof(vertex_attribute));
        vbo->unlock();
        
#if !defined(__NO_RENDER__)
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW);
        
#else
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, 0);
        
#endif
        ui16* indices = ibo->lock();
        memset(indices, 0x0, k_max_num_indices * sizeof(ui16));
        ibo->unlock();
        
        m_mesh = std::make_shared<gb::mesh_2d<vertex_attribute>>(vbo, ibo);
    }
    
    ces_font_component::~ces_font_component()
    {
        
    }
    
    std::shared_ptr<mesh_2d<vertex_attribute>> ces_font_component::generate_geometry(bool is_batching)
    {
        glm::vec2 position = glm::vec2(0.f);
        
        vertex_attribute* vertices = m_mesh->get_vbo()->lock();
        ui16* indices = m_mesh->get_ibo()->lock();
        
        i32 vertices_offset = 0;
        i32 indices_offset = 0;
        
        for(const char& symbol : m_text)
        {
            i32 glyph_index = static_cast<i32>(symbol);
            
            auto iterator = m_glyphs.find(glyph_index);
            assert(iterator != m_glyphs.end());
            
            if(glyph_index == -1 || glyph_index == 32 || iterator == m_glyphs.end())
            {
                position.x += m_font_size;
                continue;
            }
            
            f32 glyph_position_x = std::get<0>(iterator->second);
            f32 glyph_position_y = std::get<1>(iterator->second);
            f32 glyph_width = std::get<2>(iterator->second);
            f32 glyph_height = std::get<3>(iterator->second);
            __unused f32 glyph_x_offset = std::get<4>(iterator->second);
            __unused f32 glyph_y_offset = std::get<5>(iterator->second);
            
            f32 v_0 = glyph_position_x / m_atlas_width;
            f32 v_1 = v_0 + glyph_width / m_atlas_width;
            f32 u_0 = 1.f - glyph_position_y / m_atlas_height;
            f32 u_1 = u_0 - glyph_height / m_atlas_height;
            
            glyph_width += glyph_x_offset;
            glyph_height += glyph_y_offset;
            
            f32 font_scale = m_font_size / glyph_height;
            glyph_width *= font_scale;
            glyph_height *= font_scale;
            
            glyph_width -= glyph_x_offset;
            glyph_height -= glyph_y_offset;
            
            vertices[vertices_offset++].m_position = glm::vec3(position.x + glyph_x_offset, position.y + glyph_height + glyph_y_offset, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x + glyph_x_offset, position.y + glyph_y_offset, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x + glyph_width + glyph_x_offset, position.y + glyph_y_offset, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x + glyph_width + glyph_x_offset, position.y + glyph_height + glyph_y_offset, 0.f);
            
            vertices_offset -= 4;
            
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(v_0 + 0.0001f, u_1 - 0.0001f));
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(v_0 + 0.0001f, u_0 + 0.0001f));
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(v_1 - 0.0001f, u_0 + 0.0001f));
            vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(v_1 - 0.0001f, u_1 - 0.0001f));
            
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
            
            position.x += glyph_width + k_glyph_spacing + glyph_x_offset;
        }
        m_max_bound = glm::vec2(position.x, m_font_size);
        
        m_mesh->get_vbo()->unlock(is_batching, vertices_offset);
        m_mesh->get_ibo()->unlock(is_batching, indices_offset);
        
        return m_mesh;
    }
    
    void ces_font_component::set_font_size(f32 size)
    {
        m_font_size = size;
    }
    
    f32 ces_font_component::get_font_size() const
    {
        return m_font_size;
    }
    
    void ces_font_component::set_font_color(const glm::u8vec4& color)
    {
        m_font_color = color;
    }
    
    glm::u8vec4 ces_font_component::get_font_color() const
    {
        return m_font_color;
    }
    
    void ces_font_component::set_text(const std::string& text)
    {
        m_text = text.length() == 0 ? " " : text;
        /*std::transform(m_text.begin(), m_text.end(), m_text.begin(), ::toupper);*/
    }
    
    std::string ces_font_component::get_text() const
    {
        return m_text;
    }
    
    glm::vec2 ces_font_component::get_min_bound() const
    {
        return m_min_bound;
    }
    
    glm::vec2 ces_font_component::get_max_bound() const
    {
        return m_max_bound;
    }
    
    void ces_font_component::add_glyph(i32 id, i32 position_x, i32 position_y, i32 width, i32 height,
                                       f32 x_offset, f32 y_offset)
    {
        m_glyphs[id] = std::make_tuple(position_x, position_y, width, height, x_offset, y_offset);
    }
}
