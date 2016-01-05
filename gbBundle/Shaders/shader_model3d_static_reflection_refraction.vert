
#if defined(__OPENGL_30__)

out vec2 v_texcoord;
out vec4 v_ss_position;

#else

varying vec2 v_texcoord;
varying vec4 v_ss_position;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;
uniform vec4 u_vec_clip;

const mat4 bias_matrix = mat4(0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 0.5, 0.0,
                              0.5, 0.5, 0.5, 1.0);

void main(void)
{
    vec4 position = vec4(a_position, 1.0);
    position = u_mat_m * position;
    
    gl_Position = u_mat_p * u_mat_v * position;
    gl_ClipDistance[0] = dot(position, u_vec_clip);
    
    v_texcoord = a_texcoord;
    v_ss_position = gl_Position;
}