uniform int u_mask_flag_vs;

void main()
{
    vec4 position = vec4(a_position, 1.0);
    if(u_mask_flag_vs == 0)
    {
        gl_Position = get_mat_mvp() * position;
    }
    else
    {
        gl_Position = position;
    }
    
    v_position = a_position.xy;
    v_screen_position = gl_Position;
}
