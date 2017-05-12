//
//  bone_3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    
    class bone_3d_data
    {
    private:
        
    protected:
        
        i32 m_id;
        i32 m_parent_id;
        
    public:
        
        bone_3d_data(i32 id, i32 parent_id);
        ~bone_3d_data();
        
        i32 get_bone_id() const;
        i32 get_bone_parent_id() const;
    };
    
    class bone_3d final : public std::enable_shared_from_this<bone_3d>
    {
    private:
        
    protected:
        
        i32	m_id;
        i32	m_parent_id;
        
        bone_3d_weak_ptr m_parent;
        std::vector<bone_3d_shared_ptr> m_children;
        
        glm::mat4  m_base_transformation;
        glm::mat4* m_transformation;
        glm::mat4  m_bind_transformation;
        
    public:
        
        bone_3d(i32 id, i32 parent_id);
        ~bone_3d();
        
        void add_child(const bone_3d_shared_ptr& bone);
        bone_3d_shared_ptr find_child(i32 id);
        
        const std::vector<bone_3d_shared_ptr>& get_children() const;
        bone_3d_shared_ptr get_parent() const;
        
        i32 get_id() const;
        i32 get_parent_id() const;
        
        void set_transformation(glm::mat4* transformation);
        glm::mat4* get_transformation() const;
        glm::mat4 get_base_transformation() const;
        
        void update();
        void bind_pose_transformation();
    };
};
