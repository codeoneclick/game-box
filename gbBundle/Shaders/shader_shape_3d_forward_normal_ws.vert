void main()
{
    gl_Position = get_pos_mvp();
    v_normal = (get_mat_m() * a_normal).xyz;
}
