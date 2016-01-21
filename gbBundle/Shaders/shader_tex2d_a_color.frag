#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform sampler2D sampler_01;
uniform vec4 u_color;

void main(void)
{
    gl_FragColor = vec4(u_color.rgb, texture2D(sampler_01, v_texcoord).a);
}