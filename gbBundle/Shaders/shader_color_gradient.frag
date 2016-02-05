#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform vec4 u_color_01;
uniform vec4 u_color_02;

void main()
{
    vec4 color = mix(u_color_01, u_color_02, v_texcoord.y);
    gl_FragColor = color;
}