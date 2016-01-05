//
//  i_external_scene_graph.hpp
//  gbCore
//
//  Created by sergey.sergeev on 12/16/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "camera.h"
#include "shadow_cast_light.h"

namespace gb
{
    inline glm::mat4 scene_graph_parameters::get_eye_matrix_v() const
    {
        assert(m_main_camera);
        return m_main_camera->get_matrix_v();
    }
    
    inline glm::mat4 scene_graph_parameters::get_eye_matrix_i_v() const
    {
        assert(m_main_camera);
        return m_main_camera->get_matrix_i_v();
    }
    
    inline glm::mat4 scene_graph_parameters::get_eye_matrix_i_vp() const
    {
        assert(m_main_camera);
        return m_main_camera->get_matrix_i_vp();
    }
    
    inline glm::mat4 scene_graph_parameters::get_eye_matrix_p() const
    {
        assert(m_main_camera);
        return m_main_camera->get_matrix_p();
    }
    
    inline glm::mat4 scene_graph_parameters::get_eye_matrix_n() const
    {
        assert(m_main_camera);
        return m_main_camera->get_matrix_n();
    }
    
    inline glm::vec3 scene_graph_parameters::get_eye_position() const
    {
        assert(m_main_camera);
        return m_main_camera->get_position();
    }
    
    inline f32 scene_graph_parameters::get_eye_near() const
    {
        assert(m_main_camera);
        return m_main_camera->get_near();
    }
    
    inline f32 scene_graph_parameters::get_eye_far() const
    {
        assert(m_main_camera);
        return m_main_camera->get_far();
    }
    
    inline glm::ivec4 scene_graph_parameters::get_viewport() const
    {
        assert(m_main_camera);
        return m_main_camera->get_viewport();
    }
    
    inline glm::mat4 scene_graph_parameters::get_matrix_s(const glm::vec3& position)
    {
        assert(m_main_camera);
        return m_main_camera->get_matrix_s(position);
    }
    
    inline glm::mat4 scene_graph_parameters::get_matrix_c(const glm::vec3& position)
    {
        assert(m_main_camera);
        return m_main_camera->get_matrix_c(position);
    }
    
    inline glm::mat4 scene_graph_parameters::get_shadow_cast_light_matrix_v() const
    {
        assert(m_shadow_cast_light);
        return m_shadow_cast_light->get_matrix_v();
    }
    
    inline glm::mat4 scene_graph_parameters::get_shadow_cast_light_matrix_p() const
    {
        assert(m_shadow_cast_light);
        return m_shadow_cast_light->get_matrix_p();
    }
    
    inline glm::vec3 scene_graph_parameters::get_shadow_cast_light_position() const
    {
        assert(m_shadow_cast_light);
        return m_shadow_cast_light->get_position();
    }
};