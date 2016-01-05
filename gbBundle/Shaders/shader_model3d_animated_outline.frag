
uniform vec3 u_outline_color;

void main(void)
{
    gl_FragColor = vec4(u_outline_color, 1.0);
}