
#if defined(OPENGL_30)

layout (location = 1) out vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

void main(void)
{
    v_texcoord = a_texcoord;
    gl_Position = vec4(a_position, 1.0);
}
