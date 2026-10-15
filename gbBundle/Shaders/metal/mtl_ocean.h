struct ocean_uniforms {
 float4 wave0,wave1,wave2,wave3,motion0,motion1,motion2,motion3,time_level,camera,deep,shallow,foam,params,sun,extra,foam_bands,foam_detail,foam_shadow,optics,caustic_params,interaction,foam_shape;
 float4x4 view,projection;
 float4 light_sun,light_ambient,light_shape,light_options,light_material0,light_material1,light_camera;
};
// Settings append to the existing custom buffer; mode 0 retains cel lighting.
float3 island_soft_light(float3 base,float3 n,float3 l,float3 v,float strength,float vegetation,float4 light_sun,float4 light_ambient,float4 light_shape,float4 light_options,float4 light_material1){
 float wrapped=clamp((dot(n,l)+light_shape.x)/(1.0+light_shape.x),0.0,1.0);
 float diffuse=pow(smoothstep(0.0,1.0,wrapped),light_shape.y);
 float3 sunlight=light_sun.rgb*light_sun.a;
 float3 color=base*(light_ambient.rgb*light_ambient.a+sunlight*diffuse);
 float3 half_direction=l+v;
 float3 h=half_direction/max(length(half_direction),.0001);
 float specular=pow(max(dot(n,h),0.0),light_material1.z)*max(dot(n,l),0.0);
 color+=sunlight*specular*strength*light_shape.w*light_options.y;
 float back=pow(max(dot(-n,l),0.0),2.0)*vegetation*light_options.x*light_options.z;
 return color+base*sunlight*back;
}

