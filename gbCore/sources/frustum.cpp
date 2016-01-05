//
//  frustum.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "frustum.h"
#include "glm_extensions.h"

namespace gb
{
    frustum::frustum()
    {
        
    }
    
    frustum::~frustum()
    {
        
    }
    
    glm::vec4 frustum::create_plane(const glm::vec3 &point_01, const glm::vec3 &point_02, const glm::vec3 &point_03)
    {
        glm::vec3 edge_01, edge_02;
        edge_01 = point_01 - point_02;
        edge_02 = point_03 - point_02;
        glm::vec3 normal = glm::normalize(glm::cross(edge_01, edge_02));
        f32 offset = -glm::dot(normal, point_02);
        return glm::vec4(normal, offset);
    }
    
    inline f32 frustum::get_distance_to_plane(const glm::vec4& plane, const glm::vec3& point)
    {
        static glm::vec3 abc = glm::vec3(0.f);
        abc = frustum::get_plane_abc(plane);
        static f32 d = 0.f;
        d = frustum::get_plane_d(plane);
        return (d + glm::dot(abc, point)) * -1.f;
    }
    
    inline glm::vec3 frustum::get_plane_abc(const glm::vec4& plane)
    {
        return glm::vec3(plane.x, plane.y, plane.z);
    }
    
    inline f32 frustum::get_plane_d(const glm::vec4& plane)
    {
        return plane.w;
    }
    
    void frustum::update(f32 fov, f32 aspect, f32 near, f32 far, const glm::vec3 &position, const glm::vec3& up, const glm::vec3 &look_at)
    {
        f32 tan = tanf(glm::radians(fov * .5f));
        f32 near_height = near * tan;
        f32 near_width = near_height * aspect;
        f32 far_height = far  * tan;
        f32 far_width = far_height * aspect;
        
        glm::vec3 basis_z = glm::normalize(position - look_at);
        glm::vec3 basis_x = glm::normalize(glm::cross(up, basis_z));
        glm::vec3 basis_y = glm::cross(basis_z, basis_x);
        
        glm::vec3 near_offset = position - basis_z * near;
        glm::vec3 far_offset = position - basis_z * far;
        
        glm::vec3 near_top_left_point = near_offset + basis_y * near_height - basis_x * near_width;
        glm::vec3 near_top_right_point = near_offset + basis_y * near_height + basis_x * near_width;
        glm::vec3 near_bottom_left_point = near_offset - basis_y * near_height - basis_x * near_width;
        glm::vec3 near_bottom_right_point = near_offset - basis_y * near_height + basis_x * near_width;
        
        glm::vec3 far_top_left_point = far_offset + basis_y * far_height - basis_x * far_width;
        glm::vec3 far_top_right_point = far_offset + basis_y * far_height + basis_x * far_width;
        glm::vec3 far_bottom_left_point = far_offset - basis_y * far_height - basis_x * far_width;
        glm::vec3 far_bottom_right_point = far_offset - basis_y * far_height + basis_x * far_width;
        
        m_planes[e_frustum_plane_top] = frustum::create_plane(near_top_right_point, near_top_left_point, far_top_left_point);
        m_planes[e_frustum_plane_bottom] = frustum::create_plane(near_bottom_left_point, near_bottom_right_point, far_bottom_right_point);
        m_planes[e_frustum_plane_left] = frustum::create_plane(near_top_left_point, near_bottom_left_point, far_bottom_left_point);
        m_planes[e_frustum_plane_right] = frustum::create_plane(near_bottom_right_point, near_top_right_point, far_bottom_right_point);
        m_planes[e_frustum_plane_near] = frustum::create_plane(near_top_left_point, near_top_right_point, near_bottom_right_point);
        m_planes[e_frustum_plane_far] = frustum::create_plane(far_top_right_point, far_top_left_point, far_bottom_left_point);
    }
    
    e_frustum_bound_result frustum::is_point_in_frustum(const glm::vec3& point,
                                                        const glm::mat4& matrix_m)
    {
        for(ui32 i = 0; i < e_frustum_plane_max; ++i)
        {
            if(frustum::get_distance_to_plane(m_planes[i], point) < 0.f)
            {
                return e_frustum_bound_result_outside;
            }
        }
        return e_frustum_bound_result_inside;
    }
    
    e_frustum_bound_result frustum::is_sphere_in_frumstum(const glm::vec3& center, f32 radius,
                                                          const glm::mat4& matrix_m)
    {
        e_frustum_bound_result result = e_frustum_bound_result_inside;
        for(ui32 i = 0; i < e_frustum_plane_max; ++i)
        {
            f32 distance = frustum::get_distance_to_plane(m_planes[i], center);
            if (distance < -radius)
            {
                return e_frustum_bound_result_outside;
            }
            else if (distance < radius)
            {
                result =  e_frustum_bound_result_intersect;
            }
        }
        return result;
    }
    
#define kMaxPointsInBoundingBox 8
    
    e_frustum_bound_result frustum::is_bounding_box_in_frustum(const glm::vec3& min_bound,
                                                               const glm::vec3& max_bound,
                                                               const glm::mat4& matrix_m)
    {
        glm::vec3 points[kMaxPointsInBoundingBox];
        glm::vec3 min_bound_m = glm::transform(min_bound, matrix_m);
        glm::vec3 max_bound_m = glm::transform(max_bound, matrix_m);
        
        points[0] = min_bound;
        points[1] = glm::vec3(min_bound_m.x, min_bound_m.y, max_bound_m.z);
        points[2] = glm::vec3(max_bound_m.x, min_bound_m.y, min_bound_m.z);
        points[3] = glm::vec3(max_bound_m.x, min_bound_m.y, max_bound_m.z);
        
        points[4] = max_bound;
        points[5] = glm::vec3(max_bound_m.x, max_bound_m.y, min_bound_m.z);
        points[6] = glm::vec3(min_bound_m.x, max_bound_m.y, max_bound_m.z);
        points[7] = glm::vec3(min_bound_m.x, max_bound_m.y, min_bound_m.z);
        
        e_frustum_bound_result result = e_frustum_bound_result_inside;
        ui32 points_in, points_out;
        
        for(ui32 i = 0; i < e_frustum_plane_max; ++i)
        {
            points_in = 0;
            points_out = 0;
            for (ui32 j = 0; j < kMaxPointsInBoundingBox && (points_in == 0 || points_out == 0); ++j)
            {
                frustum::get_distance_to_plane(m_planes[i], points[j]) < 0.f ? ++points_out : ++points_in;
            }
            if (!points_in)
            {
                result = e_frustum_bound_result_outside;
                break;
            }
            else if (points_out)
            {
                result = e_frustum_bound_result_intersect;
            }
        }
        return result;
    }
}