void main(){
 // The existing XZ quad covers the screen; the scene position texture projects
 // the ring across terrace tops/walls without relying on a flat hover plane.
 mat4 model=get_mat_m();
 gl_Position=vec4(a_position.x,a_position.z,0.0,1.0);
 v_position=gl_Position;
 v_screen_position=model*vec4(0.0,0.0,0.0,1.0);
 v_texcoord=vec2(length(model[0].xyz),0.0);
}
