#ifndef GB_SUN_SHADOW_METAL
#define GB_SUN_SHADOW_METAL
struct sun_caster_values {float4x4 shadow_mvp;float4 shadow_cast;};
struct sun_skin_values {float4x4 bones[64];sun_caster_values caster;};
struct sun_resolve_values {float4x4 shadow_near,shadow_far;float4 shadow_sun,shadow_params;};
float2 sun_grass_wind(float2 p,float t,float phase){
 float gust=.3+.25*(.5+.5*sin(dot(p,float2(.063,.041))-t*.71))
                 +.2*(.5+.5*sin(dot(p,float2(-.037,.079))+t*.43));
 float2 broad=float2(sin(t*.83+dot(p,float2(.091,-.053))),cos(t*.67+dot(p,float2(.047,.083))));
 float2 local=float2(sin(t*(1.61+.17*sin(phase))+phase),cos(t*(1.37+.19*cos(phase))+phase*1.73));
 float2 wind=(broad*.72+local*.28)*gust;
 return wind*min(1.0,.7/max(length(wind),.0001));
}
vertex common_v_output_t vertex_shader_sun_shadow_rigid(common_v_input_t in [[stage_in]],constant common_u_input_t& m [[buffer(1)]],constant sun_caster_values& u [[buffer(3)]]){
 common_v_output_t out={};float3 position=in.position;
 if(in.tangent.w>.5){
  float weight=in.tangent.x*in.tangent.x;
  float2 wind=sun_grass_wind(in.texcoord,u.shadow_cast.x*.4,in.tangent.y*91.7)*.4;
  wind+=.02*float2(sin(u.shadow_cast.x*1.37+in.tangent.y*31.0+in.texcoord.x*.17),cos(u.shadow_cast.x*1.12+in.tangent.y*47.0+in.texcoord.y*.19));
  position.xz+=wind*weight;
 }
 out.position=u.shadow_mvp*m.mat_m*float4(position,1);out.position.z=out.position.z*.5+out.position.w*.5;
 out.texcoord=in.texcoord;out.tangent=float3(in.tangent.z>.5?1.0:0.0,u.shadow_cast.y,0);return out;
}
vertex common_v_output_t vertex_shader_sun_shadow_skin(resident_v_input in [[stage_in]],constant common_u_input_t& m [[buffer(1)]],constant sun_skin_values& b [[buffer(2)]]){
 common_v_output_t out={};float4 weights=in.weights/max(dot(in.weights,float4(1)),.00001),skinned=float4(0);
 for(int i=0;i<4;++i)skinned+=b.bones[clamp(int(in.bone_ids[i]),0,63)]*float4(in.position,1)*weights[i];
 out.position=b.caster.shadow_mvp*m.mat_m*skinned;out.position.z=out.position.z*.5+out.position.w*.5;out.texcoord=in.texcoord;return out;
}
float4 sun_depth_fragment(common_v_output_t in,texture2d<float> image){
 if(in.tangent.x>.5)discard_fragment();
 if(in.tangent.y>.5&&image.sample(ocean_nearest,in.texcoord).a<.1)discard_fragment();
 float3 depth=fract(min(in.position.z,.999999)*float3(1,255,65025));depth.xy-=depth.yz/255.0;return float4(depth,1);
}
fragment float4 fragment_shader_sun_shadow_rigid(common_v_output_t in [[stage_in]],texture2d<float> image [[texture(0)]]){return sun_depth_fragment(in,image);}
fragment float4 fragment_shader_sun_shadow_skin(common_v_output_t in [[stage_in]],texture2d<float> image [[texture(0)]]){return sun_depth_fragment(in,image);}
vertex common_v_output_t vertex_shader_sun_shadow_resolve(common_v_input_t in [[stage_in]]){common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;}
float sun_shadow_depth(float3 rgb){return dot(rgb,float3(1,1.0/255.0,1.0/65025.0));}
float sun_shadow_pcf(int cascade,float3 world,float3 normal,constant sun_resolve_values& u,texture2d<float> near_tex,texture2d<float> far_tex){
 float texel=cascade==0?u.shadow_params.x:u.shadow_params.y;
 float4x4 matrix=cascade==0?u.shadow_near:u.shadow_far;
 float3 p=(matrix*float4(world+normal*texel*.3,1)).xyz;
 float2 uv=float2(.5+.5*p.x,.5-.5*p.y);
 if(any(uv<float2(.002))||any(uv>float2(.998))||abs(p.z)>=1.0)return 0.0;
 float bias=(.012+texel*.5*(1.0-max(dot(normal,u.shadow_sun.xyz),0.0)))/u.shadow_params.z;
 float depth=p.z*.5+.5-bias;
 int2 size=cascade==0?int2(near_tex.get_width(),near_tex.get_height()):int2(far_tex.get_width(),far_tex.get_height());
 float3 right=normalize(float3(matrix[0][0],matrix[1][0],matrix[2][0]));
 float3 up=normalize(float3(matrix[0][1],matrix[1][1],matrix[2][1]));
 float facing=dot(normal,u.shadow_sun.xyz);if(facing<=.025)return 0.0;
 float world_width=2.0/length(float3(matrix[0][0],matrix[1][0],matrix[2][0]));
 float2 slope=float2(dot(normal,right),-dot(normal,up))*world_width/(facing*u.shadow_params.z);
 float2 coord=uv*float2(size)-.5,f=fract(coord);int2 base=int2(floor(coord));float result=0.0;
 for(int y=-1;y<=2;++y)for(int x=-1;x<=2;++x){
  int2 at=clamp(base+int2(x,y),int2(0),size-int2(1));
  float3 rgb=cascade==0?near_tex.read(uint2(at)).rgb:far_tex.read(uint2(at)).rgb;
  float weight=max(0.0,2.0-abs(float(x)-f.x))*max(0.0,2.0-abs(float(y)-f.y));
  float receiver_depth=depth+dot(slope,(float2(at)+.5)/float2(size)-uv);
  result+=(receiver_depth>sun_shadow_depth(rgb)?1.0:0.0)*weight;
 }return result/16.0;
}
fragment half4 fragment_shader_sun_shadow_resolve(common_v_output_t in [[stage_in]],texture2d<float> scene [[texture(0)]],texture2d<float> positions [[texture(1)]],texture2d<float> normals [[texture(2)]],texture2d<float> near_tex [[texture(3)]],texture2d<float> far_tex [[texture(4)]],texture2d<float> water_tex [[texture(5)]],constant sun_resolve_values& u [[buffer(1)]]){
 float4 color=scene.sample(ocean_nearest,in.texcoord),geometry=positions.sample(ocean_nearest,in.texcoord),normal=normals.sample(ocean_nearest,in.texcoord);
 float water=water_tex.sample(ocean_nearest,in.texcoord).a;
 if(u.shadow_params.w==2.0||u.shadow_params.w==3.0){float3 d=u.shadow_params.w==2.0?near_tex.sample(ocean_nearest,in.texcoord).rgb:far_tex.sample(ocean_nearest,in.texcoord).rgb;return half4(half3(sun_shadow_depth(d)),0);}
 if(u.shadow_sun.w<.5||geometry.w<=0.0||normal.w<=0.0||normal.w==9000000.0||water>.99)return half4(color);
 // Match GLSL: derive the receiver plane before the low-sun slope correction.
 float3 n=normalize(normal.xyz*2.0-1.0),plane=cross(dfdx(geometry.xyz),dfdy(geometry.xyz));
 float plane_length=length(plane);
 if(plane_length>1.e-8){plane/=plane_length;plane*=dot(plane,n)<0.0?-1.0:1.0;if(dot(plane,n)>.9)n=plane;}
 float3 near_clip=(u.shadow_near*float4(geometry.xyz,1)).xyz;
 float blend=smoothstep(.72,.94,max(abs(near_clip.x),abs(near_clip.y)));
 float shade=blend<1.0?sun_shadow_pcf(0,geometry.xyz,n,u,near_tex,far_tex):0.0;
 if(blend>0.0)shade=mix(shade,sun_shadow_pcf(1,geometry.xyz,n,u,near_tex,far_tex),blend);
 shade*=smoothstep(.015,.10,u.shadow_sun.y)*(1.0-water);
 if(u.shadow_params.w==1.0)return half4(half3(1.0-shade),0);
 return half4(half3(color.rgb*mix(float3(1),float3(.38,.47,.61),shade*.85)),half(color.a));
}
#endif
