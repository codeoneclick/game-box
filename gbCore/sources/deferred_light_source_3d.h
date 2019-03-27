//
//  omni_deffered_light_source.hpp
//  gbCore
//
//  Created by serhii.s on 2/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_object_3d.h"
#include "ces_shader_uniforms_component.h"

namespace gb
{
    class deferred_light_source_3d : public game_object_3d
    {
    public:
        
        class shader_uniforms : public ces_shader_uniforms_component::shader_uniforms
        {
        private:
            
        protected:
            
            struct values
            {
                glm::mat4 m_mat_i_vp;
                glm::vec4 m_frame_size;
                glm::vec4 m_center_ray_length;
                glm::vec4 m_color;
                glm::vec4 m_camera_position;
            } __attribute__ ((aligned(256)));
            
            values m_values;
            
        public:
            
            shader_uniforms(ces_shader_uniforms_component::e_shader_uniform_type type);
            
            void* get_values() override;
            ui32 get_values_size() override;
        };
        
    private:
        
    protected:
        
    public:
        
        deferred_light_source_3d();
        ~deferred_light_source_3d();
        
        virtual void setup_components() override;
        
        std::property_rw<f32> ray_length;
        std::property_rw<glm::vec4> color;
    };
};

