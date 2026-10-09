struct ocean_uniforms {
 float4 wave0,wave1,wave2,wave3,motion0,motion1,motion2,motion3,time_level,camera,deep,shallow,foam,params,sun,extra,foam_bands,foam_detail,foam_shadow,optics,caustic_params,interaction,foam_shape;
 float4x4 view,projection;
};
constexpr sampler ocean_nearest(coord::normalized,address::clamp_to_edge,filter::nearest);

float field_hash(float2 p){uint2 q=uint2(int2(p));uint h=q.x*73856093u^q.y*19349663u;h^=h>>13;h*=1274126177u;return float(h&65535u)/65535.0;}
float field_noise(float2 p){float2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);return mix(mix(field_hash(i),field_hash(i+float2(1,0)),f.x),mix(field_hash(i+float2(0,1)),field_hash(i+float2(1,1)),f.x),f.y);}
float field_wave(float2 p,float4 w,float4 m,float2 seed,constant ocean_uniforms& u){
 float t=u.time_level.x,local=field_noise(p*.037+seed);
 float period=4.5+4.0*field_noise(seed),clock=t+local*7.0+seed.x*.13;
 float age=clock-period*floor(clock/period);
 float onset=1.0-exp(-age*4.0);
 float pulse=onset*onset*exp(-age*.9)*(1.0-smoothstep(period-1.0,period,age));
 float energy=.18+2.0*pulse+.7*field_noise(p*.08+float2(t*.17,-t*.12)+seed);
 float phase_noise=field_noise(p*.045+float2(-t*.13,t*.11)+seed)-.5;
 float k=6.283185307/w.z;
 float phase=k*(dot(w.xy,p)-m.y*t)+2.4*phase_noise;
 float counter=k*(dot(float2(-w.y,w.x),p)*.85+m.y*t*.73)+seed.x+phase_noise;
 float primary=sin(phase);
 return w.w*energy*(.8*(primary+.18*(primary*primary-.5))+.45*sin(counter));
}
float3 water_scales(float2 p,constant ocean_uniforms& u){float t=u.time_level.x;float2 drift=float2(.031,-.019)*t;
 float2 warp=float2(field_noise(p*.027+drift),field_noise(float2(-p.y,p.x)*.041-drift+float2(19,7)))-.5;
 float2 q=p+warp*8.0;
 return float3(field_wave(q,u.wave0,u.motion0,float2(0,5),u)+field_wave(q+float2(7,19),u.wave3,u.motion3,float2(7,19),u),field_wave(q+warp*2.7,u.wave1,u.motion1,float2(17,31),u),field_wave(p+warp*.8,u.wave2,u.motion2,float2(41,11),u));}
float ocean_ocean_height(float2 p,constant ocean_uniforms& u){float3 h=water_scales(p,u);float limit=max(.001,1.09*(u.wave0.w+u.wave1.w+u.wave2.w+u.wave3.w));return u.time_level.y+limit*tanh((h.x+h.y+h.z)/limit);}
float ocean_hash21(float2 p) {return fract(sin(dot(p,float2(127.1,311.7)))*43758.5453);}
float ocean_noise21(float2 p) {float2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
 return mix(mix(ocean_hash21(i),ocean_hash21(i+float2(1,0)),f.x),mix(ocean_hash21(i+float2(0,1)),ocean_hash21(i+float2(1,1)),f.x),f.y);
}
float ocean_caustic_pattern(float2 p,constant ocean_uniforms& u) {
 float t=u.time_level.x*u.caustic_params.y;
 p+=float2(sin(p.y*.9+t),cos(p.x*.8-t*.62))*.35;
 float a=sin(p.x*2.7+sin(p.y*1.9+t*.92)),b=sin(p.y*2.9+sin(p.x*2.1-t*1.08));
 return pow(clamp(1.0-abs(a+b)*1.7,0.0,1.0),5.0);
}


