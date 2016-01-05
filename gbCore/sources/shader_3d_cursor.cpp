//
//  shader_3d_cursor.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "main_headers.h"

const char* shader_3d_cursor_vert = string_shader
(
 uniform mat4 u_mat_m;
 uniform mat4 u_mat_v;
 uniform mat4 u_mat_p;
 
#if defined(__OPENGL_30__)
 
 out vec4 v_color;
 
#else
 
 varying vec4 v_color;
 
#endif
 
 void main(void)
{
    vec4 position = u_mat_m * vec4(a_position, 1.0);
    gl_Position = u_mat_p * u_mat_v * position;
    v_color = a_color / 255.0;
}
 );

const char* shader_3d_cursor_frag = string_shader
(
 
#if defined(__OPENGL_30__)
 
 out vec4 v_color;
 
#else
 
 varying vec4 v_color;
 
#endif
 
 void main()
{
    gl_FragColor = vec4(0.0, 1.0, 0.0, 0.5);
}
 );