constexpr sampler ocean_nearest(coord::normalized,address::clamp_to_edge,filter::nearest);
float3 cel_light(float diffuse){
 float width=.01;
 float3 tone=mix(float3(.32,.38,.50),float3(.62,.66,.72),smoothstep(.22-width,.22+width,diffuse));
 return mix(tone,float3(1.02,.96,.83),smoothstep(.65-width,.65+width,diffuse));
}

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
 float extent=max(abs(rest.x),abs(rest.y));
 if(extent<720.0)p.y=mix(u.time_level.y,ocean_ocean_height(rest,u),1.0-smoothstep(384.0,720.0,extent));
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
struct ocean_field_debug {float4 field_debug;float4x4 view,projection;float4 camera,time_level,sun;};
// Normal alpha stores exact object IDs, shared with the GLSL silhouette pass.
float scene_edge(int2 pixel,float2 fraction,float2 footprint,float4 p,texture2d<float> normals,constant ocean_field_debug& u){
 int2 size=int2(normals.get_width(),normals.get_height());
 float object_id=normals.read(uint2(pixel)).w;
 if(u.sun.w<.5||p.w<=0.0||object_id<2.0||u.time_level.w!=0.0)return 0.0;
 float edge=0.0;
 for(int i=0;i<4;i++){
  int2 offset=i==0?int2(1,0):i==1?int2(-1,0):i==2?int2(0,1):int2(0,-1);
  int2 adjacent=pixel+offset;
  if(any(adjacent<int2(0))||any(adjacent>=size))continue;
  float adjacent_id=normals.read(uint2(adjacent)).w;
  if(adjacent_id>0.0&&adjacent_id<1.0)continue;
  if(adjacent_id!=object_id){
   float distance=i==0?1.0-fraction.x:i==1?fraction.x:i==2?1.0-fraction.y:fraction.y;
   float pixel_footprint=i<2?footprint.x:footprint.y;
   // Half a screen pixel on each side, analytically antialiased over a fragment.
   const float half_width=.5;
   float coverage=clamp(half_width+.5-distance/pixel_footprint,0.0,1.0);
   edge=max(edge,coverage);
  }
 }
 return edge*(object_id<=27.0?.18:object_id>=8000000.0?.55:.35);
}
float3 island_sky_color(float3 direction,float3 light);
// Screen-space data is suitable for contact only, not sparse long cast shadows.
float island_sun_shadow(float4 geometry,float2 uv,float water_alpha,constant ocean_field_debug& u,texture2d<float> position_tex,texture2d<float> normal_tex){
 if(u.field_debug.w<.5||geometry.w<=0.0||water_alpha>.5||u.sun.y<=.03)return 0.0;
 int2 size=int2(position_tex.get_width(),position_tex.get_height()),pixel=clamp(int2(uv*float2(size)),int2(0),size-int2(1));
 float4 receiver=normal_tex.read(uint2(pixel));
 if(receiver.w<1.0)return 0.0; // Cutout grass must not become a solid receiver.
 float3 normal=normalize(receiver.xyz*2.0-1.0),light=normalize(u.sun.xyz);
 if(dot(normal,light)<0.0)return 0.0;
 float3 origin=geometry.xyz+normal*.025;
 float shadow=0.0;
 // Adjacent samples overlap the thin depth interval: no separated silhouette copies.
 for(int i=0;i<32;i++){
  float step_distance=(float(i)+1.0)*.046875; // At most 1.5 metres.
  float3 q=origin+light*step_distance;float4 clip=u.projection*u.view*float4(q,1);
  if(clip.w<=0.0)break;
  float2 sample_uv=float2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);
  if(any(sample_uv<float2(.002))||any(sample_uv>float2(.998)))break;
  int2 at=clamp(int2(sample_uv*float2(size)),int2(0),size-int2(1));
  if(all(at==pixel))continue;
  float4 scene=position_tex.read(uint2(at));float id=normal_tex.read(uint2(at)).w;
  float delta=-(u.view*float4(q,1)).z-scene.w;
  // Depth alone treats unrelated foreground as an occluder. Also require a
  // nearby world-space surface above the receiver's tangent plane.
  if(scene.w>0.0&&id>=1.0&&delta>.005&&delta<.09&&
     length(scene.xyz-q)<.12&&dot(scene.xyz-geometry.xyz,normal)>.04){
   float border=smoothstep(0.0,.04,min(min(sample_uv.x,sample_uv.y),min(1.0-sample_uv.x,1.0-sample_uv.y)));
   float contact=(1.0-smoothstep(.8,1.5,step_distance))*(1.0-smoothstep(.04,.09,delta));
   shadow=max(shadow,border*contact);
  }
 }return shadow*.45*smoothstep(.03,.12,u.sun.y);
}
float3 atmosphere_scatter(float3 color,float3 ray,float distance,constant ocean_field_debug& u){
 // Horizon scattering is independent of the optional post-process distance fog.
 if(u.time_level.z<.5||u.time_level.w!=0.0||distance<=450.0)return color;
 float end=min(distance,6000.0),step_length=(end-450.0)/8.0;
 float mu=dot(ray,normalize(u.sun.xyz)),g=.65;
 float forward=(1.0-g*g)/pow(max(1.0+g*g-2.0*g*mu,.08),1.5);
 float3 light=island_sky_color(ray,normalize(u.sun.xyz));
 float3 scatter=float3(0);float transmission=1.0;
 for(int i=0;i<8;i++){
  float3 p=u.camera.xyz+ray*(450.0+(float(i)+.5)*step_length);
  float clouds=.65+.25*sin(p.x*.008+sin(p.z*.006+u.time_level.x*.025))*cos(p.y*.017-p.z*.003)
                  +.1*sin(dot(p,float3(-.005,.013,.009))-u.time_level.x*.018);
  float density=exp(-max(p.y-u.time_level.y,0.0)/120.0)*clouds;
  float opacity=1.0-exp(-density*.0065*step_length);
  scatter+=transmission*opacity*light*(.95+.1*clouds);
  transmission*=1.0-opacity;
 }
 return color*transmission+scatter;
}
fragment half4 fragment_shader_ocean_compose(common_v_output_t in [[stage_in]],texture2d<float> scene [[texture(0)]],texture2d<float> water [[texture(1)]],texture2d<float> foam_tex [[texture(2)]],texture2d<float> field_tex [[texture(3)]],texture2d<float> mask_tex [[texture(4)]],texture2d<float> ao_tex [[texture(5)]],constant ocean_field_debug& debug [[buffer(1)]],texture2d<float> position_tex [[texture(6)]],texture2d<float> normal_tex [[texture(7)]]){
 if(debug.field_debug.x>0.0){float4 field=field_tex.sample(ocean_nearest,in.texcoord);float3 c=float3(clamp(field.x/8.0,0.0,1.0));if(debug.field_debug.x==15.0){float2 sides=mask_tex.sample(ocean_nearest,in.texcoord).rg;c=float3(min(sides.r,sides.g));}if(debug.field_debug.x==17.0)c=float3(field.yz*.5+.5,0);return half4(half3(c),0);}
 float ao=ao_tex.sample(linear_sampler,in.texcoord).r;
 if(debug.field_debug.y>0.0)return half4(half3(ao),0);
 float4 w=water.sample(linear_sampler,in.texcoord),f=foam_tex.sample(linear_sampler,in.texcoord);
 float4 geometry=position_tex.sample(ocean_nearest,in.texcoord);
 float shade=island_sun_shadow(geometry,in.texcoord,w.a,debug,position_tex,normal_tex);
 float3 opaque=scene.sample(linear_sampler,in.texcoord).rgb*ao*mix(float3(1),float3(.55,.64,.78),shade*.9);
 float3 color=mix(opaque,w.rgb,w.a)*(1.0-f.a)+f.rgb;
 float2 ndc=float2(in.texcoord.x*2.0-1.0,1.0-in.texcoord.y*2.0);
 float3x3 rotation=float3x3(debug.view[0].xyz,debug.view[1].xyz,debug.view[2].xyz);
 float3 ray=normalize(transpose(rotation)*float3((ndc+float2(debug.projection[2][0],debug.projection[2][1]))/float2(debug.projection[0][0],debug.projection[1][1]),-1));
 float distance=0.0;
 int2 outline_size=int2(normal_tex.get_width(),normal_tex.get_height());
 float2 outline_texel=in.texcoord*float2(outline_size);
 float2 outline_footprint=max(abs(dfdx(outline_texel))+abs(dfdy(outline_texel)),float2(.0001));
 float edge=0.0;
 if(debug.sun.w>.5&&geometry.w>0.0&&w.a<.5&&debug.time_level.w==0.0)
 {
  int2 pixel=clamp(int2(outline_texel),int2(0),outline_size-int2(1));
  edge=scene_edge(pixel,fract(outline_texel),outline_footprint,geometry,normal_tex,debug)*(1.0-smoothstep(.1,.5,w.a));
 }

 if(w.a>.5&&ray.y<-.00001)distance=max(0.0,(debug.time_level.y-debug.camera.y)/ray.y);
 else if(geometry.w>0.0){distance=length(geometry.xyz-debug.camera.xyz);ray=normalize(geometry.xyz-debug.camera.xyz);}
 return half4(half3(atmosphere_scatter(color,ray,distance,debug)),half(edge));
}
float2 ocean_screen_uv(float3 p,constant ocean_uniforms& u){float4 clip=u.projection*u.view*float4(p,1);return float2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);}
bool ocean_valid_uv(float2 uv){return all(uv>float2(.002))&&all(uv<float2(.998));}
float4 interaction_sample(float2 xz,constant ocean_uniforms& u,texture2d<float> field_tex){float2 uv=(xz-u.interaction.xy)/u.interaction.z;if(any(uv<float2(0))||any(uv>float2(1)))return float4(320,0,0,0);return field_tex.sample(linear_sampler,uv);}
// Gradient Perlin noise perturbs the contours without adding texture samples.
float2 foam_gradient(float2 cell){
 float angle=6.2831853*field_hash(cell+float2(101,47));
 return float2(cos(angle),sin(angle));
}
float foam_perlin(float2 p){
 float2 cell=floor(p),f=fract(p);
 float2 fade=f*f*f*(f*(f*6.0-15.0)+10.0);
 float a=dot(foam_gradient(cell),f);
 float b=dot(foam_gradient(cell+float2(1,0)),f-float2(1,0));
 float c=dot(foam_gradient(cell+float2(0,1)),f-float2(0,1));
 float d=dot(foam_gradient(cell+float2(1,1)),f-float2(1,1));
 return mix(mix(a,b,fade.x),mix(c,d,fade.x),fade.y);
}
// Independent births: large kernels shrink and dissolve, still merging/splitting.
float foam_metaballs(float2 p,float t){
 float2 cell=floor(p/3.0);float density=0.0;
 float contour=1.0+.22*foam_perlin(p*1.15+float2(t*.13,-t*.09));
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){
  float2 id=cell+float2(x,y);
  float phase=6.2831853*field_hash(id+float2(41,73));
  float rate=mix(.65,1.25,field_hash(id+float2(19,11)));
  float2 center=(id+.5)*3.0+float2(sin(t*rate+phase),cos(t*rate*.83+phase*1.7))*.95;
  float lifetime=mix(3.6,6.2,field_hash(id+float2(67,53)));
  float age=fract(t/lifetime+field_hash(id+float2(13,89)));
  float shrink=1.0-smoothstep(.04,.92,age);
  float opacity=smoothstep(0.0,.035,age)*(1.0-smoothstep(.70,.98,age));
  float radius=mix(.83125,1.1305,field_hash(id+float2(97,23)))*shrink*contour;
  float2 delta=p-center;density+=opacity*radius*radius/(dot(delta,delta)+.35)*(1.0-smoothstep(2.5,3.4,length(delta)));
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
// Sun-driven atmosphere shared by the visible sky and water reflection fallback.
float island_sky_hash(float2 p){return fract(sin(dot(p,float2(127.1,311.7)))*43758.5453);}
float island_sky_noise(float2 p){float2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
 return mix(mix(island_sky_hash(i),island_sky_hash(i+float2(1,0)),f.x),mix(island_sky_hash(i+float2(0,1)),island_sky_hash(i+float2(1,1)),f.x),f.y);}
float3 island_sky_color(float3 direction,float3 light){
 float3 d=normalize(direction);float height=clamp(d.y,0.0,1.0);
 float daylight=smoothstep(-.12,.12,light.y),sunset=1.0-smoothstep(.12,.65,light.y);
 float3 horizon=mix(float3(.38,.64,.78),float3(.72,.18,.055),sunset);
 float3 zenith=mix(float3(.12,.32,.58),float3(.19,.13,.29),sunset);
 float3 color=mix(horizon,zenith,pow(height,.55));
 float2 q=d.xz/max(d.y+.22,.18)*1.7;
 float clouds=island_sky_noise(q)+.35*island_sky_noise(q*2.3+float2(17,31));
 float mask=smoothstep(.62,.86,clouds)*smoothstep(.01,.12,height)*(1.0-smoothstep(.65,.95,height));
 float3 cloud_color=mix(float3(.72,.79,.84),float3(.24,.10,.19),sunset);
 cloud_color+=float3(.85,.36,.08)*pow(max(dot(d,light),0.0),6.0);
 color=mix(color,cloud_color,mask*.85);
 float mu=max(dot(d,light),0.0);
 color+=float3(1.0,.52,.16)*pow(mu,24.0)*sunset*.45;
 color+=float3(4.0,2.5,.8)*smoothstep(.99955,.99975,mu)*smoothstep(-.03,.02,light.y);
 return mix(float3(.008,.014,.035)+float3(.012,.018,.025)*(1.0-height),color,daylight);
}
float3 ocean_reflection_color(float3 p,float3 direction,constant ocean_uniforms& u,texture2d<float> color_tex,texture2d<float> scene_tex){
 float3 sky=island_sky_color(direction,normalize(u.sun.xyz));float previous=.2;
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
 n=normalize(mix(n,float3(0,1,0),smoothstep(384.0,720.0,max(abs(p.x),abs(p.z)))));
 float3 v=normalize(u.camera.xyz-p),light=normalize(u.sun.xyz);
 float2 distortion=n.xz*u.params.z*smoothstep(0.0,u.optics.y,thickness)/max(.35,abs(v.y)),refract_uv=clamp(uv+distortion,float2(.002),float2(.998));
 float4 refracted_scene=scene_tex.sample(ocean_nearest,refract_uv);
 // The offset ray must not pull dry foreground geometry below the waterline.
 if(refracted_scene.w<=depth||refracted_scene.w<=0.0||refracted_scene.y>ocean_ocean_height(refracted_scene.xz,u))refract_uv=uv;
 float3 bottom=color_tex.sample(linear_sampler,refract_uv).rgb;float transmission=scene.w>0.0?exp(-thickness*u.params.y):0.0;
 float3 body=mix(u.deep.rgb,u.shallow.rgb,exp(-thickness/u.optics.x)),color=mix(body,bottom*float3(.5,.85,.93),transmission*.6);
 float water_light=(dot(n,light)-light.y)*u.foam_shadow.w;
 float band_width=.015;
 float3 water_tone=mix(float3(.65,.75,.90),float3(.88,.94,1.0),smoothstep(-.12-band_width,-.12+band_width,water_light));
 water_tone=mix(water_tone,float3(1.06,1.02,.94),smoothstep(.12-band_width,.12+band_width,water_light));
 color*=water_tone;
 float fresnel=.025+u.optics.w*pow(1.0-max(dot(n,v),0.0),u.optics.z);
 if(u.extra.x!=3.0)color=mix(color,ocean_reflection_color(p+n*.05,reflect(-v,n),u,color_tex,scene_tex),fresnel);
 float3 half_light=normalize(v+light);
 float3 sun_tint=mix(float3(1,.94,.78),float3(1,.55,.16),1.0-smoothstep(.12,.65,light.y));
 float specular=dot(n,half_light),spec_width=.003;
 if(u.extra.x==0.0){
  color+=sun_tint*pow(max(specular,0.0),96.0)*u.params.w*.7;
  color+=sun_tint*3.0*smoothstep(.9985,.9995,specular)*u.params.w;
 }
 if(u.extra.x==2.0)color=body;
 float plane=clamp((dot(n,light)-light.y)*3.0+.5,0.0,1.0);plane=floor(plane*6.0)/6.0;
 float crest_highlight=smoothstep(.25,.7,(p.y-u.time_level.y)/max(u.wave0.w+u.wave1.w+u.wave2.w+u.wave3.w,.001));
 color+=float3(.08,.29,.3)*plane*crest_highlight*u.params.w;
 color*=.97+.06*field_noise(p.xz*.039);

 float depth_difference=scene.w>0.0?max(scene.w-depth,0.0):1000.0;
 // Base-water foam values affect only these diagnostic views; visible foam has its own pass.
 float width=0.0,influence=0.0;float4 layers=float4(0),field=float4(0);
 if(u.extra.x==1.0||u.extra.x==7.0||u.extra.x==8.0||u.extra.x==18.0||u.extra.x==19.0||u.extra.x==20.0)
  layers=world_foam(p,width,influence,u,field_tex);
 if(u.extra.x==15.0||u.extra.x==16.0||u.extra.x==17.0)field=interaction_sample(p.xz,u,field_tex);
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
 float3 color=mix(u.foam_shadow.rgb,u.foam.rgb,.8+.2*shade)*1.24;float coverage=layers.w;
 return half4(half3(color*coverage),half(coverage));
}
