//
//  camera_3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/17/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "std_extensions.h"

namespace gb
{
    class camera_3d
    {
    private:
        
    protected:
        
        bool m_is_matrix_m_computed;
        
        glm::mat4 m_mat_p;
        glm::mat4 m_mat_v;
        
        glm::vec3 m_position;
        glm::vec3 m_look_at;
        glm::vec3 m_up;
        f32 m_rotation;
        glm::vec3 m_distance;
        
        f32 m_fov;
        f32 m_aspect;
        f32 m_near;
        f32 m_far;
        
        glm::ivec4 m_viewport;
        
    public:
        
        camera_3d(f32 fov, f32 near, f32 far, glm::ivec4 viewport, bool is_perspective);
        ~camera_3d();
        
        void set_position(const glm::vec3& position);
        glm::vec3 get_position() const;
        
        void set_rotation(f32 rotation);
        f32 get_rotation() const;
        
        void set_look_at(const glm::vec3& look_at);
        glm::vec3 get_look_at() const;
        
        void set_distance_to_look_at(const glm::vec3& distance);
        glm::vec3 get_distance_to_look_at() const;

        glm::mat4 get_mat_p() const;
        glm::mat4 get_mat_v();
    };
};
