
uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;
uniform vec4 u_vec_clip;

void main(void)
{
    vec4 position = vec4(a_position, 1.0);
    position = u_mat_m * position;
    
    gl_Position = u_mat_p * u_mat_v * position;
    gl_ClipDistance[0] = dot(position, u_vec_clip);
}