void main(){
 vec4 world=get_mat_m()*vec4(a_position,1.0);
 gl_Position=get_mat_p()*get_mat_v()*world;v_position=gl_Position;
 v_screen_position=world;v_texcoord=a_texcoord;
 v_normal=normalize(mat3(get_mat_m())*a_normal.xyz);
}
