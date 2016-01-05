//
//  shader_wireframe_grid.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/21/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "main_headers.h"

const char* shader_wireframe_grid_vert = string_shader
(
 uniform mat4 u_mat_m;
 uniform mat4 u_mat_v;
 uniform mat4 u_mat_p;
 
 void main(void)
{
    vec4 position = u_mat_m * vec4(a_position, 1.0);
    gl_Position = u_mat_p * u_mat_v * position;
}
 );

const char* shader_wireframe_grid_frag = string_shader
(
 
 void main()
{
    gl_FragColor = vec4(1.0);
}
 );
