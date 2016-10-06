
#if defined(__OPENGL_30__)

out vec2 v_position;
out vec2 v_texcoord;

#else

varying vec2 v_position;
varying vec2 v_texcoord;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_p;
uniform mat4 u_mat_v;

void main()
{
    gl_Position = u_mat_p * u_mat_v * u_mat_m * vec4(a_position, 1.0);
    v_position = (u_mat_m * vec4(a_position, 1.0)).xy;
    v_texcoord = a_texcoord;
}
