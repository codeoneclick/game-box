
#if defined(__OPENGL_30__)

out vec2 v_texcoord;
out vec4 v_texcoord_proj;
out vec2 v_texcoord_displace_01;
out vec2 v_texcoord_displace_02;

out float v_extra_parameter;

out vec3 v_eye_position_ws;
out vec3 v_vertex_position_ws;

out float v_fog_distance;

#else

varying vec2 v_texcoord;
varying vec4 v_texcoord_proj;
varying vec2 v_texcoord_displace_01;
varying vec2 v_texcoord_displace_02;

varying float v_extra_parameter;

varying vec3 v_eye_position_ws;
varying vec3 v_vertex_position_ws;

varying float v_fog_distance;

#endif

uniform mat4 u_mat_m;
uniform mat4 u_mat_v;
uniform mat4 u_mat_p;

uniform vec3 u_vec_camera_position;
uniform float u_f32_timer;

const float k_texcoord_scale = 24.0;

void main(void)
{
    vec4 position = u_mat_m * vec4(a_position, 1.0);
    gl_Position = u_mat_p * u_mat_v * position;
    
    v_extra_parameter = a_extra.x;
    
    v_texcoord = a_texcoord;
    vec2 texcoord = a_texcoord;
    texcoord *= k_texcoord_scale;
    v_texcoord_displace_01 = vec2(texcoord.x + sin(u_f32_timer) * 0.25,
                                  texcoord.y - cos(u_f32_timer) * 0.75);
    
    v_texcoord_displace_02 = vec2(texcoord.x - sin(u_f32_timer) * 0.75,
                                  texcoord.y + cos(u_f32_timer) * 0.25);
    v_texcoord_proj = gl_Position;
    
    v_eye_position_ws = u_vec_camera_position;
    v_vertex_position_ws = position.xyz;
    
    v_fog_distance = length(vec3(256.0, 0.0, 256.0) - v_vertex_position_ws.xyz);
}
