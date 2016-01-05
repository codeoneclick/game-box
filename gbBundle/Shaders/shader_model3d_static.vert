
#if defined(__OPENGL_30__)

out vec2 v_texcoord;
out mat3 v_tbn_matrix;
out vec3 v_position;

#else

varying vec2 v_texcoord;
varying mat3 v_tbn_matrix;
varying vec3 v_position;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;
uniform vec4 u_vec_clip;

void main(void)
{
    vec4 position = vec4(a_position, 1.0);
    position = u_mat_m * position;
    gl_Position = u_mat_p * u_mat_v * position;
    gl_ClipDistance[0] = dot(position.xyz, u_vec_clip.xyz);
    
    v_texcoord = a_texcoord;
    
    vec3 normal = normalize(u_mat_m * a_normal).xyz;
    vec3 tangent = normalize(u_mat_m * a_tangent).xyz;
    vec3 bitangent = cross(-normal, tangent);
    
    v_tbn_matrix = mat3(tangent, bitangent, normal);
    
    v_position = position.xyz;
}