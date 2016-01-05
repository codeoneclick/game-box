//
//  ces_geometry_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_geometry_component.h"
#include "mesh.h"
#include "std_extensions.h"

namespace gb
{
    ces_geometry_component::ces_geometry_component() :
    m_mesh(nullptr),
    m_frame(0.f, 0.f, 1.f, 1.f),
    m_pivot(0.f),
    m_texcoord(0.f, 0.f, 1.f, 1.f)
    {
        m_type = k_ces_component_type_geometry;
    }
    
    ces_geometry_component::~ces_geometry_component()
    {
        
    }
    
    void ces_geometry_component::set_mesh(const mesh_shared_ptr& mesh)
    {
        m_mesh = mesh;
    }
    
    mesh_shared_ptr ces_geometry_component::get_mesh() const
    {
        return m_mesh;
    }
    
    void ces_geometry_component::set_size(const glm::vec2& size)
    {
        m_frame.z = size.x;
        m_frame.w = size.y;
    }
    
    glm::vec2 ces_geometry_component::get_size() const
    {
        return glm::vec2(m_frame.z, m_frame.w);
    }
    
    void ces_geometry_component::set_pivot(const glm::vec2& pivot)
    {
        m_pivot = pivot;
        ces_geometry_component::update_mesh_position_attributes();
    }
    
    glm::vec2 ces_geometry_component::get_pivot() const
    {
        
    }
    
    void ces_geometry_component::update_mesh_position_attributes()
    {
        if(m_mesh)
        {
            glm::vec2 size = glm::vec2(m_frame.z - m_pivot.x, m_frame.w - m_pivot.y);
            glm::vec2 position = glm::vec2(size.x - m_frame.z, size.y - m_frame.w);
            glm::vec4 frame = glm::vec4(position.x, size.y, size.x, position.y);
            
            ieVertex* vertexData = shape->getVertexBuffer()->lock();
            vertexData[0].m_position = glm::vec2(frame.x, frame.z);
            vertexData[1].m_position = glm::vec2(frame.x, frame.w);
            vertexData[2].m_position = glm::vec2(frame.y, frame.z);
            vertexData[3].m_position = glm::vec2(frame.y, frame.w);
            shape->getVertexBuffer()->unlock();
        }

    }
    
    const glm::vec3 ces_geometry_component::get_min_bound() const
    {
        if(!m_mesh)
        {
            std::cout<<"warning! mesh doesn't exist - bounds is wrong."<<std::endl;
            return glm::vec3(0.f);
        }
        return m_mesh->get_min_bound();
    }
    
    const glm::vec3 ces_geometry_component::get_max_bound() const
    {
        if(!m_mesh)
        {
            std::cout<<"warning! mesh doesn't exist - bounds is wrong."<<std::endl;
            return glm::vec3(0.f);
        }
        return m_mesh->get_max_bound();
    }
    
    const std::tuple<glm::vec3, glm::vec3> ces_geometry_component::get_bounds() const
    {
        if(!m_mesh)
        {
            std::cout<<"warning! mesh doesn't exist - bounds is wrong."<<std::endl;
            return std::make_tuple(glm::vec3(0.f), glm::vec3(0.f));
        }
        return m_mesh->get_bounds();
    }
    
    const glm::vec3 ces_geometry_component::get_min_bound(const glm::mat4& mat) const
    {
        if(!m_mesh)
        {
            std::cout<<"warning! mesh doesn't exist - bounds is wrong."<<std::endl;
            return glm::vec3(0.f);
        }
        return m_mesh->get_min_bound(mat);
    }
    
    const glm::vec3 ces_geometry_component::get_max_bound(const glm::mat4& mat) const
    {
        if(!m_mesh)
        {
            std::cout<<"warning! mesh doesn't exist - bounds is wrong."<<std::endl;
            return glm::vec3(0.f);
        }
        return m_mesh->get_max_bound(mat);
    }
    
    const std::tuple<glm::vec3, glm::vec3> ces_geometry_component::get_bounds(const glm::mat4& mat) const
    {
        if(!m_mesh)
        {
            std::cout<<"warning! mesh doesn't exist - bounds is wrong."<<std::endl;
            return std::make_tuple(glm::vec3(0.f), glm::vec3(0.f));
        }
        return m_mesh->get_bounds(mat);
    }
    
    std::string ces_geometry_component::get_guid() const
    {
        return m_guid;
    }
}