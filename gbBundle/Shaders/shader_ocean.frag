
#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec4 v_texcoord_proj;
in vec2 v_texcoord_displace_01;
in vec2 v_texcoord_displace_02;

in float v_extra_parameter;

in vec3 v_eye_position_ws;
in vec3 v_vertex_position_ws;

in float v_fog_distance;

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

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;
uniform sampler2D sampler_03;
uniform sampler2D sampler_04;

const vec2 k_perturbation_factor = vec2(0.025, 0.025);

const float k_0 = 0.0;
const float k_1 = 1.0;
const float k_2 = 2.0;
const float k_05 = 0.5;
const float k_025 = 0.25;
const float k_033 = 0.33;

const vec4 k_default_normal = vec4(k_0, k_1, k_0, k_1);

const vec4 k_water_color_blue = vec4(0.1, 0.15, 0.25, 1.0);
const vec4 k_water_color_green = vec4(0.16, 0.32, 0.16, 1.0);

const vec4 fog_color = vec4(0.0);

void main()
{
    vec2 texcoord_proj = v_texcoord_proj.xy;
    texcoord_proj = k_05 + k_05 * texcoord_proj / v_texcoord_proj.w * vec2(-k_1, k_1);
    
    vec3 ripples = texture2D(sampler_03, v_texcoord_displace_01).rgb;
    ripples += texture2D(sampler_03, v_texcoord_displace_02).rgb;
    ripples -= k_1;
    
    vec3 eye_direction_ws = normalize(v_eye_position_ws - v_vertex_position_ws);
    float fresnel = dot(k_default_normal.xyz, eye_direction_ws);
    
    vec2 perturbation_intensity = k_perturbation_factor * ripples.xy;
    vec2 perturbated_texcoord = texcoord_proj + perturbation_intensity;
    
    vec4 reflection_color = texture2D(sampler_01, perturbated_texcoord);
    vec4 refraction_color = texture2D(sampler_02, vec2(k_05 + (k_05 - perturbated_texcoord.x), perturbated_texcoord.y));
    
    vec4 color = mix(reflection_color, refraction_color, fresnel);
    color += k_water_color_blue;
    color.a = k_1;
    
    float fog_distance = length(vec3(192.0 * 0.5, 0.0, 192.0 * 0.5) - v_vertex_position_ws) / (192.0 * 2.0);
    fog_distance = clamp(fog_distance, 0.0, 1.0);
    color = mix(color, fog_color, fog_distance);
    
    gl_FragColor = color;
}
