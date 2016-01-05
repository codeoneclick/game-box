//
//  shader_direction_light.cpp
//  gbCore
//
//  Created by sergey.sergeev on 9/22/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "main_headers.h"

const char* shader_direction_light_vert = string_shader
(
#if defined(__OPENGL_30__)
 
 out vec2 v_texcoord;
 out vec4 v_screen_position;
 
#else
 
 varying vec2 v_texcoord;
 varying vec4 v_screen_position;
 
#endif
 
 void main(void)
{
    gl_Position = vec4(a_position, 1.0);
    
    v_texcoord = a_texcoord;
    v_screen_position = gl_Position;
}
 );

const char* shader_direction_light_frag = string_shader
(
#if defined(__OPENGL_30__)
 
 in vec2 v_texcoord;
 in vec4 v_screen_position;
 
#else
 
 varying vec2 v_texcoord;
 varying vec4 v_screen_position;
 
#endif
 
 uniform mat4 u_mat_i_vp;
 uniform vec3 u_light_direction;
 uniform vec3 u_vec_camera_position;
 uniform vec4 u_light_color;
 uniform float u_light_intensity;
 
 uniform sampler2D  sampler_01;
 uniform sampler2D  sampler_02;
 
#define __SPECULAR__
#if defined(__SPECULAR__)
 
 float specular_square = 16.0;
 float specular_power = 4.0;
 
#endif
 
 void main()
{
    vec4 screen_position = v_screen_position;
    screen_position.xy /= screen_position.w;
    vec2 texcoord = 0.5 * (screen_position.xy + 1.0);
    
    vec4 color = texture2D(sampler_01, texcoord);
    vec3 normal = color.rgb * 2.0 - 1.0;
    vec3 diffuse = vec3(clamp(dot(normal, u_light_direction), 1.0 - u_light_intensity, 1.0));
    
#if defined(__SPECULAR__)
    
    float depth = texture2D(sampler_02, texcoord).r;
    
    vec4 position = vec4(texcoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    position = u_mat_i_vp * position;
    position.xyz = position.xyz / position.w;
    
    vec3 camera_direction = normalize(position.xyz - u_vec_camera_position);
    
    float specular_intensity = color.a;
    vec3 half_vector = normalize(u_light_direction + camera_direction);
    float specular = pow(clamp(dot(normal, half_vector), 0.0, 1.0), specular_square) * specular_intensity * specular_power;
    
#else
    
    float specular = 0.0;
    
#endif
    
    gl_FragColor = vec4((diffuse * u_light_color.rgb + specular * u_light_color.rgb) * u_light_intensity, 1.0);
}
 );
