#if defined(OPENGL_30)

layout (location = 1) in vec2 v_texcoord;
layout (location = 2) in vec4 v_color;

#else
 
varying highp vec2 v_texcoord;
varying vec4 v_color;

#endif

uniform sampler2D sampler_01;

void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    if (color.r == 0.0)
    {
        discard;
    }
    gl_FragColor = vec4(v_color.rgb, color.r * v_color.a);
}
