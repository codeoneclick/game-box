#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform sampler2D sampler_01;

uniform vec4 u_shadow_color;

void main()
{
    gl_FragColor = u_shadow_color;
}