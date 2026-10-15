uniform mat4 u_bones[64];
uniform mat4 shadow_mvp;
uniform vec4 shadow_cast;
void main(){
 vec4 weights=a_color/max(dot(a_color,vec4(1)),.00001),skinned=vec4(0);
 for(int i=0;i<4;i++)skinned+=u_bones[clamp(int(a_extra[i]),0,63)]*vec4(a_position,1)*weights[i];
 gl_Position=shadow_mvp*get_mat_m()*skinned;
#if defined(VULKAN_API)
 gl_Position.z=gl_Position.z*.5+gl_Position.w*.5;
#endif
 v_texcoord=a_texcoord;v_tangent=vec3(0);
}
