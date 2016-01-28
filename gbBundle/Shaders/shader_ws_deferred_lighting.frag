#if defined(__OPENGL_30__)

in vec2 v_position;
in vec4 v_screen_position;

#else

varying vec2 v_position;
varying vec4 v_screen_position;

#endif

uniform int u_mask_flag_fs;
uniform vec4 u_color;

uniform sampler2D sampler_01;

void main()
{
    vec4 color = vec4(0.0);
    if(u_mask_flag_fs == 0)
    {
        vec4 screen_position = v_screen_position;
        screen_position.xy /= screen_position.w;
        vec2 texcoord = 0.5 * (screen_position.xy + 1.0);
        
        vec3 light_direction = normalize(vec3(-v_position, 0.1));
        vec3 normal = texture2D(sampler_01, texcoord).rgb * 2.0 - 1.0;
        float diffuse = clamp(dot(normal, light_direction), 0.0, 1.0);
        
        float attenuation = 1.0 - length(v_position);
        color = vec4(u_color.rgb * diffuse, attenuation);
    }
    gl_FragColor = color;
}