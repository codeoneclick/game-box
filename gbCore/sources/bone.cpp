//
//  bone.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "bone.h"

namespace gb
{
    bone_data::bone_data(i32 id, i32 parent_id) :
    m_id(id),
    m_parent_id(parent_id)
    {
        
    }
    
    i32 bone_data::get_bone_id(void) const
    {
        return m_id;
    }
    
    i32 bone_data::get_bone_parent_id(void) const
    {
        return m_parent_id;
    }
    
    bone::bone(i32 id, i32 parent_id) :
    m_id(id),
    m_parent_id(parent_id),
    m_parent(nullptr),
    m_transformation(nullptr),
    m_base_transformation(glm::mat4(1.0)),
    m_bind_transformation(glm::mat4(1.0))
    {
        
    }
    
    bone::~bone(void)
    {
        
    }
    
    void bone::add_child(const std::shared_ptr<bone> &bone)
    {
        assert(bone != nullptr);
        m_children.push_back(bone);
        bone->m_parent = shared_from_this();
        bone->m_parent_id = m_id;
    }
    
    std::shared_ptr<bone> bone::get_child(i32 id)
    {
        if(m_children.size() == 0)
        {
            return nullptr;
        }
        std::shared_ptr<bone> bone = nullptr;
        for(const auto& iterator : m_children)
        {
            if(iterator->m_id == id)
            {
                bone = iterator;
                break;
            }
            bone = iterator->get_child(id);
            if(bone != nullptr)
            {
                break;
            }
        }
        return bone;
    }
    
    const std::vector<std::shared_ptr<bone>>& bone::get_children(void) const
    {
        return m_children;
    }
    
    std::shared_ptr<bone> bone::get_parent(void) const
    {
        return m_parent;
    }
    
    i32 bone::get_id(void) const
    {
        return m_id;
    }
    
    i32 bone::get_parent_id(void) const
    {
        return m_parent_id;
    }
    
    void bone::set_transformation(glm::mat4* transformation)
    {
        m_transformation = transformation;
    }
    
    glm::mat4* bone::get_transformation(void) const
    {
        return m_transformation;
    }
    
    glm::mat4 bone::get_base_transformation(void) const
    {
        return m_base_transformation;
    }
    
    void bone::update(void)
    {
        for(const auto& iterator : m_children)
        {
            iterator->update();
        }
        m_base_transformation = (*m_transformation) * glm::inverse(m_bind_transformation);
        (*m_transformation) = (*m_transformation) * m_bind_transformation;
    }
    
    void bone::bind_pose_transformation(void)
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