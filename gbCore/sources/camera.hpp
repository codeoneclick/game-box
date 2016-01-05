//
//  camera.hpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef camera_hpp
#define camera_hpp

#include "ces_camera_component.h"

namespace gb
{
    inline glm::mat4 camera::get_matrix_v() const
    {
        return unsafe_get_camera_component_from_this->get_matrix_v();
    }
    
    inline glm::mat4 camera::get_matrix_i_v() const
    {
        return unsafe_get_camera_component_from_this->get_matrix_i_v();
    }
    
    inline glm::mat4 camera::get_matrix_i_vp() const
    {
        return unsafe_get_camera_component_from_this->get_matrix_i_vp();
    }
    
    inline glm::mat4 camera::get_matrix_p() const
    {
        return unsafe_get_camera_component_from_this->get_matrix_p();
    }
    
    inline glm::mat4 camera::get_matrix_n() const
    {
        return unsafe_get_camera_component_from_this->get_matrix_n();
    }
    
    inline void camera::set_position(const glm::vec3& position)
    {
        unsafe_get_camera_component_from_this->set_position(position);
    }
    
    inline glm::vec3 camera::get_position() const
    {
        return unsafe_get_camera_component_from_this->get_position();
    }
    
    inline void camera::set_look_at(const glm::vec3& look_at)
    {
        unsafe_get_camera_component_from_this->set_look_at(look_at);
    }
    
    inline glm::vec3 camera::get_look_at() const
    {
        return unsafe_get_camera_component_from_this->get_look_at();
    }
    
    inline void camera::set_up(const glm::vec3& up)
    {
        unsafe_get_camera_component_from_this->set_up(up);
    }
    
    inline glm::vec3 camera::get_up() const
    {
        return unsafe_get_camera_component_from_this->get_up();
    }
    
    inline void camera::set_fov(f32 fov)
    {
        unsafe_get_camera_component_from_this->set_fov(fov);
    }
    
    inline f32 camera::get_fov() const
    {
        return unsafe_get_camera_component_from_this->get_fov();
    }
    
    inline f32 camera::get_aspect() const
    {
        return unsafe_get_camera_component_from_this->get_aspect();
    }
    
    inline f32 camera::get_near() const
    {
        return unsafe_get_camera_component_from_this->get_near();
    }
    
    inline void camera::set_near(f32 near)
    {
        unsafe_get_camera_component_from_this->set_near(near);
    }
    
    inline f32 camera::get_far() const
    {
        return unsafe_get_camera_component_from_this->get_far();
    }
    
    inline void camera::set_far(f32 far)
    {
        unsafe_get_camera_component_from_this->set_far(far);
    }
    
    inline glm::ivec4 camera::get_viewport() const
    {
        return unsafe_get_camera_component_from_this->get_viewport();
    }
    
    inline void camera::set_viewport(const glm::ivec4 &viewport)
    {
        unsafe_get_camera_component_from_this->set_viewport(viewport);
    }
    
    inline frustum_shared_ptr camera::get_frustum() const
    {
        return unsafe_get_camera_component_from_this->get_frustum();
    }
    
    inline f32 camera::get_yaw() const
    {
        return unsafe_get_camera_component_from_this->get_yaw();
    }
    
    inline f32 camera::get_pitch() const
    {
        return unsafe_get_camera_component_from_this->get_pitch();
    }
    
    inline f32 camera::get_roll() const
    {
        return unsafe_get_camera_component_from_this->get_roll();
    }
    
    inline void camera::set_horizontal_angle(f32 angle)
    {
        unsafe_get_camera_component_from_this->set_horizontal_angle(angle);
    }
    
    inline f32 camera::get_horizontal_angle() const
    {
        return unsafe_get_camera_component_from_this->get_horizontal_angle();
    }
    
    inline void camera::set_vertical_angle(f32 angle)
    {
        unsafe_get_camera_component_from_this->set_vertical_angle(angle);
    }
    
    inline f32 camera::get_vertical_angle() const
    {
        return unsafe_get_camera_component_from_this->get_vertical_angle();
    }
    
    inline glm::vec3 camera::get_direction() const
    {
        return unsafe_get_camera_component_from_this->get_direction();
    }
    
    inline glm::mat4 camera::get_matrix_s(const glm::vec3& position)
    {
        return unsafe_get_camera_component_from_this->get_matrix_s(position);
    }
    
    inline glm::mat4 camera::get_matrix_c(const glm::vec3& position)
    {
        return unsafe_get_camera_component_from_this->get_matrix_c(position);
    }
}

#endif
