const float k_texcoord_scale = 8.0;

void main()
{
    v_texcoord = a_texcoord;
    v_texcoord_scaled = v_texcoord * k_texcoord_scale;
    gl_Position = vec4(a_position, 1.0);
}
