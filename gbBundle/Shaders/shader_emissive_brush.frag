#if defined(__OPENGL_30__)

in vec2 v_position;

#else

varying vec2 v_position;

#endif

uniform float u_radius;
uniform vec2 u_center;

void main(void)
{
    gl_FragColor = vec4(1.0 - length(v_position - u_center) / u_radius);
}