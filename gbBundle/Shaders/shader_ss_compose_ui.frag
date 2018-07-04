
#if defined(OPENGL_30)

layout (location = 1) in vec2 v_texcoord;

layout (binding = 0) uniform sampler2D sampler_01;
layout (binding = 1) uniform sampler2D sampler_02;

#else

varying vec2 v_texcoord;

uniform sampler2D sampler_01;
uniform sampler2D sampler_02;

#endif


void main()
{
    vec4 scene_color = texture2D(sampler_01, v_texcoord);
    vec4 ui_color = texture2D(sampler_02, v_texcoord);
    gl_FragColor = mix(scene_color, ui_color, ui_color.a);
}

