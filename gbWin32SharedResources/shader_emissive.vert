
#if defined(__OPENGL_30__)

#else

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_p;
uniform mat4 u_mat_v;

void main()
{
     gl_Position = u_mat_p * u_mat_v * u_mat_m * vec4(a_position, 1.0);
}
