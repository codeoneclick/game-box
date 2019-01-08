//
//  ces_animation_3d_mask_component.cpp
//  gbCore
//
//  Created by serhii.s on 1/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_animation_3d_mask_component.h"

namespace gb
{
    bone_3d_mask::bone_3d_mask(const std::string& name, f32 weight) :
    m_name(name),
    m_weight(weight)
    {
        
    }
    
    std::string bone_3d_mask::get_name() const
    {
        return m_name;
    }
    
    f32 bone_3d_mask::get_weight() const
    {
        return m_weight;
    }
    
    void animation_3d_mask::add_bone_mask(const std::string& name, f32 weight)
    {
        m_bones_mask.push_back(std::make_shared<bone_3d_mask>(name, weight));
    }
    
    const std::vector<bone_3d_mask_shared_ptr>& animation_3d_mask::get_bones_mask() const
    {
        return m_bones_mask;
    }
    
    bool animation_3d_mask::is_bone_has_weight(const std::string& bone_name) const
    {
        bool result = false;
        for (auto bone : m_bones_mask)
        {
            if (bone->get_name() == bone_name && bone->get_weight() > 0.f)
            {
                result = true;
                break;
            }
        }
        return result;
    }
    
    ces_animation_3d_mask_component::ces_animation_3d_mask_component()
    {
        
    }
    
    ces_animation_3d_mask_component::~ces_animation_3d_mask_component()
    {
        
    }
    
    void ces_animation_3d_mask_component::add_bone_mask(const std::string& animation_name, const std::string& bone_name, f32 bone_weight)
    {
        animation_3d_mask_shared_ptr animation_mask = nullptr;
        const auto animation_mask_it = m_animation_masks.find(animation_name);
        if (animation_mask_it != m_animation_masks.end())
        {
            animation_mask = animation_mask_it->second;
        }
        else
        {
            animation_mask = std::make_shared<animation_3d_mask>();
            m_animation_masks.insert(std::make_pair(animation_name, animation_mask));
        }
        animation_mask->add_bone_mask(bone_name, bone_weight);
    }
    
    animation_3d_mask_shared_ptr ces_animation_3d_mask_component::get_animation_mask(const std::string& animation_name) const
    {
        animation_3d_mask_shared_ptr animation_mask = nullptr;
        const auto animation_mask_it = m_animation_masks.find(animation_name);
        if (animation_mask_it != m_animation_masks.end())
        {
            animation_mask = animation_mask_it->second;
        }
        return animation_mask;
    }
    
    bool ces_animation_3d_mask_component::is_bone_has_weight(const std::string& animation_name, const std::string& bone_name) const
    {
        bool result = false;
        animation_3d_mask_shared_ptr animation_mask = get_animation_mask(animation_name);
        if (animation_mask)
        {
            result = animation_mask->is_bone_has_weight(bone_name);
        }
        else
        {
            assert(false);
        }
        return result;
    }
}
