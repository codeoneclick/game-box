//
//  ces_skeleton_3d_component.h
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_skeleton_3d_component : public ces_base_component
    {
    private:
        
    protected:
        
        ui32 m_num_bones;
        std::set<bone_3d_shared_ptr> m_root_bones;
        glm::mat4* m_bones_transformations;
        
    public:
        
        STTI_CLASS_GUID(ces_skeleton_3d_component, ces_base_component::g_guids_container)
        ces_skeleton_3d_component();
        ~ces_skeleton_3d_component();
        
        void setup(const skeleton_3d_transfering_data_shared_ptr& skeleton_transfering_data,
                   const sequence_3d_transfering_data_shared_ptr& sequence_transfering_data);
        
        void add_bone(const bone_3d_shared_ptr& bone);
        bone_3d_shared_ptr get_bone(i32 index) const;
        bone_3d_shared_ptr get_bone(const std::string& name) const;
        
        i32 get_num_bones() const;
        const std::set<bone_3d_shared_ptr> get_root_bones() const;
        glm::mat4* get_bones_transformations() const;
    };
};
