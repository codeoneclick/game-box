//
//  shader_splatting_tex2d.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "main_headers.h"

const char* shader_splatting_tex2d_vert = string_shader
(
#if defined(__OPENGL_30__)
 
 out vec2 v_texcoord;
 out vec2 v_scaled_texcoord;
 out vec2 v_position;
 
#else
 
 varying vec2 v_texcoord;
 varying vec2 v_scaled_texcoord;
 varying vec2 v_position;
 
#endif
 
 const float k_texcoord_scale = 4.0;
 
 void main(void)
{
    v_texcoord = a_texcoord;
    v_scaled_texcoord = v_texcoord * k_texcoord_scale;
    v_position = a_position.xy;
    gl_Position = vec4(a_position, 1.0);
}
 );

const char* shader_splatting_tex2d_frag = string_shader
(
#if defined(__OPENGL_30__)
 
 in vec2 v_texcoord;
 in vec2 v_scaled_texcoord;
 in vec2 v_position;
 
#else
 
 varying vec2 v_texcoord;
 varying vec2 v_scaled_texcoord;
 varying vec2 v_position;
 
#endif
 
 uniform sampler2D sampler_01;
 uniform sampler2D sampler_02;
 
 uniform vec2 u_point;
 
 void main()
{
    vec4 canvas = texture2D(sampler_01, v_texcoord);
    vec4 brush = texture2D(sampler_02, v_scaled_texcoord);
    
    gl_FragColor = mix(canvas, brush, clamp(1.0 - length(v_position - u_point) / 0.25, 0.0, 1.0));
}
 );
