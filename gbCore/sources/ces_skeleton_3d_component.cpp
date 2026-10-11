//
//  ces_skeleton_3d_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_skeleton_3d_component.h"
#include "mesh_3d.h"
#include "animation_3d_sequence.h"
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
        m_root_bones.clear();
        m_bones_by_id.clear();
        m_num_bones = skeleton_transfering_data->get_num_bones();
        m_bones_by_id.reserve(m_num_bones);
        m_bones_storage.resize(m_num_bones);
        m_bones_transformations = m_bones_storage.data();
        
        const auto& bones_raw_data = skeleton_transfering_data->get_bones_raw_data();
        for(const auto& bone_raw_data : bones_raw_data)
        {
            i32 id = bone_raw_data->get_bone_id();
            i32 parent_id = bone_raw_data->get_bone_parent_id();
            std::string name = bone_raw_data->get_bone_name();
            bone_3d_shared_ptr bone = ces_skeleton_3d_component::get_bone(id);
            if(bone == nullptr)
            {
                bone = std::make_shared<bone_3d>(id, parent_id, name);
                // Rig creation is repeated for crowd members; avoid per-bone console flushes.
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
            const auto id = bone->get_id();
            if (id >= 0 && static_cast<ui32>(id) < m_num_bones) {
                if (m_bones_by_id.size() <= static_cast<size_t>(id)) m_bones_by_id.resize(id + 1);
                if (!m_bones_by_id[id]) m_bones_by_id[id] = bone;
            }
            return;
        }
        
        bone_3d_shared_ptr parent = ces_skeleton_3d_component::get_bone(bone->get_parent_id());
        if (parent != nullptr)
        {
            parent->add_child(bone);
            const auto id = bone->get_id();
            if (id >= 0 && static_cast<ui32>(id) < m_num_bones) {
                if (m_bones_by_id.size() <= static_cast<size_t>(id)) m_bones_by_id.resize(id + 1);
                if (!m_bones_by_id[id]) m_bones_by_id[id] = bone;
            }
            return;
        }
        assert(false);
    }
    
    bone_3d_shared_ptr ces_skeleton_3d_component::get_bone(i32 index) const
    {
        // Animation queries every bone twice per frame. Avoid a recursive
        // hierarchy search for registered IDs; keep the legacy fallback for
        // children attached directly through the public bone graph.
        if (index >= 0 && static_cast<size_t>(index) < m_bones_by_id.size() && m_bones_by_id[index])
            return m_bones_by_id[index];
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
    
    bone_3d_shared_ptr ces_skeleton_3d_component::get_bone(const std::string& name) const
    {
        for(const auto& root : m_root_bones)
        {
            if (root->get_name() == name)
            {
                return root;
            }
            else
            {
                bone_3d_shared_ptr child = root->find_child(name);
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
    
    const std::set<bone_3d_shared_ptr>& ces_skeleton_3d_component::get_root_bones() const
    {
        return m_root_bones;
    }
    
    glm::mat4* ces_skeleton_3d_component::get_bones_transformations() const
    {
        return m_bones_transformations;
    }
}
