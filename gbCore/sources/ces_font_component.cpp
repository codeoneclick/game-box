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
#include "texture.h"
#include "common.h"

namespace gb
{
    static const f32 k_text_default_size = 24.f;
    static const f32 k_glyph_spacing = 2.f;
    static const i32 k_max_symbols = 256;
    static const i32 k_max_num_vertices = k_max_symbols * 4;
    static const i32 k_max_num_indices = k_max_symbols * 6;
    std::unordered_map<std::string, texture_shared_ptr> ces_font_component::m_font_atlases;
    
    ces_font_component::ces_font_component() :
    m_text("undefined"),
    m_mesh(nullptr),
    m_font_size(k_text_default_size),
    m_font_name("Vera.ttf"),
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
        
        vbo_shared_ptr vbo = nullptr;
        std::shared_ptr<vbo::vertex_declaration_PTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTC>(k_max_num_vertices);
        
#if !defined(__NO_RENDER__)
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, GL_DYNAMIC_DRAW, true);
        
#else
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, 0, true);
        
#endif
        
        vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
        memset(vertices, 0x0, k_max_num_vertices * sizeof(vbo::vertex_attribute_PTC));
        vbo->unlock();
        
#if !defined(__NO_RENDER__)
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, GL_DYNAMIC_DRAW, true);
        
#else
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(k_max_num_indices, 0, true);
        
#endif
        ui16* indices = ibo->lock();
        memset(indices, 0x0, k_max_num_indices * sizeof(ui16));
        ibo->unlock();
        
        m_mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
    }
    
    ces_font_component::~ces_font_component()
    {
        
    }
    
    mesh_2d_shared_ptr ces_font_component::update()
    {
        ftgl::texture_atlas_t* atlas = ftgl::texture_atlas_new(512, 512, 1);
        ftgl::texture_font_t* font = texture_font_new_from_file(atlas, m_font_size, bundlepath().append(m_font_name).c_str());
        texture_font_load_glyphs(font, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!№;%:?*()_+-=.,/|\"'@#$^&{}[]");
        
        std::stringstream font_guid;
        font_guid<<m_font_name<<m_font_size;
        
        gl_create_textures(1, &atlas->id);
        gl_bind_texture( GL_TEXTURE_2D, atlas->id );
        gl_texture_parameter_i( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
        gl_texture_parameter_i( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        gl_texture_parameter_i( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        gl_texture_parameter_i( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        gl_texture_image2d( GL_TEXTURE_2D, 0, GL_LUMINANCE,  static_cast<i32>(atlas->width),  static_cast<i32>(atlas->height), 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, atlas->data );
        auto font_texture = gb::texture::construct(font_guid.str(), atlas->id, static_cast<i32>(atlas->width),  static_cast<i32>(atlas->height));
        m_font_atlases.insert(std::make_pair(font_guid.str(), font_texture));
        
        /*const char * filename = "fonts/Vera.ttf";
        char * text = "A Quick Brown Fox Jumps Over The Lazy Dog 0123456789";
        buffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f" );
        vec2 pen = {{5,400}};
        vec4 black = {{0,0,0,1}};
        for( i=7; i < 27; ++i)
        {
            font = texture_font_new_from_file( atlas, i, filename );
            pen.x = 5;
            pen.y -= font->height;
            texture_font_load_glyphs( font, text );
            add_text( buffer, font, text, &black, &pen );
            texture_font_delete( font );
        }*/
        
        glm::vec2 position = glm::vec2(0.f);
        
        vbo::vertex_attribute_PTC* vertices = m_mesh->get_vbo()->lock<vbo::vertex_attribute_PTC>();
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
            f32 glyph_x_offset = std::get<4>(iterator->second);
            f32 glyph_y_offset = std::get<5>(iterator->second);
            
            f32 v_0 = glyph_position_x / m_atlas_width;
            f32 v_1 = v_0 + glyph_width / m_atlas_width;
            f32 u_0 = 1.f - glyph_position_y / m_atlas_height;
            f32 u_1 = u_0 - glyph_height / m_atlas_height;
            
            f32 font_scale = m_font_size / glyph_height;
            glyph_width *= font_scale;
            glyph_height *= font_scale;
            
            vertices[vertices_offset++].m_position = glm::vec3(position.x, position.y + glyph_height, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x, position.y, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x + glyph_width, position.y, 0.f);
            vertices[vertices_offset++].m_position = glm::vec3(position.x + glyph_width, position.y + glyph_height, 0.f);
            
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
        
        m_mesh->get_vbo()->unlock(vertices_offset);
        m_mesh->get_ibo()->unlock(indices_offset);
        
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
