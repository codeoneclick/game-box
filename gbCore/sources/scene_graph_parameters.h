//
//  scene_graph_parameters.h
//  gbCore
//
//  Created by sergey.sergeev on 12/16/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef scene_graph_parameters_h
#define scene_graph_parameters_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class scene_graph_parameters
    {
    private:
        
    protected:
        
        camera_shared_ptr m_main_camera;
        shadow_cast_light_shared_ptr m_shadow_cast_light;
        
    public:
        
        scene_graph_parameters();
        virtual ~scene_graph_parameters();
        
        inline glm::mat4 get_eye_matrix_v() const;
        inline glm::mat4 get_eye_matrix_i_v() const;
        inline glm::mat4 get_eye_matrix_i_vp() const;
        inline glm::mat4 get_eye_matrix_p() const;
        inline glm::mat4 get_eye_matrix_n() const;
        
        inline glm::vec3 get_eye_position() const;
        
        inline f32 get_eye_near() const;
        inline f32 get_eye_far() const;
        
        inline glm::mat4 get_shadow_cast_light_matrix_v() const;
        inline glm::mat4 get_shadow_cast_light_matrix_p() const;
        
        inline glm::vec3 get_shadow_cast_light_position() const;
        
        inline glm::ivec4 get_viewport() const;
        
        inline glm::mat4 get_matrix_s(const glm::vec3& position);
        inline glm::mat4 get_matrix_c(const glm::vec3& position);
    };
};

#include "scene_graph_parameters.hpp"

#endif
