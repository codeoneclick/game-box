
#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec4 v_texcoord_proj;
in vec2 v_texcoord_displace_01;
in vec2 v_texcoord_displace_02;
in float v_extra_parameter;

#else

varying vec2 v_texcoord;
varying vec4 v_texcoord_proj;
varying vec2 v_texcoord_displace_01;
varying vec2 v_texcoord_displace_02;
varying float v_extra_parameter;

#endif

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;
uniform sampler2D sampler_04;

const float k_0 = 0.0;
const float k_1 = 1.0;
const float k_2 = 2.0;
const float k_05 = 0.5;

const vec2 k_perturbation_factor = vec2(0.025, 0.025);
const vec4 k_default_normal = vec4(k_0, k_1, k_0, k_1);

void main()
{
    vec2 texcoord_proj = v_texcoord_proj.xy;
    texcoord_proj = k_05 + k_05 * texcoord_proj / v_texcoord_proj.w * vec2(-k_1, k_1);
    
    vec3 ripples = texture2D(sampler_02, v_texcoord_displace_01).rgb;
    ripples += texture2D(sampler_02, v_texcoord_displace_02).rgb;
    ripples -= k_1;
    
    vec2 perturbation_intensity = k_perturbation_factor * ripples.xy;
    vec2 perturbated_texcoord = texcoord_proj + perturbation_intensity;
    
    vec4 color = texture2D(sampler_01, vec2(k_05 + (k_05 - perturbated_texcoord.x), perturbated_texcoord.y));
    color = mix(color, k_default_normal, v_extra_parameter);
    color.a = k_1;
    
    gl_FragColor = color;
}
