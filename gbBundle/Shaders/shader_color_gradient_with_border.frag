#if defined(__OPENGL_30__)

in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform vec4 u_color_01;
uniform vec4 u_color_02;
uniform vec4 u_border_color;
uniform vec4 u_border;

void main()
{
    vec4 color = mix(u_color_01, u_color_02, v_texcoord.y);
    if(v_texcoord.x < u_border.x || v_texcoord.x > u_border.z ||
       v_texcoord.y < u_border.y || v_texcoord.y > u_border.w)
    {
        color = u_border_color;
    }
    gl_FragColor = color;
}