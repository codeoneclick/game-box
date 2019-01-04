//
//  ces_skeleton_3d_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_skeleton_3d_component.h"
#include "mesh_3d.h"
#include "animation_sequence_3d.h"
#include "bone_3d.h"

namespace gb
{
    ces_skeleton_3d_component::ces_skeleton_3d_component() :
    m_num_bones(0),
    m_bones_transformations(nullptr)
    {
        
    }
    
    ces_skeleton_3d_component::~ces_skeleton_3d_component()
    {
        
    }
    
    void ces_skeleton_3d_component::setup(const skeleton_3d_transfering_data_shared_ptr &skeleton_transfering_data,
                                          const sequence_3d_transfering_data_shared_ptr &sequence_transfering_data)
    {
        assert(skeleton_transfering_data != nullptr);
        m_num_bones = skeleton_transfering_data->get_num_bones();
        m_bones_transformations = new glm::mat4x4[m_num_bones];
        
        std::vector<bone_3d_data_shared_ptr> bones_raw_data = skeleton_transfering_data->get_bones_raw_data();
        for(const auto& bone_raw_data : bones_raw_data)
        {
            i32 id = bone_raw_data->get_bone_id();
            i32 parent_id = bone_raw_data->get_bone_parent_id();
            std::string name = bone_raw_data->get_bone_name();
            bone_3d_shared_ptr bone = ces_skeleton_3d_component::get_bone(id);
            if(bone == nullptr)
            {
                bone = std::make_shared<bone_3d>(id, parent_id, name);
                std::cout<<"bone with name: "<<name<<" created"<<std::endl;
            }
            ces_skeleton_3d_component::add_bone(bone);
        }
    }
    
    void ces_skeleton_3d_component::add_bone(const bone_3d_shared_ptr& bone)
    {
        if(bone == nullptr)
        {
            return;
        }
        
        if (bone->get_parent_id() == -1)
        {
            m_root_bones.insert(bone);
            return;
        }
        
        bone_3d_shared_ptr parent = ces_skeleton_3d_component::get_bone(bone->get_parent_id());
        if (parent != nullptr)
        {
            parent->add_child(bone);
            return;
        }
        assert(false);
    }
    
    bone_3d_shared_ptr ces_skeleton_3d_component::get_bone(i32 index) const
    {
        for(const auto& root : m_root_bones)
        {
            if (root->get_id() == index)
            {
                return root;
            }
            else
            {
                bone_3d_shared_ptr child = root->find_child(index);
                if(child != nullptr)
                {
                    return child;
                }
            }
        }
        return nullptr;
    }
    
    i32 ces_skeleton_3d_component::get_num_bones() const
    {
        return m_num_bones;
    }
    
    const std::set<bone_3d_shared_ptr> ces_skeleton_3d_component::get_root_bones() const
    {
        return m_root_bones;
    }
    
    glm::mat4* ces_skeleton_3d_component::get_bones_transformations() const
    {
        return m_bones_transformations;
    }
}
