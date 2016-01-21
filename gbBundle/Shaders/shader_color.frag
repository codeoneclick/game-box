#if defined(__OPENGL_30__)

#else

#endif

uniform vec4 u_color;

void main(void)
{
    gl_FragColor = u_color;
}