void main()
{
    gl_FragColor = vec4(1.0, 1.0, 1.0, texture2D(sampler_01, v_texcoord).a * v_color.a);
}
