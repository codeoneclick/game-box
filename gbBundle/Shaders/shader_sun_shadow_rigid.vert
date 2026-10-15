uniform mat4 shadow_mvp;
uniform vec4 shadow_cast;
vec2 grass_wind(vec2 p,float t,float phase){
 float gust=.3+.25*(.5+.5*sin(dot(p,vec2(.063,.041))-t*.71))
                 +.2*(.5+.5*sin(dot(p,vec2(-.037,.079))+t*.43));
 vec2 broad=vec2(sin(t*.83+dot(p,vec2(.091,-.053))),cos(t*.67+dot(p,vec2(.047,.083))));
 vec2 local=vec2(sin(t*(1.61+.17*sin(phase))+phase),cos(t*(1.37+.19*cos(phase))+phase*1.73));
 vec2 wind=(broad*.72+local*.28)*gust;
 return wind*min(1.0,.7/max(length(wind),.0001));
}
void main(){
 vec3 position=a_position;
 if(a_tangent.w>.5){
  float weight=a_tangent.x*a_tangent.x;
  vec2 wind=grass_wind(a_texcoord,shadow_cast.x*.4,a_tangent.y*91.7)*.4;
  wind+=.02*vec2(sin(shadow_cast.x*1.37+a_tangent.y*31.0+a_texcoord.x*.17),cos(shadow_cast.x*1.12+a_tangent.y*47.0+a_texcoord.y*.19));
  position.xz+=wind*weight;
 }
 gl_Position=shadow_mvp*get_mat_m()*vec4(position,1);
#if defined(VULKAN_API)
 gl_Position.z=gl_Position.z*.5+gl_Position.w*.5;
#endif
 v_texcoord=a_texcoord;v_tangent=vec3(a_tangent.z>.5?1.0:0.0,shadow_cast.y,0);
}
