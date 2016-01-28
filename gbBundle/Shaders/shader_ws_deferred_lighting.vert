
#if defined(__OPENGL_30__)

out vec2 v_position;
out vec4 v_screen_position;

#else

varying vec2 v_position;
varying vec4 v_screen_position;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_p;
uniform mat4 u_mat_v;

uniform int u_mask_flag_vs;

uniform vec2 u_center;

void main()
{
    vec4 position = vec4(a_position, 0.0, 1.0);
    if(u_mask_flag_vs == 0)
    {
        gl_Position = u_mat_p * u_mat_v * u_mat_m * position;
    }
    else
    {
        gl_Position = position;
    }
    
    v_position = a_position;
    v_screen_position = gl_Position;
}