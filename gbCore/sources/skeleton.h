//
//  skeleton.h
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef skeleton_h
#define skeleton_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class skeleton
    {
    private:
        
    protected:
        
        ui32 m_num_bones;
        std::set<bone_shared_ptr> m_root_bones;
        glm::mat4* m_bones_transformations;
        
    public:
        
        skeleton(const skeleton_transfering_data_shared_ptr& skeleton_transfering_data);
        ~skeleton();
        
        void add_bone(const bone_shared_ptr& bone);
        bone_shared_ptr get_bone(ui32 index) const;
        
        ui32 get_num_bones() const;
        const std::set<bone_shared_ptr> get_root_bones() const;
        
        glm::mat4* get_bones_transformations() const;
        
        void update();
        void bind_pose_transformation();
    };
};

#endif
