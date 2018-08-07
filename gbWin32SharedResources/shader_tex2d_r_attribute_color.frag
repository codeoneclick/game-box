void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    if (color.r == 0.0)
    {
        discard;
    }
    gl_FragColor = vec4(v_color.rgb, color.r * v_color.a);
}
