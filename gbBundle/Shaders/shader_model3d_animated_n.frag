
#if defined(__OPENGL_30__)

in vec3 v_normal;

#else

varying vec3 v_normal;

#endif

void main(void)
{
    gl_FragColor = vec4(v_normal * 0.5 + 0.5, 1.0);
}
