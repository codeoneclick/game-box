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
    class omni_deferred_light_source_3d : public game_object_3d
    {
    public:
        
        class shader_uniforms : public ces_shader_uniforms_component::shader_uniforms
        {
        public:
            
            struct values
            {
                glm::mat4 m_mat_i_vp;
                glm::vec4 m_frame_size;
                glm::vec4 m_center_radius;
                glm::vec4 m_color;
               
            } __attribute__ ((aligned(256)));
            
        private:
            
        protected:
            
            values m_values;
            
        public:
            
            shader_uniforms(ces_shader_uniforms_component::e_shader_uniform_type type);
            
            void* get_values() override;
            ui32 get_values_size() override;
            
            void set_mat_i_vp(const glm::mat4& mat_i_vp);
            void set_frame_size(const glm::vec2& size);
            void set_center(const glm::vec3& center);
            void set_radius(f32 radius);
            void set_color(const glm::vec4& color);
        };
        
    private:
        
    protected:
        
    public:
        
        omni_deferred_light_source_3d();
        ~omni_deferred_light_source_3d();
        
        void setup_components() override;
        
        std::property_rw<f32> radius;
        std::property_rw<glm::vec4> color;
    };
};

