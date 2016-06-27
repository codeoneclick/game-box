//
//  ces_geometry_quad_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_quad_component.h"
#include "mesh.h"
#include "std_extensions.h"
#include "mesh_constructor.h"
#include "game_commands_container.h"

namespace gb
{
    ces_geometry_quad_component::ces_geometry_quad_component() :
    m_frame(0.f, 0.f, 1.f, 1.f),
    m_pivot(0.f),
    m_texcoord(0.f, 0.f, 1.f, 1.f)
    {
        m_mesh = mesh_constructor::create_shape_quad();
        ces_geometry_quad_component::update_mesh_position_attributes();
        ces_geometry_quad_component::update_mesh_texcoord_attributes();
    }
    
    ces_geometry_quad_component::~ces_geometry_quad_component()
    {
        
    }
    
    void ces_geometry_quad_component::set_size(const glm::vec2& size)
    {
        m_frame.w = size.x;
        m_frame.z = size.y;
        ces_geometry_quad_component::update_mesh_position_attributes();
    }
    
    glm::vec2 ces_geometry_quad_component::get_size() const
    {
        return glm::vec2(m_frame.z, m_frame.w);
    }
    
    void ces_geometry_quad_component::set_pivot(const glm::vec2& pivot)
    {
        m_pivot = pivot;
        ces_geometry_quad_component::update_mesh_position_attributes();
    }
    
    glm::vec2 ces_geometry_quad_component::get_pivot() const
    {
        return m_pivot;
    }
    
    void ces_geometry_quad_component::update_mesh_position_attributes()
    {
        if(m_mesh)
        {
            glm::vec2 size = glm::vec2(m_frame.z - m_pivot.x, m_frame.w - m_pivot.y);
            glm::vec2 position = glm::vec2(size.x - m_frame.z, size.y - m_frame.w);
            glm::vec4 frame = glm::vec4(position.x, size.y, size.x, position.y);
            
            vbo::vertex_attribute* vertices = m_mesh->get_vbo()->lock();
            vertices[0].m_position = glm::vec2(frame.x, frame.z);
            vertices[1].m_position = glm::vec2(frame.x, frame.w);
            vertices[2].m_position = glm::vec2(frame.y, frame.z);
            vertices[3].m_position = glm::vec2(frame.y, frame.w);
            m_mesh->get_vbo()->unlock();
        }
    }
    
    void ces_geometry_quad_component::update_mesh_texcoord_attributes()
    {
        if(m_mesh)
        {
            vbo::vertex_attribute* vertices = m_mesh->get_vbo()->lock();
            vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(m_texcoord.x, m_texcoord.y));
            vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(m_texcoord.x, m_texcoord.w));
            vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(m_texcoord.z, m_texcoord.y));
            vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(m_texcoord.z, m_texcoord.w));
            m_mesh->get_vbo()->unlock();
        }
    }
}