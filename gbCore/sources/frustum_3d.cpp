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
    frustum_3d::frustum_3d()
    {
        
    }
    
    frustum_3d::~frustum_3d()
    {
        
    }
    
    void frustum_3d::create_plane(const glm::vec3 &point_01, const glm::vec3 &point_02, const glm::vec3 &point_03, glm::vec4* out)
    {
        glm::vec3 edge_01, edge_02;
        edge_01 = point_01 - point_02;
        edge_02 = point_03 - point_02;
        glm::vec3 normal = glm::normalize(glm::cross(edge_01, edge_02));
        f32 offset = -glm::dot(normal, point_02);
        out->x = normal.x;
        out->y = normal.y;
        out->z = normal.z;
        out->w = offset;
    }
    
    inline f32 frustum_3d::get_distance_to_plane(const glm::vec4& plane, const glm::vec3& point)
    {
        static glm::vec3 abc;
        abc.x = plane.x; abc.y = plane.y; abc.z = plane.z;
        static f32 d;
        d = plane.w;
        return (d + glm::dot(abc, point)) * -1.f;
    }
    
    void frustum_3d::update(const camera_3d_shared_ptr& camera)
    {
        f32 tan = tanf(glm::radians(camera->get_fov() * .5f));
        f32 near_height = camera->get_near() * tan;
        f32 near_width = near_height * camera->get_aspect();
        f32 far_height = camera->get_far()  * tan;
        f32 far_width = far_height * camera->get_aspect();
        
        glm::vec3 camera_z_axis = glm::normalize(camera->get_position() - camera->get_look_at());
        glm::vec3 camera_x_axis = glm::normalize(glm::cross(camera->get_up(), camera_z_axis));
        glm::vec3 camera_y_axis = glm::cross(camera_z_axis, camera_x_axis);
        
        glm::vec3 near_center = camera->get_position() - camera_z_axis * camera->get_near();
        glm::vec3 far_center = camera->get_position() - camera_z_axis * camera->get_far();
        
        glm::vec3 near_top_left_point = near_center + camera_y_axis * near_height - camera_x_axis * near_width;
        glm::vec3 near_top_right_point = near_center + camera_y_axis * near_height + camera_x_axis * near_width;
        glm::vec3 near_bottom_left_point = near_center - camera_y_axis * near_height - camera_x_axis * near_width;
        glm::vec3 near_bottom_right_point = near_center - camera_y_axis * near_height + camera_x_axis * near_width;
        
        glm::vec3 far_top_left_point = far_center + camera_y_axis * far_height - camera_x_axis * far_width;
        glm::vec3 far_top_right_point = far_center + camera_y_axis * far_height + camera_x_axis * far_width;
        glm::vec3 far_bottom_left_point = far_center - camera_y_axis * far_height - camera_x_axis * far_width;
        glm::vec3 far_bottom_right_point = far_center - camera_y_axis * far_height + camera_x_axis * far_width;
        
        frustum_3d::create_plane(near_top_right_point, near_top_left_point, far_top_left_point, &m_planes[e_frustum_plane_top]);
        frustum_3d::create_plane(near_bottom_left_point, near_bottom_right_point, far_bottom_right_point, &m_planes[e_frustum_plane_bottom]);
        frustum_3d::create_plane(near_top_left_point, near_bottom_left_point, far_bottom_left_point, &m_planes[e_frustum_plane_left]);
        frustum_3d::create_plane(near_bottom_right_point, near_top_right_point, far_bottom_right_point, &m_planes[e_frustum_plane_right]);
        frustum_3d::create_plane(near_top_left_point, near_top_right_point, near_bottom_right_point, &m_planes[e_frustum_plane_near]);
        frustum_3d::create_plane(far_top_right_point, far_top_left_point, far_bottom_left_point, &m_planes[e_frustum_plane_far]);
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
        static glm::vec3 points[k_max_points_in_bounding_box];
        static glm::vec3 absolute_min_bound;
        static glm::vec3 absolute_max_bound;
        glm::transform(min_bound, matrix_m, &absolute_min_bound);
        glm::transform(max_bound, matrix_m, &absolute_max_bound);
        
        points[0] = absolute_min_bound;
        points[1].x = absolute_min_bound.x; points[1].y = absolute_min_bound.y; points[1].z = absolute_max_bound.z;
        points[2].x = absolute_max_bound.x; points[2].y = absolute_min_bound.y; points[2].z = absolute_min_bound.z;
        points[3].x = absolute_max_bound.x; points[3].y = absolute_min_bound.y; points[3].z = absolute_max_bound.z;
        
        points[4] = absolute_max_bound;
        points[5].x = absolute_max_bound.x; points[5].y = absolute_max_bound.y; points[5].z = absolute_min_bound.z;
        points[6].x = absolute_min_bound.x; points[6].y = absolute_max_bound.y; points[6].z = absolute_max_bound.z;
        points[7].x = absolute_min_bound.x; points[7].y = absolute_max_bound.y; points[7].z = absolute_min_bound.z;
        
        e_frustum_bounds_result result = e_frustum_bounds_result_inside;
        ui32 points_in, points_out;
        
        for(i32 i = 0; i < e_frustum_plane_max; ++i)
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
