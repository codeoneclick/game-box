//
//  ces_camera_component.h
//  gbCore
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_camera_component_h
#define ces_camera_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_camera_component : public ces_base_component
    {
    private:
        
        glm::mat4 m_matrix_v;
        glm::mat4 m_matrix_i_v;
        glm::mat4 m_matrix_i_vp;
        glm::mat4 m_matrix_p;
        glm::mat4 m_matrix_n;
        
        glm::vec3 m_position;
        glm::vec3 m_look_at;
        glm::vec3 m_up;
        
        f32 m_horizontal_angle;
        f32 m_vertical_angle;
        
        f32 m_fov;
        f32 m_aspect;
        f32 m_near;
        f32 m_far;
        
        glm::ivec4 m_viewport;
        
    protected:
        
        frustum_shared_ptr m_frustum;
        
    public:
        
        ces_camera_component();
        ~ces_camera_component();
        
        void update(f32 deltatime);
        
        inline glm::mat4 get_matrix_v() const;
        inline glm::mat4 get_matrix_i_v() const;
        inline glm::mat4 get_matrix_i_vp() const;
        inline glm::mat4 get_matrix_p() const;
        inline glm::mat4 get_matrix_n() const;
        
        inline void set_position(const glm::vec3& position);
        inline glm::vec3 get_position() const;
        
        inline void set_look_at(const glm::vec3& look_at);
        inline glm::vec3 get_look_at() const;
        
        inline void set_up(const glm::vec3& up);
        inline glm::vec3 get_up() const;
        
        inline void set_horizontal_angle(f32 angle);
        inline f32 get_horizontal_angle() const;
        
        inline void set_vertical_angle(f32 angle);
        inline f32 get_vertical_angle() const;
        
        inline void set_fov(f32 fov);
        inline f32 get_fov() const;
        
        inline f32 get_aspect() const;
        
        inline f32 get_near() const;
        inline void set_near(f32 near);
        
        inline f32 get_far() const;
        inline void set_far(f32 far);
        
        inline glm::ivec4 get_viewport() const;
        inline void set_viewport(const glm::ivec4& viewport);
        
        inline frustum_shared_ptr get_frustum() const;
        
        glm::mat4 get_matrix_s(const glm::vec3& position); // spherical
        glm::mat4 get_matrix_c(const glm::vec3& position); // cylindrical
        
        inline glm::vec3 get_direction() const;
        
        inline f32 get_yaw() const;
        inline f32 get_pitch() const;
        inline f32 get_roll() const;
    };
};

#include "ces_camera_component.hpp"

#endif
