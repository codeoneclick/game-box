//
//  ces_animation_3d_mask_component.hpp
//  gbCore
//
//  Created by serhii.s on 1/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class bone_3d_mask
    {
    private:
        
        std::string m_name;
        f32 m_weight;
        
    protected:
        
    public:
        
        bone_3d_mask(const std::string& name, f32 weight);
        
        std::string get_name() const;
        f32 get_weight() const;
    };
    
    class animation_3d_mask
    {
    private:
        
    protected:
        
        std::vector<bone_3d_mask_shared_ptr> m_bones_mask;
        
    public:
        
        animation_3d_mask() = default;
        ~animation_3d_mask() = default;
        
        void add_bone_mask(const std::string& name, f32 weight);
        const std::vector<bone_3d_mask_shared_ptr>& get_bones_mask() const;
        bool is_bone_has_weight(const std::string& bone_name) const;
    };
    
    class ces_animation_3d_mask_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::unordered_map<std::string, animation_3d_mask_shared_ptr> m_animation_masks;
        
    public:
        
        CTTI_CLASS_GUID(ces_animation_3d_mask_component, ces_base_component::g_guids_container)
        ces_animation_3d_mask_component();
        ~ces_animation_3d_mask_component();
        
        void add_bone_mask(const std::string& animation_name, const std::string& bone_name, f32 bone_weight);
        animation_3d_mask_shared_ptr get_animation_mask(const std::string& animation_name) const;
        bool is_bone_has_weight(const std::string& animation_name, const std::string& bone_name) const;
    };
};
