//
//  sphere_deferred_light_source_3d.h
//  gbCore
//
//  Created by serhii.s on 3/22/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "deferred_light_source_3d.h"

namespace gb
{
    class deferred_point_light_3d : public deferred_light_source_3d
    {
    private:
        
        class shader_uniforms : public ces_shader_uniforms_component::shader_uniforms
        {
        private:
            
        protected:
            
#if defined(__WINOS__)
            
            // __declspec(align(256))
            
#endif
            struct values
            {
                glm::vec4 m_light_position_and_ray_length;
                glm::vec4 m_light_color;
                glm::vec4 m_camera_position;
            }
            
#if defined(__OSX__) || defined(__IOS__) || defined(__TVOS__)
            
            __attribute__ ((aligned(256)));
            
#else
            
            ;
            
#endif
            
            values m_values;
            
        public:
            
            shader_uniforms(ces_shader_uniforms_component::e_shader_uniform_type type);
            
            void* get_values() override;
            ui32 get_values_size() override;
        };
        
    protected:
        
    public:
        
        deferred_point_light_3d();
        ~deferred_point_light_3d();
        
        std::property_rw<f32> ray_length;
        
        void setup_components() override;
    };
};

