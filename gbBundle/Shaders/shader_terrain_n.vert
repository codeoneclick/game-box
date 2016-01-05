#if defined(__OPENGL_30__)

out vec2 v_texcoord;
out vec3 v_camera_direction_ts;
out vec3 v_normal_ts;
out mat3 v_mat_tangent_space;

#else

varying vec2 v_texcoord;
varying vec3 v_camera_direction_ts;
varying vec3 v_normal_ts;
varying mat3 v_mat_tangent_space;

#endif

uniform vec4 u_vec_clip;
uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;

uniform vec3 u_vec_camera_position;

void main()
{
    vec4 position = vec4(a_position, 1.0);
    position = u_mat_m * position;
    gl_Position = u_mat_p * u_mat_v * position;
    gl_ClipDistance[0] = dot(position, u_vec_clip);
    
    v_texcoord = a_texcoord;
    
    vec3 normal = a_normal.xyz;
    vec3 tangent = a_tangent.xyz;
    vec3 bitangent = cross(-normal, tangent);
    
    v_mat_tangent_space = mat3(tangent, bitangent, normal);
    
    vec3 camera_direction = u_vec_camera_position - position.xyz;
    v_camera_direction_ts = camera_direction * v_mat_tangent_space;
    v_normal_ts = normal * v_mat_tangent_space;
}