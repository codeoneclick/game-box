void main()
{
    gl_FragColor = vec4(normalize(v_normal) * 0.5 + 0.5, 1.0);
}
