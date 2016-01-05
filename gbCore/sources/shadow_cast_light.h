//
//  shadow_cast_light.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef shadow_cast_light_h
#define shadow_cast_light_h

#include "main_headers.h"

namespace gb
{
    class shadow_cast_light
    {
    private:
        
    protected:
        
        glm::mat4 m_matrix_v;
        glm::mat4 m_matrix_p;

        glm::vec3 m_position;
        glm::vec3 m_look_at;
        
    public:
        
        shadow_cast_light(f32 fov, f32 near, f32 far);
        ~shadow_cast_light();
        
        glm::vec3 get_position() const;
        void set_position(const glm::vec3& position);
        
        void set_look_at(const glm::vec3& look_at);
        glm::vec3 get_look_at() const;
        
        glm::mat4 get_matrix_v() const;
        glm::mat4 get_matrix_p() const;
        
        void update(f32 deltatime);
    };
};

#endif
