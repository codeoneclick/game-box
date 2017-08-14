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
    static const i32 k_text_default_size = 24;
    static const i32 k_max_symbols = 256;
    static const i32 k_max_num_vertices = k_max_symbols * 4;
    static const i32 k_max_num_indices = k_max_symbols * 6;
    std::unordered_map<std::string, std::tuple<ftgl::texture_font_t*, ftgl::texture_atlas_t*, texture_shared_ptr>> ces_font_component::m_font_atlases;
    
    ces_font_component::ces_font_component() :
    m_text(""),
    m_mesh(nullptr),
    m_font_size(k_text_default_size),
    m_font_name("Font.ttf"),
    m_font_color(0.f, 0.f, 0.f, 1.f),
    m_min_bound(glm::vec2(0.f)),
    m_max_bound(glm::vec2(0.f))
    {
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
        std::stringstream font_guid;
        font_guid<<m_font_name<<m_font_size;
        ftgl::texture_font_t* font = nullptr;
        auto font_it = m_font_atlases.find(font_guid.str());
        if(font_it == m_font_atlases.end())
        {
            ftgl::texture_atlas_t* atlas = ftgl::texture_atlas_new(256, 256, 1);
            font = texture_font_new_from_file(atlas, m_font_size, bundlepath().append(m_font_name).c_str());
            texture_font_load_glyphs(font, "a b c d e f g h i j k l m n o p q r s t u v w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 0 1 2 3 4 5 6 7 8 9 ! № ; % : ? * ( ) _ + - = . , / | \\ \" ' @ # $ ^ & { } [ ]");
            
            ui32 format;
            
#if defined(__OPENGL_30__)
            
            format = GL_RED;
            
#else
            
            format = GL_LUMINANCE;
            
#endif
            
            gl_create_textures(1, &atlas->id);
            gl_bind_texture(GL_TEXTURE_2D, atlas->id);
            gl_texture_image2d( GL_TEXTURE_2D, 0, format,  static_cast<i32>(atlas->width),  static_cast<i32>(atlas->height), 0, format, GL_UNSIGNED_BYTE, atlas->data);
            auto font_texture = gb::texture::construct(font_guid.str(), atlas->id, static_cast<i32>(atlas->width),  static_cast<i32>(atlas->height));
            font_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
            font_texture->set_mag_filter(GL_LINEAR);
            font_texture->set_min_filter(GL_LINEAR);
            m_font_atlases.insert(std::make_pair(font_guid.str(), std::make_tuple(font, atlas, font_texture)));
            m_texture = font_texture;
        }
        else
        {
            font = std::get<0>(font_it->second);
            m_texture = std::get<2>(font_it->second);
        }
        
        glm::vec2 position = glm::vec2(0.f);
        
        vbo::vertex_attribute_PTC* vertices = m_mesh->get_vbo()->lock<vbo::vertex_attribute_PTC>();
        ui16* indices = m_mesh->get_ibo()->lock();
        
        i32 vertices_offset = 0;
        i32 indices_offset = 0;
        i32 index = 0;
        for(const char& symbol : m_text)
        {
            ftgl::texture_glyph_t *glyph = texture_font_get_glyph(font, &symbol);
            if(glyph != nullptr)
            {
                f32 kerning =  0.f;
                if(index != 0)
                {
                    kerning = texture_glyph_get_kerning(glyph, &m_text.at(index - 1));
                }
                position.x += kerning;
                
                i32 offset_y = static_cast<i32>(glyph->height) - static_cast<i32>(glyph->offset_y);
                i32 x0 = position.x;
                i32 y0 = position.y;
                i32 x1 = x0 + static_cast<i32>(glyph->width);
                i32 y1 = y0 - static_cast<i32>(glyph->height);
                f32 s0 = glyph->s0;
                f32 t0 = glyph->t0;
                f32 s1 = glyph->s1;
                f32 t1 = glyph->t1;
                
                vertices[vertices_offset++].m_position = glm::vec3(x0, y0 + m_font_size * .85f + offset_y, 0.f);
                vertices[vertices_offset++].m_position = glm::vec3(x0, y1 + m_font_size * .85f + offset_y, 0.f);
                vertices[vertices_offset++].m_position = glm::vec3(x1, y1 + m_font_size * .85f + offset_y, 0.f);
                vertices[vertices_offset++].m_position = glm::vec3(x1, y0 + m_font_size * .85f + offset_y, 0.f);
                
                vertices_offset -= 4;
                
                glm::vec2 texture_correction = glm::vec2(.001f, .005f);
                vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(s0 + texture_correction.x, t1 - texture_correction.y));
                vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(s0 + texture_correction.x, t0 + texture_correction.y));
                vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(s1 - texture_correction.x, t0 + texture_correction.y));
                vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(s1 - texture_correction.x, t1 - texture_correction.y));
                
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

                position.x += glyph->advance_x;
                index++;
            }
        }
        m_max_bound = glm::vec2(position.x, m_font_size);
        
        m_mesh->get_vbo()->unlock(vertices_offset);
        m_mesh->get_ibo()->unlock(indices_offset);
        
        return m_mesh;
    }
    
    void ces_font_component::set_font_size(i32 size)
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
    
    texture_shared_ptr ces_font_component::get_texture() const
    {
        return m_texture;
    }
}
