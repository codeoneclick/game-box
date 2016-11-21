
#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform int u_lighting;

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;
uniform sampler2D sampler_03;

void main()
{
    vec3 diffuse_color = texture2D(sampler_01, v_texcoord).rgb;
    if(u_lighting == 1)
    {
        vec3 diffuse_intensity = texture2D(sampler_02, v_texcoord).rgb + vec3(0.25);
        vec3 emissive_color = texture2D(sampler_03, v_texcoord).rgb;
        diffuse_color = diffuse_color * (diffuse_intensity + emissive_color);
    }
    gl_FragColor = vec4(diffuse_color, 1.0);
}

