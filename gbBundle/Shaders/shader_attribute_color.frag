#if defined(__OPENGL_30__)

in vec2 v_texcoord;
in vec4 v_color;

#else

varying vec2 v_texcoord;
varying vec4 v_color;

#endif

void main()
{
    gl_FragColor = v_color;
}
