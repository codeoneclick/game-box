
#if defined(__OPENGL_30__)

out vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform mat4 u_mat_v;
uniform mat4 u_mat_p;
uniform vec4 u_vec_clip;
uniform mat4 u_transform_parameters[16];

void main(void)
{
    vec4 position = vec4(a_position, 1.0);
    position = u_transform_parameters[gl_InstanceID] * position;
    gl_Position = u_mat_p * u_mat_v * position;
    gl_ClipDistance[0] = dot(position.xyz, u_vec_clip.xyz);
    v_texcoord = a_texcoord;
}