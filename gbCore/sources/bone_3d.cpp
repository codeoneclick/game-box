//
//  bone_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "bone_3d.h"

namespace gb
{
    bone_3d_data::bone_3d_data(i32 id, i32 parent_id, const std::string& name) :
    m_id(id),
    m_parent_id(parent_id),
    m_name(name)
    {
        
    }
    
    bone_3d_data::~bone_3d_data()
    {
        
    }
    
    i32 bone_3d_data::get_bone_id() const
    {
        return m_id;
    }
    
    i32 bone_3d_data::get_bone_parent_id() const
    {
        return m_parent_id;
    }
    
    std::string bone_3d_data::get_bone_name() const
    {
        return m_name;
    }
    
    bone_3d::bone_3d(i32 id, i32 parent_id, const std::string& name) :
    m_id(id),
    m_parent_id(parent_id),
    m_name(name),
    m_transformation(nullptr),
    m_base_transformation(glm::mat4(1.f)),
    m_bind_transformation(glm::mat4(1.f))
    {
        
    }
    
    bone_3d::~bone_3d()
    {
        
    }
    
    void bone_3d::add_child(const bone_3d_shared_ptr& bone)
    {
        assert(bone != nullptr);
        m_children.push_back(bone);
        bone->m_parent = shared_from_this();
        bone->m_parent_id = m_id;
    }
    
    bone_3d_shared_ptr bone_3d::find_child(i32 id) const
    {
        if(m_children.size() == 0)
        {
            return nullptr;
        }
        bone_3d_shared_ptr bone = nullptr;
        for(const auto& iterator : m_children)
        {
            if(iterator->m_id == id)
            {
                bone = iterator;
                break;
            }
            bone = iterator->find_child(id);
            if(bone != nullptr)
            {
                break;
            }
        }
        return bone;
    }
    
    bone_3d_shared_ptr bone_3d::find_child(const std::string& name) const
    {
        if(m_children.size() == 0)
        {
            return nullptr;
        }
        bone_3d_shared_ptr bone = nullptr;
        for(const auto& iterator : m_children)
        {
            if(iterator->m_name == name)
            {
                bone = iterator;
                break;
            }
            bone = iterator->find_child(name);
            if(bone != nullptr)
            {
                break;
            }
        }
        return bone;
    }
    
    const std::vector<bone_3d_shared_ptr>& bone_3d::get_children() const
    {
        return m_children;
    }
    
    bone_3d_shared_ptr bone_3d::get_parent() const
    {
        bone_3d_shared_ptr parent = nullptr;
        if(!m_parent.expired())
        {
            parent = m_parent.lock();
        }
        return parent;
    }
    
    i32 bone_3d::get_id() const
    {
        return m_id;
    }
    
    i32 bone_3d::get_parent_id() const
    {
        return m_parent_id;
    }
    
    std::string bone_3d::get_name() const
    {
        return m_name;
    }
    
    void bone_3d::set_transformation(glm::mat4* transformation)
    {
        m_transformation = transformation;
    }
    
    glm::mat4* bone_3d::get_transformation() const
    {
        return m_transformation;
    }
    
    glm::mat4 bone_3d::get_base_transformation() const
    {
        return m_base_transformation;
    }
    
    void bone_3d::update()
    {
        for(const auto& iterator : m_children)
        {
            iterator->update();
        }
        m_base_transformation = (*m_transformation) * glm::inverse(m_bind_transformation);
        (*m_transformation) = (*m_transformation) * m_bind_transformation;
    }
    
    void bone_3d::bind_pose_transformation()
    {
        if (m_transformation != nullptr)
        {
            m_bind_transformation = glm::inverse(*m_transformation);
        }
        for(const auto& iterator : m_children)
        {
            iterator->bind_pose_transformation();
        }
    }
}
