//
//  frustum_3d.hpp
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class frustum_3d
    {
    public:
        
        enum e_frustum_bounds_result
        {
            e_frustum_bounds_result_outside = 0,
            e_frustum_bounds_result_intersect,
            e_frustum_bounds_result_inside
        };
        
        enum e_frustum_plane
        {
            e_frustum_plane_top = 0,
            e_frustum_plane_bottom,
            e_frustum_plane_left,
            e_frustum_plane_right,
            e_frustum_plane_near,
            e_frustum_plane_far,
            e_frustum_plane_max
        };
        
    private:
        
        std::array<glm::vec4, e_frustum_plane_max> m_planes;
        camera_3d_weak_ptr m_camera;
        
    protected:
        
        glm::vec4 create_plane(const glm::vec3& point_01, const glm::vec3& point_02, const glm::vec3& point_03);
        
        static f32 get_distance_to_plane(const glm::vec4& plane, const glm::vec3& point);
        static glm::vec3 get_plane_abc(const glm::vec4& plane);
        static f32 get_plane_d(const glm::vec4& plane);
        
    public:
        
        frustum_3d(const camera_3d_shared_ptr& camera);
        ~frustum_3d();
        
        void update(f32 dt);
        
        e_frustum_bounds_result is_point_in_frustum(const glm::vec3& point);
        e_frustum_bounds_result is_sphere_in_frustum(const glm::vec3& center, f32 radius);
        e_frustum_bounds_result is_bound_box_in_frustum(const glm::vec3& max_bound, const glm::vec3& min_bound, const glm::mat4& matrix_m = glm::mat4(1.f));
    };
};