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
        
    public:
        
        CTTI_CLASS_GUID(ces_skeleton_3d_component, ces_base_component::g_guids_container)
        ces_skeleton_3d_component();
        ~ces_skeleton_3d_component();
        
        void add_bone(const bone_3d_shared_ptr& bone);
        bone_3d_shared_ptr get_bone(ui32 index) const;
        
        ui32 get_num_bones() const;
        const std::set<bone_3d_shared_ptr> get_root_bones() const;
        
        glm::mat4* get_bones_transformations() const;
    };
};
