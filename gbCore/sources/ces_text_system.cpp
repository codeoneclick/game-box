//
//  ces_text_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_text_system.h"
#include "ces_text_component.h"
#include "ces_geometry_freeform_component.h"
#include "vbo.h"
#include "ibo.h"
#include "mesh.h"

namespace gb
{
    static const f32 k_text_uv_box_width = .125f;
    static const f32 k_text_width = 32.f;
    static const f32 k_text_spacesize = 20.f;
    
    static i32 l_size[64] = {
        36, 29, 30, 34, 25, 25, 34, 33,
        11, 20, 31, 24, 48, 35, 39, 29,
        42, 31, 27, 31, 34, 35, 46, 35,
        31, 27, 30, 26, 28, 26, 31, 28,
        28, 28, 29, 29, 14, 24, 30, 18,
        26, 14, 14, 14, 25, 28, 31, 0,
        0, 38, 39, 12, 36, 34, 0, 0,
        0, 38, 0, 0, 0, 0, 0, 0 };
    
    ces_text_system::ces_text_system()
    {
        m_type = e_ces_system_type_text;
    }
    
    ces_text_system::~ces_text_system()
    {
        
    }
    
    void ces_text_system::on_feed_start(f32 deltatime)
    {
        
    }
    
    void ces_text_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_text_system::update_recursively(entity);
    }
    
    void ces_text_system::on_feed_end(f32 deltatime)
    {
        
    }
    
    void ces_text_system::update_recursively(const ces_entity_shared_ptr& entity)
    {
        ces_text_component* text_component = unsafe_get_text_component(entity);
        if(text_component && text_component->is_text_changed())
        {
            ces_geometry_freeform_component* geometry_freeform_component = unsafe_get_geometry_freeform_component(entity);
            geometry_freeform_component->set_mesh(ces_text_system::convert_text_to_geometry(text_component->get_text()));
            text_component->reset();
        }
        
        std::set<ces_entity_shared_ptr> children = entity->get_children();
        for(const auto& child : children)
        {
            ces_text_system::update_recursively(child);
        }
    }
    
    i32 ces_text_system::convert_symbol_to_index(i32 c_val)
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
    
    mesh_shared_ptr ces_text_system::convert_text_to_geometry(const std::string& text)
    {
        glm::vec2 position = glm::vec2(0.f);
        
        std::vector<vbo::vertex_attribute> raw_vertices;
        std::vector<i16> raw_indices;
        
        i32 vertices_offset = 0;
        i32 indices_offset = 0;
        
        for(const char& symbol : text)
        {
            i32 c_val = static_cast<i32>(symbol);
            
            i32 index = ces_text_system::convert_symbol_to_index(c_val);
            
            if(index == -1)
            {
                position.x += k_text_spacesize;
                continue;
            }
            
            raw_vertices.resize(raw_vertices.size() + 4);
            raw_indices.resize(raw_indices.size() + 6);
            
            i32 row = index / 8;
            i32 column = index % 8;
            
            f32 v = row * k_text_uv_box_width;
            f32 v2 = v + k_text_uv_box_width;
            f32 u = column * k_text_uv_box_width;
            f32 u2 = u + k_text_uv_box_width;

            raw_vertices[vertices_offset++].m_position = glm::vec2(position.x, position.y + k_text_width);
            raw_vertices[vertices_offset++].m_position = glm::vec2(position.x, position.y);
            raw_vertices[vertices_offset++].m_position = glm::vec2(position.x + k_text_width, position.y);
            raw_vertices[vertices_offset++].m_position = glm::vec2(position.x + k_text_width, position.y + k_text_width);
            
            vertices_offset -= 4;
            
            raw_vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u + .001f, v + .001f));
            raw_vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u + .001f, v2 - .001f));
            raw_vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u2 - .001f, v2 - .001f));
            raw_vertices[vertices_offset++].m_texcoord = glm::packUnorm2x16(glm::vec2(u2 - .001f, v + .001f));
            
            raw_indices[indices_offset++] = 0 + raw_indices.size() - 6;
            raw_indices[indices_offset++] = 1 + raw_indices.size() - 6;
            raw_indices[indices_offset++] = 2 + raw_indices.size() - 6;
            raw_indices[indices_offset++] = 0 + raw_indices.size() - 6;
            raw_indices[indices_offset++] = 2 + raw_indices.size() - 6;
            raw_indices[indices_offset++] = 3 + raw_indices.size() - 6;
            
            position.x += l_size[index] / 2;
        }
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(raw_vertices.size(), GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        std::memcpy(vertices, &raw_vertices[0], raw_vertices.size() * sizeof(vbo::vertex_attribute));
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(raw_indices.size(), GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        std::memcpy(indices, &raw_indices[0], raw_indices.size() * sizeof(i16));
        ibo->unlock();
        
        mesh_shared_ptr mesh = std::make_shared<gb::mesh>(vbo, ibo);
        return mesh;
    }
}