//
//  game_objects_custom_uniforms.h
//  gbDemo
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_shader_uniforms_component.h"

namespace game
{
    class reflection_shader_uniforms : public gb::ces_shader_uniforms_component::shader_uniforms
    {
    public:
        
        static const std::string k_camera_position_uniform;
        static const std::string k_i_view_mat_uniform;
        
    private:
        
    protected:
        
#if defined(__WINOS__)
        
        // __declspec(align(256))
        
#endif
        
        struct values
        {
            glm::vec4 m_camera_position;
            glm::mat4 m_i_view_mat;
        }
        
#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)
        
        __attribute__ ((aligned(256)));
        
#else
        
        ;
        
#endif
        
        values m_values;
        
    public:
        
        reflection_shader_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name);
        
        void* get_values() override;
        ui32 get_values_size() override;
    };
    
    class car_colorization_shader_uniforms : public gb::ces_shader_uniforms_component::shader_uniforms
    {
    public:
        
        static const std::string k_body_color;
        static const std::string k_windows_color;
        
    private:
        
    protected:
        
#if defined(__WINOS__)
        
        // __declspec(align(256))
        
#endif
        
        struct values
        {
            glm::vec4 m_body_color;
            glm::vec4 m_windows_color;
        }
        
#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)
        
        __attribute__ ((aligned(256)));
        
#else
        
        ;
        
#endif
        
        values m_values;
        
    public:
        
        car_colorization_shader_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode, const std::string& name);
        
        void* get_values() override;
        ui32 get_values_size() override;
    };
};
