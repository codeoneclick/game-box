#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec4 v_color;

#else

varying vec2 v_texcoord;
varying vec4 v_color;

#endif

uniform sampler2D sampler_01;

void main(void)
{
    gl_FragColor = vec4(v_color.rgb, texture2D(sampler_01, v_texcoord).a);
}
