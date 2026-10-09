uniform mat4 u_bones[64];
void main(){
 vec4 weights=a_color/max(dot(a_color,vec4(1)),.00001);
 vec4 skinned=vec4(0);
 for(int i=0;i<4;i++)skinned+=u_bones[clamp(int(a_extra[i]),0,63)]*vec4(a_position,1)*weights[i];
 vec4 world=get_mat_m()*skinned;
 gl_Position=get_mat_p()*get_mat_v()*world;v_position=gl_Position;
 v_screen_position=world;v_texcoord=a_texcoord;
}
