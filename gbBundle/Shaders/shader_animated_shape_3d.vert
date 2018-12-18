uniform mat4 u_bones[64];

void main()
{
    vec4 position = vec4(a_position, 1.0);
    vec4 bones_weights = a_color;
    
    int index = 0;
    vec4 bone_position = vec4(0.0, 0.0, 0.0, 0.0);
    
    for(int i = 0; i < 4; i++)
    {
        index = int(a_extra[i]);
        bone_position += u_bones[index] * position * bones_weights[i];
    }
    bone_position = get_mat_m() * bone_position;
    position = get_mat_p() * get_mat_v() * bone_position;
    gl_Position = position;
    gl_ClipDistance[0] = dot(bone_position.xyz, vec3(0.0, 1.0, 0.0));
    v_texcoord = a_texcoord;
}
