uniform vec4 wave0;
uniform vec4 wave1;
uniform vec4 wave2;
uniform vec4 wave3;
uniform vec4 motion0;
uniform vec4 motion1;
uniform vec4 motion2;
uniform vec4 motion3;
uniform vec4 time_level;
uniform vec4 camera;
uniform vec4 deep;
uniform vec4 shallow;
uniform vec4 foam;
uniform vec4 params;
uniform vec4 sun;
uniform vec4 extra;
uniform vec4 foam_bands;
uniform vec4 foam_detail;
uniform vec4 foam_shadow;
uniform vec4 optics;
uniform vec4 caustic_params;
uniform vec4 interaction;
uniform vec4 foam_shape;
uniform mat4 view;
uniform mat4 projection;
#if defined(VULKAN_API)
layout(binding=3) uniform sampler2D sampler_01;
#else
uniform sampler2D sampler_01;
#endif
// Tangent.z marks grass; UV holds one shared root for all blade vertices.
vec2 grass_wind(vec2 p,float t,float phase){
 float gust=.3+.25*(.5+.5*sin(dot(p,vec2(.063,.041))-t*.71))
                 +.2*(.5+.5*sin(dot(p,vec2(-.037,.079))+t*.43));
 vec2 broad=vec2(sin(t*.83+dot(p,vec2(.091,-.053))),cos(t*.67+dot(p,vec2(.047,.083))));
 vec2 local=vec2(sin(t*(1.61+.17*sin(phase))+phase),cos(t*(1.37+.19*cos(phase))+phase*1.73));
 vec2 wind=(broad*.72+local*.28)*gust;
 return wind*min(1.0,.7/max(length(wind),.0001));
}
void main(){
 vec3 position=a_position,normal=a_normal.xyz;
 if(a_tangent.z>.5){
  float weight=a_tangent.x*a_tangent.x;
  vec2 wind=grass_wind(a_texcoord,time_level.x,a_tangent.y*91.7);
  int layer=clamp(int(floor((position.y-a_tangent.x*a_tangent.y)/2.0+.5)),1,4)-1;
  vec2 tile=vec2(layer%2,layer/2);
  vec2 uv=clamp((a_texcoord-interaction.xy)/interaction.z,vec2(.001),vec2(.999));
  vec2 atlas_uv=(tile+uv)*.5;
#if !defined(VULKAN_API)
  atlas_uv.y=1.0-atlas_uv.y;
#endif
#if defined(USE_LAYOUTS)
  vec2 push=textureLod(sampler_01,atlas_uv,0.0).xy;
#else
  vec2 push=texture2DLod(sampler_01,atlas_uv,0.0).xy;
#endif
  float force=length(push);push*=min(1.0,.85/max(force,.0001));
  wind=mix(wind,wind*.2+push,smoothstep(0.0,.4,force));
  wind*=min(1.0,1.0/max(length(wind),.0001));
  wind*=a_tangent.y*.8;
  position.xz+=wind*weight;
  position.y-=dot(wind,wind)*.18*weight;
  normal=normalize(normal-vec3(wind.x,0,wind.y)*weight*.45);
 }
 else if(a_tangent.w>.5){
  float weight=a_tangent.x*a_tangent.x;
  vec2 wind=grass_wind(a_texcoord,time_level.x*.4,a_tangent.y*91.7)*.4;
  // Every vertex at the same height shares the tree's wind field, including
  // subdivided neighbors of a greedy face. Rest-pose root UVs prevent cracks.
  wind+=.02*vec2(sin(time_level.x*1.37+a_tangent.y*31.0+a_texcoord.x*.17),cos(time_level.x*1.12+a_tangent.y*47.0+a_texcoord.y*.19));
  position.xz+=wind*weight;
  normal=normalize(normal-vec3(wind.x,0,wind.y)*weight*.12);
 }
 gl_Position=get_mat_mvp()*vec4(position,1);v_position=gl_Position;
 v_screen_position=get_mat_m()*vec4(position,1);
 v_normal=normalize((get_mat_n()*vec4(normal,0)).xyz);v_color=a_color;
 // Decode a constant object ID from unused packed vertex channels.
 // Keep all RGB color, XYZ normal, bend weight/phase and root UVs unchanged.
 float outline_id=floor(a_color.a*255.0+.5)*16384.0+floor(a_normal.w*127.0+.5)*128.0+floor(-a_tangent.z*127.0+.5);
 v_color.a=a_tangent.z>.5?.5:(a_normal.w>0.0||a_tangent.z<0.0||a_color.a<.9999?outline_id:1.0);
 v_texcoord=a_texcoord;
 if(a_tangent.z>.5){
  // Blade-local rest-pose metres, after the shared root UV has sampled wind/push.
  // Explicit side bit avoids recovering it from half-precision root coordinates.
  v_texcoord=vec2(a_color.a,a_tangent.x);
 }
 if(a_tangent.w>.5){
  // Project the rest pose after wind sampling: bark/leaf pixels move with the mesh.
  vec3 rest=a_position;rest.xz-=a_texcoord;
  v_texcoord=abs(a_normal.y)>.5?rest.xz:(abs(a_normal.x)>abs(a_normal.z)?rest.zy:rest.xy);
 }
 v_tangent=vec3(a_tangent.w<-.5?1.0:0.0,a_tangent.w>.5?1.0:0.0,fract(sin(dot(a_texcoord,vec2(127.1,311.7)))*43758.5453));
}
