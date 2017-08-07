#if defined(__OPENGL_30__)

in highp vec2 v_texcoord;
in vec4 v_color;

#else
 
varying highp vec2 v_texcoord;
varying vec4 v_color;

#endif

uniform sampler2D sampler_01;

void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    gl_FragColor = vec4(v_color.rgb, color.r * v_color.a);
}
