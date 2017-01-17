
#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;

void main()
{
    vec4 scene_color = texture2D(sampler_01, v_texcoord);
    vec4 ui_color = texture2D(sampler_02, v_texcoord);
    gl_FragColor = mix(scene_color, ui_color, ui_color.a);
}

