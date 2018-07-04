#if defined(OPENGL_30)

layout (location = 1) in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform sampler2D sampler_01;

void main(void)
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    if (color.a < 1.0)
    {
        discard;
    }
    gl_FragColor = color;
}
