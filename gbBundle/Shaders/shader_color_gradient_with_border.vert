
#if defined(__OPENGL_30__)

out vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_p;
uniform mat4 u_mat_v;

void main()
{
    gl_Position = u_mat_p * u_mat_v * u_mat_m * vec4(a_position, 0.0, 1.0);
    v_texcoord = a_texcoord;
}