
#if defined(__OPENGL_30__)

out vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_p;

void main(void)
{
    vec4 position = vec4(a_position, 0.0, 1.0);
    position = u_mat_m * position;
    gl_Position = u_mat_p * position;
    v_texcoord = a_texcoord;
}