#if defined(OPENGL_30)

layout (location = 1) in vec2 v_texcoord;
layout (location = 2) in vec4 v_color;

#else

varying vec2 v_texcoord;
varying vec4 v_color;

#endif

uniform sampler2D sampler_01;

void main(void)
{
    gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(sampler_01, v_texcoord).a * v_color.a);
}
