void main()
{
    gl_Position = get_pos_mvp();
    v_screen_position = get_mat_m() * vec4(a_position, 1.0);
    v_normal = (get_mat_m() * a_normal).xyz;
    v_color = a_color;
}
