void main()
{
    vec3 light_position = vec3(0.0, 16.0, 0.0);
    vec3 light_direction = (light_position - v_screen_position.xyz);
    vec4 color = vec4(1.0, 1.0, 1.0, 1.0) * max(dot(normalize(v_normal), normalize(light_direction)), 0.0);
    color.a = 1.0;
    gl_FragColor = color;
}
