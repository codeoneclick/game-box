#if defined (OPENGL_30)

layout (location = 1) in vec4 v_color;

#else

varying vec4 v_color;

#endif

void main()
{
    gl_FragColor = v_color;
}
