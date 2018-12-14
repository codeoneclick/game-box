//
//  shader_texture_2d.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "main_headers.h"

const char* shader_screen_quad_tex2d_vert = string_shader
(
#if USED_GRAPHICS_API == OPENGL_30_API
 
 out vec2 v_texcoord;
 
#elif USED_GRAPHICS_API == OPENGL_20_API
 
 varying vec2 v_texcoord;
 
#endif
 
 void main(void)
{
    v_texcoord = a_texcoord;
    gl_Position = vec4(a_position, 1.0);
}
 );

const char* shader_screen_quad_tex2d_frag = string_shader
(
#if USED_GRAPHICS_API == OPENGL_30_API
 
 in vec2 v_texcoord;
 
#elif USED_GRAPHICS_API == OPENGL_20_API
 
 varying vec2 v_texcoord;
 
#endif
 
 uniform sampler2D  sampler_01;
 
 void main()
{
    gl_FragColor = texture2D(sampler_01, v_texcoord);
}
 );
