void main(){
 gl_Position=get_mat_mvp()*vec4(a_position,1);
 v_position=gl_Position;v_screen_position=get_mat_m()*vec4(a_position,1);
 v_texcoord=a_texcoord;v_color=a_color;
}
