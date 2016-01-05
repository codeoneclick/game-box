//
//  frustum.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef frustum_h
#define frustum_h

#include "main_headers.h"

namespace gb
{
    enum e_frustum_bound_result
    {
        e_frustum_bound_result_outside = 0,
        e_frustum_bound_result_intersect,
        e_frustum_bound_result_inside
    };
    
    class frustum
    {
    public:
        
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
        
    protected:
        
        glm::vec4 create_plane(const glm::vec3& point_01, const glm::vec3& point_02, const glm::vec3& point_03);
        
        static f32 get_distance_to_plane(const glm::vec4& plane, const glm::vec3& point);
        static glm::vec3 get_plane_abc(const glm::vec4& plane);
        static f32 get_plane_d(const glm::vec4& plane);
        
    public:
        
        frustum();
        ~frustum();
        
        void update(f32 fov, f32 aspect, f32 near, f32 far,
                    const glm::vec3& position, const glm::vec3& up, const glm::vec3& look_at);
        
        e_frustum_bound_result is_point_in_frustum(const glm::vec3& point, const glm::mat4& matrix_m = glm::mat4(1.f));
        e_frustum_bound_result is_sphere_in_frumstum(const glm::vec3& center, f32 radius, const glm::mat4& matrix_m = glm::mat4(1.f));
        e_frustum_bound_result is_bounding_box_in_frustum(const glm::vec3& min_bound, const glm::vec3& max_bound, const glm::mat4& matrix_m = glm::mat4(1.f));
    };
    
};

#endif
