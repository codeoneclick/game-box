//
//  camera.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef camera_hpp
#define camera_hpp

#include "main_headers.h"
#include "std_extensions.h"

namespace gb
{
    class camera
    {
    private:
        
    protected:
        
        glm::mat4 m_mat_t;
        glm::mat4 m_mat_r;
        glm::mat4 m_mat_s;
        
        bool m_is_matrix_m_computed;
        
        glm::mat4 m_mat_p;
        glm::mat4 m_mat_v;
        
        glm::vec2 m_position;
        f32 m_zoom;
        f32 m_rotation;
        glm::vec2 m_pivot;
        
        glm::vec2 m_position_cache;
        f32 m_rotation_cache;
        
        glm::ivec2 m_screen_size;
        
    public:
        
        camera(i32 width, i32 height);
        ~camera();
        
        void set_position(const glm::vec2& position);
        glm::vec2 get_position() const;
        
        void set_zoom(f32 zoom);
        f32 get_zoom() const;
        
        void set_rotation(f32 rotation);
        f32 get_rotation() const;
        
        glm::mat4 get_mat_p() const;
        glm::mat4 get_mat_v();
        
        std::property_ro<glm::ivec2> screen_size;
        std::property_ro<glm::vec4> bound;
        std::property_rw<glm::vec2> pivot;
    };
};

#endif
