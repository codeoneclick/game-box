uniform vec4 u_color_01;
uniform vec4 u_color_02;

void main()
{
    vec4 color = mix(u_color_02, u_color_01, v_texcoord.y);
    gl_FragColor = color;
}