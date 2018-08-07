void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    if (color.a == 0.0)
    {
        discard;
    }
    color.a *= v_color.a;
    gl_FragColor = color;
}
