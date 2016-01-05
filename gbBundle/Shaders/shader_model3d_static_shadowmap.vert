#if defined(__OPENGL_30__)

out vec2 v_texcoord;
out vec4 v_shadow_parameters;
out mat3 v_tbn_matrix;
out vec3 v_position;

#else

varying vec2 v_texcoord;
varying vec4 v_shadow_parameters;
varying mat3 v_tbn_matrix;
varying vec3 v_position;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;
uniform vec4 u_vec_clip;
uniform mat4 u_mat_global_light_p;
uniform mat4 u_mat_global_light_v;


const mat4 bias_matrix = mat4(0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 0.5, 0.0,
                              0.5, 0.5, 0.5, 1.0);

void main(void)
{
    vec4 position = u_mat_m * vec4(a_position, 1.0);
    gl_Position = u_mat_p * u_mat_v * position;
    gl_ClipDistance[0] = dot(position.xyz, u_vec_clip.xyz);
    
    v_shadow_parameters = bias_matrix * u_mat_global_light_p * u_mat_global_light_v * position;
    
    v_texcoord = a_texcoord;
    
    vec3 normal = normalize(u_mat_m * a_normal).xyz;
    vec3 tangent = normalize(u_mat_m * a_tangent).xyz;
    vec3 bitangent = cross(-normal, tangent);
    
    v_tbn_matrix = mat3(tangent, bitangent, normal);
    
    v_position = position.xyz;
}