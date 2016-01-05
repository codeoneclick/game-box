
#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;
uniform sampler2D sampler_03;

void main()
{
    vec3 diffuse_color = texture2D(sampler_01, v_texcoord).rgb;
    vec3 emissive_color = texture2D(sampler_03, v_texcoord).rgb;
    vec3 diffuse_intensity = texture2D(sampler_02, v_texcoord).rgb;
    
    vec3  warm_color = vec3(0.75, 0.75, 0.0);
    vec3  cold_color = vec3(0.0, 0.0, 0.25);
    float warm_power  = 0.85;
    float cold_power  = 0.85;
    
    float diffuse = (diffuse_intensity.x + diffuse_intensity.y + diffuse_intensity.z) / 3.0;
    
    warm_color = min(warm_color + warm_power * diffuse_intensity.rgb, 1.0);
    cold_color = min(cold_color + cold_power * diffuse_intensity.rgb, 1.0);
    vec3 light_color = mix(cold_color, warm_color, diffuse);
    
    gl_FragColor = vec4(diffuse_color * (light_color + emissive_color), 1.0);
}

