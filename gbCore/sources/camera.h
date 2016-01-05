//
//  camera.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef camera_h
#define camera_h

#include "ces_entity.h"

namespace gb
{
    class camera : public ces_entity
    {
    private:
        
    protected:
        
    public:
        
        camera(f32 fov, f32 near, f32 far, glm::ivec4 viewport);
        ~camera();
        
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
        
        inline glm::mat4 get_matrix_s(const glm::vec3& position); // spherical
        inline glm::mat4 get_matrix_c(const glm::vec3& position); // cylindrical
        
        inline glm::vec3 get_direction() const;
        
        inline f32 get_yaw() const;
        inline f32 get_pitch() const;
        inline f32 get_roll() const;
    };
};

#include "camera.hpp"

#endif
