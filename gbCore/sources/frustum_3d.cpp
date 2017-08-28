//
//  frustum_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "frustum_3d.h"
#include "camera_3d.h"
#include "glm_extensions.h"

namespace gb
{
    frustum_3d::frustum_3d(const camera_3d_shared_ptr& camera) :
    m_camera(camera)
    {
        
    }
    
    frustum_3d::~frustum_3d()
    {
        
    }
    
    glm::vec4 frustum_3d::create_plane(const glm::vec3 &point_01, const glm::vec3 &point_02, const glm::vec3 &point_03)
    {
        glm::vec3 edge_01, edge_02;
        edge_01 = point_01 - point_02;
        edge_02 = point_03 - point_02;
        glm::vec3 normal = glm::normalize(glm::cross(edge_01, edge_02));
        f32 offset = -glm::dot(normal, point_02);
        return glm::vec4(normal, offset);
    }
    
    inline f32 frustum_3d::get_distance_to_plane(const glm::vec4& plane, const glm::vec3& point)
    {
        static glm::vec3 abc = glm::vec3(0.f);
        abc = frustum_3d::get_plane_abc(plane);
        static f32 d = 0.f;
        d = frustum_3d::get_plane_d(plane);
        return (d + glm::dot(abc, point)) * -1.f;
    }
    
    inline glm::vec3 frustum_3d::get_plane_abc(const glm::vec4& plane)
    {
        return glm::vec3(plane.x, plane.y, plane.z);
    }
    
    inline f32 frustum_3d::get_plane_d(const glm::vec4& plane)
    {
        return plane.w;
    }
    
    void frustum_3d::update(f32 dt)
    {
        if(!m_camera.expired())
        {
            f32 tan = tanf(glm::radians(m_camera.lock()->get_fov() * .6f));
            f32 near_height = m_camera.lock()->get_near() * tan;
            f32 near_width = near_height * m_camera.lock()->get_aspect();
            f32 far_height = m_camera.lock()->get_far()  * tan;
            f32 far_width = far_height * m_camera.lock()->get_aspect();
            
            glm::vec3 basis_z = glm::normalize(m_camera.lock()->get_position() - m_camera.lock()->get_look_at());
            glm::vec3 basis_x = glm::normalize(glm::cross(glm::vec3(0.f, 1.f, 0.f), basis_z));
            glm::vec3 basis_y = glm::cross(basis_z, basis_x);
            
            glm::vec3 near_offset = m_camera.lock()->get_position() - basis_z * m_camera.lock()->get_near();
            glm::vec3 far_offset = m_camera.lock()->get_position() - basis_z * m_camera.lock()->get_far();
            
            glm::vec3 near_top_left_point = near_offset + basis_y * near_height - basis_x * near_width;
            glm::vec3 near_top_right_point = near_offset + basis_y * near_height + basis_x * near_width;
            glm::vec3 near_bottom_left_point = near_offset - basis_y * near_height - basis_x * near_width;
            glm::vec3 near_bottom_right_point = near_offset - basis_y * near_height + basis_x * near_width;
            
            glm::vec3 far_top_left_point = far_offset + basis_y * far_height - basis_x * far_width;
            glm::vec3 far_top_right_point = far_offset + basis_y * far_height + basis_x * far_width;
            glm::vec3 far_bottom_left_point = far_offset - basis_y * far_height - basis_x * far_width;
            glm::vec3 far_bottom_right_point = far_offset - basis_y * far_height + basis_x * far_width;
            
            m_planes[e_frustum_plane_top] = frustum_3d::create_plane(near_top_right_point, near_top_left_point, far_top_left_point);
            m_planes[e_frustum_plane_bottom] = frustum_3d::create_plane(near_bottom_left_point, near_bottom_right_point, far_bottom_right_point);
            m_planes[e_frustum_plane_left] = frustum_3d::create_plane(near_top_left_point, near_bottom_left_point, far_bottom_left_point);
            m_planes[e_frustum_plane_right] = frustum_3d::create_plane(near_bottom_right_point, near_top_right_point, far_bottom_right_point);
            m_planes[e_frustum_plane_near] = frustum_3d::create_plane(near_top_left_point, near_top_right_point, near_bottom_right_point);
            m_planes[e_frustum_plane_far] = frustum_3d::create_plane(far_top_right_point, far_top_left_point, far_bottom_left_point);
        }
    }
    
    frustum_3d::e_frustum_bounds_result frustum_3d::is_point_in_frustum(const glm::vec3& point)
    {
        for(ui32 i = 0; i < e_frustum_plane_max; ++i)
        {
            if(frustum_3d::get_distance_to_plane(m_planes[i], point) < 0.f)
            {
                return e_frustum_bounds_result_outside;
            }
        }
        return e_frustum_bounds_result_inside;
    }
    
    frustum_3d::e_frustum_bounds_result frustum_3d::is_sphere_in_frustum(const glm::vec3& center, f32 radius)
    {
        e_frustum_bounds_result result = e_frustum_bounds_result_inside;
        for(ui32 i = 0; i < e_frustum_plane_max; ++i)
        {
            f32 distance = frustum_3d::get_distance_to_plane(m_planes[i], center);
            if (distance < -radius)
            {
                return e_frustum_bounds_result_outside;
            }
            else if (distance < radius)
            {
                result =  e_frustum_bounds_result_intersect;
            }
        }
        return result;
    }
    
#define k_max_points_in_bounding_box 8
    
    frustum_3d::e_frustum_bounds_result frustum_3d::is_bound_box_in_frustum(const glm::vec3& max_bound,
                                                                            const glm::vec3& min_bound,
                                                                            const glm::mat4& matrix_m)
    {
        glm::vec3 points[k_max_points_in_bounding_box];
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
        
        e_frustum_bounds_result result = e_frustum_bounds_result_inside;
        ui32 points_in, points_out;
        
        for(ui32 i = 0; i < e_frustum_plane_max; ++i)
        {
            points_in = 0;
            points_out = 0;
            for (ui32 j = 0; j < k_max_points_in_bounding_box && (points_in == 0 || points_out == 0); ++j)
            {
                frustum_3d::get_distance_to_plane(m_planes[i], points[j]) < 0.f ? ++points_out : ++points_in;
            }
            if (!points_in)
            {
                result = e_frustum_bounds_result_outside;
                break;
            }
            else if (points_out)
            {
                result = e_frustum_bounds_result_intersect;
            }
        }
        return result;
    }
};
