void main()
{
    vec4 color = texture2D(sampler_01, v_texcoord);
    color.a = 1.0;
    if (color.a < 1.0)
    {
        discard;
    }
    //vec3 light_position = vec3(0.0, 128.0, 0.0);
    //vec3 light_direction = (light_position - v_screen_position.xyz);
    //color = color * max(dot(normalize(v_normal), normalize(light_direction)), 0.0);
    //color.a = 1.0;
    gl_FragColor = color;
}
