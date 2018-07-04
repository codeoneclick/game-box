#if defined(OPENGL_30)

layout (location = 1) in vec2 v_texcoord;

#else

varying vec2 v_texcoord;

#endif

uniform vec4 u_color_01;
uniform vec4 u_color_02;
uniform vec4 u_border_color;
uniform vec2 u_border_size;

void main()
{
    vec4 color = mix(u_color_02, u_color_01, v_texcoord.y);
    if(v_texcoord.x < u_border_size.x || v_texcoord.x > 1.0 - u_border_size.x ||
       v_texcoord.y < u_border_size.y || v_texcoord.y > 1.0 - u_border_size.y)
    {
        color = u_border_color;
    }
    gl_FragColor = color;
}