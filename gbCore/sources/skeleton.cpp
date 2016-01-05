//
//  skeleton.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "skeleton.h"
#include "mesh.h"
#include "bone.h"

namespace gb
{
    skeleton::skeleton(const skeleton_transfering_data_shared_ptr& skeleton_transfering_data) :
    m_num_bones(skeleton_transfering_data->get_num_bones())
    {
        assert(skeleton_transfering_data != nullptr);
        m_bones_transformations = new glm::mat4x4[m_num_bones];
        
        std::vector<bone_data_shared_ptr> bones_raw_data = skeleton_transfering_data->get_bones_data();
        for(const auto& bone_raw_data : bones_raw_data)
        {
            i32 id = bone_raw_data->get_bone_id();
            i32 parent_id = bone_raw_data->get_bone_parent_id();
            bone_shared_ptr bone = skeleton::get_bone(id);
            if(!bone)
            {
                bone = std::make_shared<gb::bone>(id, parent_id);
            }
            skeleton::add_bone(bone);
        }
    }
    
    skeleton::~skeleton()
    {
        delete [] m_bones_transformations;
    }
    
    void skeleton::add_bone(const bone_shared_ptr &bone)
    {
        if(!bone)
        {
            return;
        }
        
        if (bone->get_parent_id() == -1)
        {
            m_root_bones.insert(bone);
            return;
        }
        
        bone_shared_ptr parent = skeleton::get_bone(bone->get_parent_id());
        if(parent)
        {
            parent->add_child(bone);
            return;
        }
        assert(false);
    }
    
    bone_shared_ptr skeleton::get_bone(ui32 index) const
    {
        for(const auto& root : m_root_bones)
        {
            if (root->get_id() == index)
            {
                return root;
            }
            else
            {
                bone_shared_ptr child = root->get_child(index);
                if(child)
                {
                    return child;
                }
            }
        }
        return nullptr;
    }
    
    ui32 skeleton::get_num_bones() const
    {
        return m_num_bones;
    }
    
    const std::set<bone_shared_ptr> skeleton::get_root_bones() const
    {
        return m_root_bones;
    }
    
    glm::mat4* skeleton::get_bones_transformations() const
    {
        return m_bones_transformations;
    }
    
    void skeleton::update()
    {
        bone_shared_ptr bone = nullptr;
        for (ui32 i = 0; i < skeleton::get_num_bones(); ++i)
        {
            bone = skeleton::get_bone(i);
            if(bone)
            {
                bone->set_transformation(m_bones_transformations + i);
            }
        }
        for(const auto& root : m_root_bones)
        {
            root->update();
        }
    }
    
    void skeleton::bind_pose_transformation(void)
    {
        for(const auto& root : m_root_bones)
        {
            root->bind_pose_transformation();
        }
    }
}