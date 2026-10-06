void main()
{
    gl_Position = get_pos_mvp();
    v_screen_position = get_mat_m() * vec4(a_position, 1.0);
    v_normal = normalize((get_mat_n() * vec4(a_normal.xyz, 0.0)).xyz);
    v_color = a_color;
}
