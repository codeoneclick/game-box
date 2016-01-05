//
//  bone.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef bone_h
#define bone_h

#include "main_headers.h"

namespace gb
{
    class bone_data
    {
    private:
        
    protected:
        
        i32 m_id;
        i32 m_parent_id;
        
    public:
        
        bone_data(i32 id, i32 parent_id);
        ~bone_data(void) = default;
        
        i32 get_bone_id(void) const;
        i32 get_bone_parent_id(void) const;
    };
    
    class bone : public std::enable_shared_from_this<bone>
    {
    private:
        
    protected:
        
        i32	m_id;
        i32	m_parent_id;
        
        std::shared_ptr<bone> m_parent;
        std::vector<std::shared_ptr<bone>> m_children;
        
        glm::mat4  m_base_transformation;
        glm::mat4* m_transformation;
        glm::mat4  m_bind_transformation;
        
    public:
        
        bone(i32 id, i32 parent_id);
        ~bone(void);
        
        void add_child(const std::shared_ptr<bone>& bone);
        std::shared_ptr<bone> get_child(i32 id);
        
        const std::vector<std::shared_ptr<bone>>& get_children(void) const;
        std::shared_ptr<bone> get_parent(void) const;
        
        i32 get_id(void) const;
        i32 get_parent_id(void) const;
        
        void set_transformation(glm::mat4* transformation);
        glm::mat4* get_transformation(void) const;
        glm::mat4 get_base_transformation(void) const;
        
        void update(void);
        void bind_pose_transformation(void);
    };
};

#endif
