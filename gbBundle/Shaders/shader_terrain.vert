
#if defined(__OPENGL_30__)

out vec2 v_texcoord;
out vec3 v_camera_direction_ts;
out vec3 v_normal_ts;
out vec4 v_shadow_parameters;

#else

varying vec2 v_texcoord;
varying vec3 v_camera_direction_ts;
varying vec3 v_normal_ts;
varying vec4 v_shadow_parameters;

#endif

uniform vec4 u_vec_clip;
uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;

uniform mat4 u_mat_global_light_p;
uniform mat4 u_mat_global_light_v;

uniform vec3 u_vec_camera_position;

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
    
    vec3 normal = a_normal.xyz;
    vec3 tangent = a_tangent.xyz;
    vec3 bitangent = cross(-normal, tangent);
    
    mat3 mat_tangent_space = mat3(tangent, bitangent, normal);
    
    vec3 camera_direction = u_vec_camera_position - position.xyz;
    v_camera_direction_ts = camera_direction * mat_tangent_space;
    v_normal_ts = normal * mat_tangent_space;
    
    v_shadow_parameters = bias_matrix * u_mat_global_light_p * u_mat_global_light_v * position;
}