vertex common_v_output_t vertex_shader_ocean(common_v_input_t in [[stage_in]],constant common_u_input_t& mvp [[buffer(1)]],constant ocean_uniforms& u [[buffer(2)]]){
 common_v_output_t out={};
 float2 rest=(get_mat_m(mvp)*float4(in.position,1)).xz;
 float3 p=float3(rest.x,u.time_level.y,rest.y),tx=float3(1,0,0),tz=float3(0,0,1);
 p.y=ocean_ocean_height(rest,u);
 out.position=get_mat_p(mvp)*get_mat_v(mvp)*float4(p,1);out.view_space_position=float4(p,1);
 out.normal=normalize(cross(tz,tx));return out;
}
vertex common_v_output_t vertex_shader_ocean_foam(common_v_input_t in [[stage_in]],constant common_u_input_t& mvp [[buffer(1)]],constant ocean_uniforms& u [[buffer(2)]]){
 common_v_output_t out={};
 float2 rest=(get_mat_m(mvp)*float4(in.position,1)).xz;
 float3 p=float3(rest.x,u.time_level.y,rest.y),tx=float3(1,0,0),tz=float3(0,0,1);
 p.y=ocean_ocean_height(rest,u)+u.foam_shape.w;
 out.position=get_mat_p(mvp)*get_mat_v(mvp)*float4(p,1);out.view_space_position=float4(p,1);
 out.normal=normalize(cross(tz,tx));return out;
}
vertex common_v_output_t vertex_shader_ocean_compose(common_v_input_t in [[stage_in]]){
 common_v_output_t out={};out.position=float4(in.position,1);out.texcoord=in.texcoord;return out;
}
struct ocean_field_debug {float4 field_debug;};
fragment half4 fragment_shader_ocean_compose(common_v_output_t in [[stage_in]],texture2d<float> scene [[texture(0)]],texture2d<float> water [[texture(1)]],texture2d<float> foam_tex [[texture(2)]],texture2d<float> field_tex [[texture(3)]],texture2d<float> mask_tex [[texture(4)]],constant ocean_field_debug& debug [[buffer(0)]]){
 if(debug.field_debug.x>0.0){float4 field=field_tex.sample(ocean_nearest,in.texcoord);float3 c=float3(clamp(field.x/8.0,0.0,1.0));if(debug.field_debug.x==15.0){float2 sides=mask_tex.sample(ocean_nearest,in.texcoord).rg;c=float3(min(sides.r,sides.g));}if(debug.field_debug.x==17.0)c=float3(field.yz*.5+.5,0);return half4(half3(c),1);}
 float4 w=water.sample(linear_sampler,in.texcoord),f=foam_tex.sample(linear_sampler,in.texcoord);return half4(half3(mix(scene.sample(linear_sampler,in.texcoord).rgb,w.rgb,w.a)*(1.0-f.a)+f.rgb),1);
}
float2 ocean_screen_uv(float3 p,constant ocean_uniforms& u){float4 clip=u.projection*u.view*float4(p,1);return float2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);}
bool ocean_valid_uv(float2 uv){return all(uv>float2(.002))&&all(uv<float2(.998));}
float4 interaction_sample(float2 xz,constant ocean_uniforms& u,texture2d<float> field_tex){float2 uv=(xz-u.interaction.xy)/u.interaction.z;if(any(uv<float2(0))||any(uv>float2(1)))return float4(320,0,0,0);return field_tex.sample(linear_sampler,uv);}
// Summed moving kernels: the isocontour merges/splits rather than scrolling a texture.
float foam_metaballs(float2 p,float t){
 float2 cell=floor(p/3.0);float density=0.0;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){
  float2 id=cell+float2(x,y);
  float phase=6.2831853*field_hash(id+float2(41,73));
  float rate=mix(.65,1.25,field_hash(id+float2(19,11)));
  float2 center=(id+.5)*3.0+float2(sin(t*rate+phase),cos(t*rate*.83+phase*1.7))*.95;
  float radius=mix(.65,1.05,field_hash(id+float2(97,23)));
  float2 delta=p-center;density+=radius*radius/(dot(delta,delta)+.35)*(1.0-smoothstep(2.5,3.4,length(delta)));
 }
 return density;
}
float4 world_foam(float3 p,thread float& width,thread float& influence,constant ocean_uniforms& u,texture2d<float> field_tex){
 float4 f=interaction_sample(p.xz,u,field_tex);float d=f.x,t=u.time_level.x*u.foam_bands.w;
 float2 r=float2(p.x*.7986-p.z*.6018,p.x*.6018+p.z*.7986);
 float large=field_noise(r*.137+float2(.21,-.17)*t),medium=field_noise(float2(-p.z,p.x)*.263+float2(-.19,.31)*t+float2(17,31));
 float blobs=foam_metaballs(p.xz,t);
 float shape=smoothstep(.75,1.65,blobs);
 float width_noise=field_noise(p.xz*.18+float2(.15,-.11)*t);
 float amplitude=u.wave0.w+u.wave1.w+u.wave2.w+u.wave3.w;
 float crest=smoothstep(-.2,.65,(p.y-u.time_level.y)/max(amplitude,.001));
 float2 dir=normalize(u.wave0.xy+u.wave1.xy*.35+float2(large-.5,medium-.5)*.7);
 float facing=dot(f.yz,dir);influence=mix(.72,1.32,crest)*mix(.7,1.3,facing*.5+.5);
 width=mix(u.foam_shape.x,u.foam_shape.y,width_noise*.45+shape*.55)*influence;
 // Keep a solid contact core at raster-field precision; animate only its outer edge.
 float contact=u.foam_bands.x*mix(1.0,2.0,shape);
 float inner=1.0-smoothstep(.2,contact+.25,d);
 float mid=(1.0-smoothstep(contact*.5,width+contact,d))*smoothstep(.65,1.45,blobs)*(1.0-inner);
 float2 warp=(float2(field_noise(p.xz*.083+float2(t*.13,-t*.09)),field_noise(r*.061+float2(-t*.11,t*.17)+float2(9,21)))-.5)*u.foam_shape.z*smoothstep(contact,width+contact,d);
 float outer_d=interaction_sample(p.xz+warp,u,field_tex).x;
 float islands=smoothstep(.85,1.7,blobs)*smoothstep(.35,.65,large);
 float outer=smoothstep(width*.7,width+.7,d)*(1.0-smoothstep(width+1.2,width+4.2,outer_d))*islands*influence;
 return float4(inner,mid,outer,smoothstep(.18,.34,clamp(inner+mid*u.foam_detail.y+outer,0.0,1.0)))*(1.0-smoothstep(.25,.75,f.w));
}
float3 ocean_reflection_color(float3 p,float3 direction,constant ocean_uniforms& u,texture2d<float> color_tex,texture2d<float> scene_tex){
 float3 sky=mix(float3(.075,.2,.42),float3(.32,.64,.8),clamp(direction.y,0.0,1.0));float previous=.2;
 for(int i=0;i<32;i++){
  float distance=.7+float(i)*1.1+float(i*i)*.09;
  float3 q=p+direction*distance;float4 clip=u.projection*u.view*float4(q,1);if(clip.w<=0.0)break;
  float2 uv=float2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);if(!ocean_valid_uv(uv))break;
  float4 scene=scene_tex.sample(ocean_nearest,uv);float delta=-(u.view*float4(q,1)).z-scene.w;
  if(scene.w>0.0&&delta>0.0&&delta<2.5&&scene.y>u.time_level.y-.3){
   float lo=previous,hi=distance;
   for(int j=0;j<4;j++){
    float mid=(lo+hi)*.5;float3 r=p+direction*mid;float4 s=scene_tex.sample(ocean_nearest,ocean_screen_uv(r,u));
    if(s.w>0.0&&-(u.view*float4(r,1)).z>s.w)hi=mid;else lo=mid;
   }
   uv=ocean_screen_uv(p+direction*hi,u);float edge=smoothstep(0.0,.06,min(min(uv.x,uv.y),min(1.0-uv.x,1.0-uv.y)));
   return mix(sky,color_tex.sample(linear_sampler,uv).rgb,edge);
  }previous=distance;
 }return sky;
}
fragment half4 fragment_shader_ocean(common_v_output_t in [[stage_in]],constant ocean_uniforms& u [[buffer(0)]],texture2d<float> color_tex [[texture(0)]],texture2d<float> scene_tex [[texture(1)]],texture2d<float> normal_tex [[texture(2)]],texture2d<float> field_tex [[texture(3)]]){
 float3 p=in.view_space_position.xyz;float2 uv=in.position.xy/float2(scene_tex.get_width(),scene_tex.get_height());
 float4 scene=scene_tex.sample(ocean_nearest,uv);float depth=-(u.view*float4(p,1)).z;
 if(scene.w>0.0&&scene.w<depth-.015)discard_fragment();
 if(u.extra.x==5.0){return half4(half3((p.y-u.time_level.y+.6)/1.2),1);}
 float thickness=scene.w>0.0?max(p.y-scene.y,0.0):25.0;
 float3 n=normalize(cross(dfdx(p),dfdy(p)));if(n.y<0.0)n=-n;
 float3 v=normalize(u.camera.xyz-p),light=normalize(u.sun.xyz);
 float2 distortion=n.xz*u.params.z*smoothstep(0.0,u.optics.y,thickness)/max(.35,abs(v.y)),refract_uv=clamp(uv+distortion,float2(.002),float2(.998));
 float4 refracted_scene=scene_tex.sample(ocean_nearest,refract_uv);
 // The offset ray must not pull dry foreground geometry below the waterline.
 if(refracted_scene.w<=depth||refracted_scene.w<=0.0||refracted_scene.y>ocean_ocean_height(refracted_scene.xz,u))refract_uv=uv;
 float3 bottom=color_tex.sample(linear_sampler,refract_uv).rgb;float transmission=scene.w>0.0?exp(-thickness*u.params.y):0.0;
 float3 body=mix(u.deep.rgb,u.shallow.rgb,exp(-thickness/u.optics.x)),color=mix(body,bottom*float3(.5,.85,.93),transmission*.6);
 color*=clamp(1.0+(dot(n,light)-light.y)*u.foam_shadow.w,.58,1.3);
 float fresnel=.025+u.optics.w*pow(1.0-max(dot(n,v),0.0),u.optics.z);
 if(u.extra.x!=3.0)color=mix(color,ocean_reflection_color(p+n*.05,reflect(-v,n),u,color_tex,scene_tex),fresnel);
 if(u.extra.x==2.0)color=body;
 float plane=clamp((dot(n,light)-light.y)*3.0+.5,0.0,1.0);plane=floor(plane*6.0)/6.0;
 float crest_highlight=smoothstep(.25,.7,(p.y-u.time_level.y)/max(u.wave0.w+u.wave1.w+u.wave2.w+u.wave3.w,.001));
 color+=float3(.08,.29,.3)*plane*crest_highlight*u.params.w;
 color*=.97+.06*field_noise(p.xz*.039);

 float depth_difference=scene.w>0.0?max(scene.w-depth,0.0):1000.0;
 float width,influence;float4 layers=world_foam(p,width,influence,u,field_tex);float4 field=interaction_sample(p.xz,u,field_tex);
 if(u.extra.x==1.0)color=float3(layers.w);if(u.extra.x==4.0)color=float3(clamp(thickness/16.0,0.0,1.0));
 if(u.extra.x==6.0)color=float3(clamp(depth_difference/u.foam_bands.y,0.0,1.0));
 if(u.extra.x==7.0)color=float3(layers.x);if(u.extra.x==8.0)color=float3(layers.z);
 if(u.extra.x==9.0)color=n*.5+.5;if(u.extra.x==10.0)color=float3(fresnel);if(u.extra.x==11.0)color=float3(refract_uv,0);
 if(u.extra.x==12.0)color=float3(scene.w>0.0?ocean_caustic_pattern(scene.xz*u.caustic_params.x,u)*exp(-thickness*u.caustic_params.w)*u.caustic_params.z:0.0);
 if(u.extra.x==13.0)color=float3(.5+water_scales(p.xz,u).x);if(u.extra.x==14.0)color=float3(.5+water_scales(p.xz,u).y*2.0);
 if(u.extra.x==15.0)color=float3(field.w);if(u.extra.x==16.0)color=float3(clamp(field.x/8.0,0.0,1.0));
 if(u.extra.x==17.0)color=float3(field.yz*.5+.5,0);if(u.extra.x==18.0)color=float3(layers.y);
 if(u.extra.x==19.0)color=float3(width/2.0);if(u.extra.x==20.0)color=float3(influence*.5);
 return half4(half3(color),1);
}

fragment half4 fragment_shader_ocean_foam(common_v_output_t in [[stage_in]],constant ocean_uniforms& u [[buffer(0)]],texture2d<float> scene_tex [[texture(1)]],texture2d<float> field_tex [[texture(3)]]){
 float3 p=in.view_space_position.xyz;float2 uv=in.position.xy/float2(scene_tex.get_width(),scene_tex.get_height());
 if(u.extra.x!=0.0){return half4(0);}
 float4 scene=scene_tex.sample(ocean_nearest,uv);float depth=-(u.view*float4(p,1)).z;
 if(scene.w>0.0&&scene.w<depth-.015)discard_fragment();
 float3 n=normalize(cross(dfdx(p),dfdy(p)));if(n.y<0.0)n=-n;
 float width,influence;float4 layers=world_foam(p-float3(0,u.foam_shape.w,0),width,influence,u,field_tex);
 float shade=floor(max(dot(n,normalize(u.sun.xyz)),0.0)*5.0)/5.0;
 float3 color=mix(u.foam_shadow.rgb,u.foam.rgb,.8+.2*shade);float coverage=layers.w;
 return half4(half3(color*coverage),half(coverage));
}
