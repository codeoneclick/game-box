#if defined(OPENGL_30)

layout (location = 1) in vec2 v_position;
layout (location = 2) in vec2 v_texcoord;
layout (location = 3) in vec4 v_color;

#else

varying vec2 v_position;
varying vec2 v_texcoord;
varying vec4 v_color;

#endif

uniform sampler2D sampler_01;

void main(void)
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    if (color.a == 0.0)
    {
        discard;
    }
    color.a *= v_color.a;
    gl_FragColor = color;
}
