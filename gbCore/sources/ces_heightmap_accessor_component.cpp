//
//  ces_heightmap_accessor_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_accessor_component.h"
#include "glm_extensions.h"

namespace gb
{
    ces_heightmap_accessor_component::ces_heightmap_accessor_component() :
    m_heightmap_mmap(nullptr)
    {
        
    }
    
    ces_heightmap_accessor_component::~ces_heightmap_accessor_component()
    {
        
    }
    
    void ces_heightmap_accessor_component::setup(const std::shared_ptr<heightmap_mmap>& heightmap_mmap)
    {
        m_heightmap_mmap = heightmap_mmap;
    }
    
    f32 ces_heightmap_accessor_component::get_height(const glm::vec3& position) const
    {
        glm::ivec2 heightmap_size = m_heightmap_mmap->get_heightmap_size();
        f32 _x = position.x / 1.f;
        f32 _z = position.z / 1.f;
        i32 x = static_cast<i32>(floor(_x));
        i32 z = static_cast<i32>(floor(_z));
        f32 dx = _x - x;
        f32 dy = _z - z;
        
        if((x < 0) || (z < 0) || (x > (heightmap_size.x - 1)) || (z > (heightmap_size.y - 1)))
        {
            return 0.f;
        }
        
        f32 height_00 = m_heightmap_mmap->get_vertex_position(x, z).y;
        f32 height_01 = m_heightmap_mmap->get_vertex_position(x, z).y;
        if(z < (heightmap_size.y - 1) && z >= 0)
        {
            height_01 = m_heightmap_mmap->get_vertex_position(x, z + 1).y;
        }
        
        f32 height_10 = m_heightmap_mmap->get_vertex_position(x, z).y;
        if(x < (heightmap_size.x - 1) && x >= 0)
        {
            height_10 = m_heightmap_mmap->get_vertex_position(x + 1, z).y;
        }
        
        f32 height_11 = m_heightmap_mmap->get_vertex_position(x, z).y;
        if(z < (heightmap_size.y - 1) && z >= 0 && x < (heightmap_size.x - 1) && x >= 0)
        {
            height_11 = m_heightmap_mmap->get_vertex_position(x + 1, z + 1).y;
        }
        
        f32 height_0 = height_00 * (1.0f - dy) + height_01 * dy;
        f32 height_1 = height_10 * (1.0f - dy) + height_11 * dy;
        return height_0 * (1.0f - dx) + height_1 * dx;
    }
    
    glm::vec3 ces_heightmap_accessor_component::get_normal(const glm::vec3& position) const
    {
        glm::ivec2 heightmap_size = m_heightmap_mmap->get_heightmap_size();
        f32 _x = position.x / 1.f;
        f32 _z = position.z / 1.f;
        i32 x = static_cast<i32>(floor(_x));
        i32 z = static_cast<i32>(floor(_z));
        
        if((x < 0) || (z < 0) || (x > (heightmap_size.x - 1)) || (z > (heightmap_size.y - 1)))
        {
            return glm::vec3(0.f, 1.f, 0.f);
        }
        
        glm::vec3 normal_00 = m_heightmap_mmap->get_uncompressed_vertex_normal(x, z);
        
        glm::vec3 normal_01 = m_heightmap_mmap->get_uncompressed_vertex_normal(x, z);
        if(z < (heightmap_size.y - 1) && z >= 0)
        {
            normal_01 = m_heightmap_mmap->get_uncompressed_vertex_normal(x, z + 1);
        }
        
        glm::vec3 normal_10 = m_heightmap_mmap->get_uncompressed_vertex_normal(x, z);
        if(x < (heightmap_size.x - 1) && x >= 0)
        {
            normal_10 = m_heightmap_mmap->get_uncompressed_vertex_normal(x + 1, z);
        }
        
        glm::vec3 normal_11 = m_heightmap_mmap->get_uncompressed_vertex_normal(x, z);
        if(z < (heightmap_size.y - 1) && z >= 0 && x < (heightmap_size.x - 1) && x >= 0)
        {
            normal_11 = m_heightmap_mmap->get_uncompressed_vertex_normal(x + 1, z + 1);
        }
        
        glm::vec3 normal = normal_00 + normal_01 + normal_10 + normal_11 / 4.f;
        return glm::normalize(glm::vec3(normal.x, normal.y, normal.z));

    }
    
    glm::vec2 ces_heightmap_accessor_component::get_angles(const glm::vec3& position) const
    {
        f32 offset = .25f;
        glm::vec3 point_01 = position;
        glm::vec3 point_02 = glm::vec3(position.x, position.y + offset, position.z);
        f32 height =  ces_heightmap_accessor_component::get_height(glm::vec3(position.x + offset, 0.f, position.z));
        glm::vec3 point_03 = glm::vec3(position.x + offset, height, position.z);
        height = ces_heightmap_accessor_component::get_height(glm::vec3(position.x, 0.f, position.z + offset));
        glm::vec3 point_04 = glm::vec3(position.x, height, position.z + offset);
        
        f32 angle_01 = glm::get_angle(point_01, point_02, point_03);
        f32 angle_02 = glm::get_angle(point_01, point_02, point_04);
        
        return glm::vec2(glm::degrees(acos(angle_02) - M_PI_2), glm::degrees(asin(angle_01)));
    }
};
