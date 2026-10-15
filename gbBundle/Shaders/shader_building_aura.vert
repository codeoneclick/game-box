void main(){
 vec4 world=get_mat_m()*vec4(a_position.x,0.0,a_position.y,1.0);
 gl_Position=get_mat_p()*get_mat_v()*world;
 v_texcoord=a_texcoord;
}
