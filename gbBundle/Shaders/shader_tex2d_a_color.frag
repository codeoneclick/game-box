uniform vec4 u_color;

void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    if (color.a == 0.0)
    {
        discard;
    }
    gl_FragColor = vec4(u_color.rgb, color.a);
}
