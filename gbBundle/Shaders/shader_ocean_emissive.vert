
uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;

void main(void)
{
    vec4 position = vec4(a_position, 1.0);
    gl_Position = u_mat_p * u_mat_v * u_mat_m * position;
}