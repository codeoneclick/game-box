#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform vec4 u_color;

void main()
{
    gl_FragColor = u_color;
}