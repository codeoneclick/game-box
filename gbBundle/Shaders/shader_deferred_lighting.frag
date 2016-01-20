
#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;

void main()
{
    vec3 diffuse_color = texture2D(sampler_01, v_texcoord).rgb;
    vec3 diffuse_intensity = texture2D(sampler_02, v_texcoord).rgb;
    gl_FragColor = vec4(diffuse_color * diffuse_intensity, 1.0);
}

