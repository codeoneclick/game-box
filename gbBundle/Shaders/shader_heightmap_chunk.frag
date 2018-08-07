void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    gl_FragColor = color;
}
