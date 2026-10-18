// cos(yaw), sin(yaw)
uniform vec4 sky_rotation;
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

float field_hash(vec2 p){uvec2 q=uvec2(ivec2(p));uint h=q.x*73856093u^q.y*19349663u;h^=h>>13;h*=1274126177u;return float(h&65535u)/65535.0;}
float field_noise(vec2 p){vec2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);return mix(mix(field_hash(i),field_hash(i+vec2(1,0)),f.x),mix(field_hash(i+vec2(0,1)),field_hash(i+vec2(1,1)),f.x),f.y);}
float field_wave(vec2 p,vec4 w,vec4 m,vec2 seed){
 float t=time_level.x,local=field_noise(p*.037+seed);
 float period=4.5+4.0*field_noise(seed),clock=t+local*7.0+seed.x*.13;
 float age=clock-period*floor(clock/period);
 float onset=1.0-exp(-age*4.0);
 float pulse=onset*onset*exp(-age*.9)*(1.0-smoothstep(period-1.0,period,age));
 float energy=.18+2.0*pulse+.7*field_noise(p*.08+vec2(t*.17,-t*.12)+seed);
 float phase_noise=field_noise(p*.045+vec2(-t*.13,t*.11)+seed)-.5;
 float k=6.283185307/w.z;
 float phase=k*(dot(w.xy,p)-m.y*t)+2.4*phase_noise;
 float counter=k*(dot(vec2(-w.y,w.x),p)*.85+m.y*t*.73)+seed.x+phase_noise;
 float primary=sin(phase);
 return w.w*energy*(.8*(primary+.18*(primary*primary-.5))+.45*sin(counter));
}
vec3 water_scales(vec2 p){float t=time_level.x;vec2 drift=vec2(.031,-.019)*t;
 vec2 warp=vec2(field_noise(p*.027+drift),field_noise(vec2(-p.y,p.x)*.041-drift+vec2(19,7)))-.5;
 vec2 q=p+warp*8.0;
 return vec3(field_wave(q,wave0,motion0,vec2(0,5))+field_wave(q+vec2(7,19),wave3,motion3,vec2(7,19)),field_wave(q+warp*2.7,wave1,motion1,vec2(17,31)),field_wave(p+warp*.8,wave2,motion2,vec2(41,11)));}
float ocean_height(vec2 p){vec3 h=water_scales(p);float limit=max(.001,1.09*(wave0.w+wave1.w+wave2.w+wave3.w));return time_level.y+limit*tanh((h.x+h.y+h.z)/limit);}
float hash21(vec2 p) {return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}
float noise21(vec2 p) {vec2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
 return mix(mix(hash21(i),hash21(i+vec2(1,0)),f.x),mix(hash21(i+vec2(0,1)),hash21(i+vec2(1,1)),f.x),f.y);
}
float caustic_pattern(vec2 p) {
 float t=time_level.x*caustic_params.y;
 p+=vec2(sin(p.y*.9+t),cos(p.x*.8-t*.62))*.35;
 float a=sin(p.x*2.7+sin(p.y*1.9+t*.92)),b=sin(p.y*2.9+sin(p.x*2.1-t*1.08));
 return pow(clamp(1.0-abs(a+b)*1.7,0.0,1.0),5.0);
}

vec2 screen_uv(vec3 p){vec4 clip=projection*view*vec4(p,1);return vec2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);}
bool valid_uv(vec2 uv){return all(greaterThan(uv,vec2(.002)))&&all(lessThan(uv,vec2(.998)));}
vec4 interaction_sample(vec2 xz){vec2 uv=(xz-interaction.xy)/interaction.z;if(any(lessThan(uv,vec2(0)))||any(greaterThan(uv,vec2(1))))return vec4(320,0,0,0);return texture2D(sampler_04,uv);}
// Gradient Perlin noise perturbs the contours without adding texture samples.
vec2 foam_gradient(vec2 cell){
 float angle=6.2831853*field_hash(cell+vec2(101,47));
 return vec2(cos(angle),sin(angle));
}
float foam_perlin(vec2 p){
 vec2 cell=floor(p),f=fract(p);
 vec2 fade=f*f*f*(f*(f*6.0-15.0)+10.0);
 float a=dot(foam_gradient(cell),f);
 float b=dot(foam_gradient(cell+vec2(1,0)),f-vec2(1,0));
 float c=dot(foam_gradient(cell+vec2(0,1)),f-vec2(0,1));
 float d=dot(foam_gradient(cell+vec2(1,1)),f-vec2(1,1));
 return mix(mix(a,b,fade.x),mix(c,d,fade.x),fade.y);
}
// Independent births: large kernels shrink and dissolve, still merging/splitting.
float foam_metaballs(vec2 p,float t){
 vec2 cell=floor(p/3.0);float density=0.0;
 float contour=1.0+.22*foam_perlin(p*1.15+vec2(t*.13,-t*.09));
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){
  vec2 id=cell+vec2(x,y);
  float phase=6.2831853*field_hash(id+vec2(41,73));
  float rate=mix(.65,1.25,field_hash(id+vec2(19,11)));
  vec2 center=(id+.5)*3.0+vec2(sin(t*rate+phase),cos(t*rate*.83+phase*1.7))*.95;
  float lifetime=mix(3.6,6.2,field_hash(id+vec2(67,53)));
  float age=fract(t/lifetime+field_hash(id+vec2(13,89)));
  float shrink=1.0-smoothstep(.04,.92,age);
  float opacity=smoothstep(0.0,.035,age)*(1.0-smoothstep(.70,.98,age));
  float radius=mix(.83125,1.1305,field_hash(id+vec2(97,23)))*shrink*contour;
  vec2 delta=p-center;density+=opacity*radius*radius/(dot(delta,delta)+.35)*(1.0-smoothstep(2.5,3.4,length(delta)));
 }
 return density;
}
float foam_smooth_union(float a,float b,float radius){
 float h=clamp(.5+.5*(b-a)/radius,0.0,1.0);
 return mix(b,a,h)-radius*h*(1.0-h);
}
vec4 world_foam(vec3 p,out float width,out float influence){
 vec4 f=interaction_sample(p.xz);float d=f.x,t=time_level.x*foam_bands.w;
 // Keep the outer widths unchanged; only the contact band narrows by 25%.
 float base_width=max(.001,foam_bands.y)*1.33;
 width=base_width*.75;influence=1.0;
 if(d>=319.0||d>base_width*4.0+.5)return vec4(0);
 float second_width=base_width*.66,third_width=second_width*.33;
 vec2 shore=p.xz-f.yz*d;
 float blobs=foam_metaballs(p.xz,t),shape=smoothstep(.75,1.65,blobs);
 // World-anchored delays break up each front locally without a clock
 // rate that accumulates spatial discontinuities as the session runs.
 float delay2=field_noise(shore*.08)*2.8+field_noise(shore*.23+vec2(17,43))*.65;
 float delay3=field_noise(shore*.071+vec2(71,29))*2.8+field_noise(shore*.19+vec2(53,11))*.65;
 float phase2=fract(t/4.8+delay2),phase3=fract(t/5.7+delay3+.37);
 float approach2=smoothstep(0.0,1.0,phase2),approach3=smoothstep(0.0,1.0,phase3);
 // Fade out after absorption as well as in: wrapping either local clock
 // must never teleport a visible ribbon back out to sea.
 float birth2=smoothstep(0.0,.14,phase2)*(1.0-smoothstep(.84,1.0,phase2));
 float birth3=smoothstep(0.0,.17,phase3)*(1.0-smoothstep(.82,1.0,phase3));
 float gap=base_width*.38;
 // Two animated Perlin scales deform the fronts and tear holes in the
 // outer ribbons. Filter unresolved detail instead of introducing shimmer.
 float footprint=max(length(dFdx(p.xz)),length(dFdy(p.xz)));
 vec2 drift=vec2(t*.16,-t*.12);
 float broad_noise=foam_perlin(p.xz*.45+drift)*(1.0-smoothstep(.5,2.0,footprint));
 float detail_noise=foam_perlin(p.xz*1.6-drift*1.7+vec2(19,37))*(1.0-smoothstep(.18,.65,footprint));
 float bend=base_width*.5*(broad_noise+shape-.5);
 // Both outer fronts advance shoreward; their torn contours locally
 // stretch into metaball necks before being absorbed by the contact band.
 float center2=mix(base_width+gap+second_width*.5,width*.55,approach2)+bend*sin(phase2*3.14159265)+base_width*.16*detail_noise;
 float center3=mix(base_width+gap+second_width+gap+third_width*.5,width*.55,approach3)+bend*.7*sin(phase3*3.14159265)-base_width*.22*detail_noise;
 float inner_distance=d-width+width*(broad_noise*.16+detail_noise*.06);
 float second_distance=abs(d-center2)-second_width*.5+smoothstep(.12,.40,broad_noise+detail_noise*.65)*second_width*1.6;
 float third_distance=abs(d-center3)-third_width*.5+smoothstep(.03,.30,-broad_noise+detail_noise*.8)*third_width*2.2;
 // Remove dormant ribbons from the distance union too, so another
 // active front cannot reveal them through a shared visibility envelope.
 second_distance+=(1.0-birth2)*base_width*6.0;
 third_distance+=(1.0-birth3)*base_width*6.0;
 float amplitude=wave0.w+wave1.w+wave2.w+wave3.w;
 float crest=smoothstep(-.2,.65,(p.y-time_level.y)/max(amplitude,.001));
 vec2 direction=normalize(wave0.xy+wave1.xy*.35);
 float facing=dot(f.yz,direction);
 influence=mix(.72,1.32,crest)*mix(.7,1.3,facing*.5+.5);
 float bridge=base_width*(.18+.38*shape)*clamp(foam_shape.z,.2,1.5)*mix(.85,1.15,facing*.5+.5);
 float joined=foam_smooth_union(foam_smooth_union(inner_distance,second_distance,bridge),third_distance,bridge*.8);
 float aa=clamp(fwidth(d)*.6,.04,.3);
 float inner=1.0-smoothstep(-aa,aa,inner_distance);
 float second=1.0-smoothstep(-aa,aa,second_distance);
 float third=1.0-smoothstep(-aa,aa,third_distance);
 float coverage=1.0-smoothstep(-aa,aa,joined);
 coverage=max(inner,coverage*clamp(foam_detail.y,0.0,1.0));
 return vec4(inner,second,third,coverage)*(1.0-smoothstep(.25,.75,f.w));
}
// Low-frequency airlight only: never reveal cubemap objects through distant water.
vec3 island_horizon_color(vec3 ray,vec3 light){
 float day=smoothstep(-.12,.12,light.y);
 vec3 air=mix(vec3(.018,.035,.075),vec3(.12,.36,.58),day);
 return air+vec3(.09,.18,.24)*day*exp(-abs(ray.y)/.004);
}
// Sun-driven atmosphere shared by the visible sky and water reflection fallback.
float island_sky_hash(vec2 p){return fract(sin(dot(p,vec2(127.1,311.7)))*43758.5453);}
float island_sky_noise(vec2 p){vec2 i=floor(p),f=fract(p);f=f*f*(3.0-2.0*f);
 return mix(mix(island_sky_hash(i),island_sky_hash(i+vec2(1,0)),f.x),mix(island_sky_hash(i+vec2(0,1)),island_sky_hash(i+vec2(1,1)),f.x),f.y);}
vec3 island_sky_color(vec3 direction,vec3 light){
 vec3 d=normalize(direction);float height=clamp(d.y,0.0,1.0);
 float daylight=smoothstep(-.12,.12,light.y),sunset=1.0-smoothstep(.12,.65,light.y);
 vec3 horizon=mix(vec3(.38,.64,.78),vec3(.72,.18,.055),sunset);
 vec3 zenith=mix(vec3(.12,.32,.58),vec3(.19,.13,.29),sunset);
 vec3 color=mix(horizon,zenith,pow(height,.55));
 vec2 q=d.xz/max(d.y+.22,.18)*1.7;
 float clouds=island_sky_noise(q)+.35*island_sky_noise(q*2.3+vec2(17,31));
 float mask=smoothstep(.62,.86,clouds)*smoothstep(.01,.12,height)*(1.0-smoothstep(.65,.95,height));
 vec3 cloud_color=mix(vec3(.72,.79,.84),vec3(.24,.10,.19),sunset);
 cloud_color+=vec3(.85,.36,.08)*pow(max(dot(d,light),0.0),6.0);
 color=mix(color,cloud_color,mask*.85);
 float mu=max(dot(d,light),0.0);
 color+=vec3(1.0,.52,.16)*pow(mu,24.0)*sunset*.45;
 color+=vec3(4.0,2.5,.8)*smoothstep(.99955,.99975,mu)*smoothstep(-.03,.02,light.y);
 return mix(vec3(.008,.014,.035)+vec3(.012,.018,.025)*(1.0-height),color,daylight);
}
vec3 reflection_color(vec3 p,vec3 direction,float footprint){
 vec3 lookup=normalize(direction);
 lookup.xz=vec2(sky_rotation.x*lookup.x-sky_rotation.y*lookup.z,sky_rotation.y*lookup.x+sky_rotation.x*lookup.z);
 // GB_SAMPLER_02_CUBE: reflect the same environment as the visible skybox.
#if defined(USE_LAYOUTS)
 vec3 sky_sample=texture(sampler_02,lookup).rgb;
#else
 vec3 sky_sample=textureCube(sampler_02,lookup).rgb;
#endif
 vec3 sky=mix(sky_sample/12.92,pow((sky_sample+.055)/1.055,vec3(2.4)),step(vec3(.04045),sky_sample));
 float roughness=smoothstep(.25,1.25,footprint);
 // Pixel footprint filters detail; it must not erase the environment reflection.
 float air_blend=.9*smoothstep(motion0.z,motion0.z*3.125,max(abs(p.x),abs(p.z)));
 sky=mix(sky,island_horizon_color(normalize(direction),normalize(sun.xyz)),air_blend);
 // Distant rays cannot reach the island within this short SSR march.
 if(max(abs(p.x),abs(p.z))>motion0.w||roughness>.995)return sky;
 float previous=.2;
 for(int i=0;i<32;i++){
  float distance=.7+float(i)*1.1+float(i*i)*.09;
  vec3 q=p+direction*distance;vec4 clip=projection*view*vec4(q,1);if(clip.w<=0.0)break;
  vec2 uv=vec2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);if(!valid_uv(uv))break;
  vec4 scene=texture2D(sampler_05,uv);float delta=-(view*vec4(q,1)).z-scene.w;
  if(scene.w>0.0&&delta>0.0&&delta<2.5&&scene.y>time_level.y-.3){
   float lo=previous,hi=distance;
   for(int j=0;j<4;j++){
    float mid=(lo+hi)*.5;vec3 r=p+direction*mid;vec4 s=texture2D(sampler_05,screen_uv(r));
    if(s.w>0.0&&-(view*vec4(r,1)).z>s.w)hi=mid;else lo=mid;
   }
   uv=screen_uv(p+direction*hi);float edge=smoothstep(0.0,.06,min(min(uv.x,uv.y),min(1.0-uv.x,1.0-uv.y)));
   return mix(sky,texture2D(sampler_01,uv).rgb,edge*(1.0-roughness));
  }previous=distance;
 }return sky;
}
// Blend adjacent world-space ripple scales as the pixel footprint grows.
// Unresolved waves become a moving rough surface, never a flat mirror normal.
vec2 ocean_far_facet(vec2 q,float t){
 vec2 cell=floor(q),seed=vec2(0);float nearest=1.e10;
 // Jittered Voronoi facets have no rectangular tile boundary or repeated diagonal.
 for(int y=-1;y<=1;++y)for(int x=-1;x<=1;++x){
  vec2 id=cell+vec2(x,y);
  vec2 random=vec2(field_hash(id),field_hash(id+vec2(37,91)));
  vec2 delta=id+.15+random*.7-q;float distance2=dot(delta,delta);
  if(distance2<nearest){nearest=distance2;seed=random;}
 }
 return vec2(sin(t*.43+seed.x*6.2831853),cos(t*.37+seed.y*6.2831853))*.12;
}
vec2 ocean_distant_slope(vec2 p){
 float footprint=max(length(dFdx(p)),length(dFdy(p)));
 // Keep a facet roughly 8-16 pixels wide instead of letting it collapse to noise.
 float lod=clamp(log2(max(footprint*4.0,1.0)),0.0,18.0);
 vec2 q=p*(.25/exp2(floor(lod)));
 vec2 fine=ocean_far_facet(q,time_level.x);
 vec2 coarse=ocean_far_facet(q*.5,time_level.x);
 return mix(fine,coarse,smoothstep(0.0,1.0,fract(lod)));
}
void main(){
 vec3 p=v_screen_position.xyz;vec2 uv=vec2(.5+.5*v_position.x/v_position.w,.5-.5*v_position.y/v_position.w);
 vec4 scene=texture2D(sampler_05,uv);float depth=-(view*vec4(p,1)).z;
 vec3 n=normalize(cross(dFdx(p),dFdy(p)));if(n.y<0.0)n=-n;
 vec2 far_slope=ocean_distant_slope(p.xz);
 float footprint=max(length(dFdx(p.xz)),length(dFdy(p.xz)));
 // Replace under-resolved mesh facets before they shimmer, including inside the
 // detailed grid. The same gradual blend handles the horizon mesh boundary.
 float facet_blend=max(smoothstep(.25,1.0,footprint),smoothstep(motion0.z,motion0.w,max(abs(p.x),abs(p.z))));
 n=normalize(mix(n,normalize(vec3(-far_slope.x,1,-far_slope.y)),facet_blend));
 n=normalize(vec3(n.x*1.6,n.y,n.z*1.6));
 if(scene.w>0.0&&scene.w<depth-.015)discard;
 if(extra.x==5.0){attachment_01=vec4(vec3((p.y-time_level.y+.6)/1.2),1);return;}
 float thickness=scene.w>0.0?max(p.y-scene.y,0.0):25.0;
 vec3 v=normalize(camera.xyz-p),light=normalize(sun.xyz);
 vec2 refract_uv=uv;
 if(params.z>0.0){
  vec2 distortion=n.xz*params.z*smoothstep(0.0,optics.y,thickness)/max(.35,abs(v.y));
  refract_uv=clamp(uv+distortion,vec2(.002),vec2(.998));
  vec4 refracted_scene=texture2D(sampler_05,refract_uv);
  // The offset ray must not pull dry foreground geometry below the waterline.
  if(refracted_scene.w<=depth||refracted_scene.w<=0.0||refracted_scene.y>ocean_height(refracted_scene.xz))refract_uv=uv;
 }
 vec3 bottom=texture2D(sampler_01,refract_uv).rgb;float transmission=scene.w>0.0?exp(-thickness*params.y):0.0;
 vec3 body=mix(deep.rgb,shallow.rgb,exp(-thickness/optics.x)),color=mix(body,bottom*vec3(.5,.85,.93),transmission*.6);
 float water_light=(dot(n,light)-light.y)*foam_shadow.w;
 float band_width=.015;
 vec3 water_tone=mix(vec3(.65,.75,.90),vec3(.88,.94,1.0),smoothstep(-.12-band_width,-.12+band_width,water_light));
 water_tone=mix(water_tone,vec3(1.06,1.02,.94),smoothstep(.12-band_width,.12+band_width,water_light));
 color*=water_tone;
 // Non-repeating world pixels, averaged out once smaller than a screen pixel.
 vec2 water_pixel=floor(p.xz/2.0);
 float pixel_visibility=1.0-smoothstep(.5,1.5,footprint);
 color*=1.0+(field_hash(water_pixel)-.5)*.16*pixel_visibility;
 float fresnel=optics.w>0.0?clamp((.025+optics.w*pow(1.0-max(dot(n,v),0.0),optics.z)),0.0,1.0):0.0;
 fresnel=mix(fresnel,min(fresnel,.22),smoothstep(motion0.z,motion0.z*3.125,max(abs(p.x),abs(p.z))));
 if(extra.x!=3.0&&fresnel>0.0)color=mix(color,reflection_color(p+n*.05,reflect(-v,n),footprint),fresnel);
 // Compact cel-style sun glints, independent of wave crest foam.
 vec3 half_light=normalize(v+light);
 vec3 sun_tint=mix(vec3(1,.94,.78),vec3(1,.55,.16),1.0-smoothstep(.12,.65,light.y));
 float specular=dot(n,half_light),spec_width=.003;
 if(extra.x==0.0){
  color+=sun_tint*pow(max(specular,0.0),48.0)*params.w*.7;
  float glint_width=clamp(fwidth(specular)*1.5,.001,.004);
  color+=sun_tint*.8*smoothstep(.998-glint_width,.998+glint_width,specular)*params.w;
 }
 if(extra.x==2.0)color=body;
 float plane=clamp((dot(n,light)-light.y)*3.0+.5,0.0,1.0);plane=floor(plane*6.0)/6.0;
 float crest_highlight=smoothstep(.25,.7,(p.y-time_level.y)/max(wave0.w+wave1.w+wave2.w+wave3.w,.001));
 color+=vec3(.08,.29,.3)*plane*crest_highlight*params.w;
 color*=.97+.06*field_noise(p.xz*.039);

 float depth_difference=scene.w>0.0?max(scene.w-depth,0.0):1000.0;
 // Base-water foam values affect only these diagnostic views; visible foam has its own pass.
 float width=0.0,influence=0.0;vec4 layers=vec4(0),field=vec4(0);
 if(extra.x==1.0||extra.x==7.0||extra.x==8.0||extra.x==18.0||extra.x==19.0||extra.x==20.0)
  layers=world_foam(p,width,influence);
 if(extra.x==15.0||extra.x==16.0||extra.x==17.0)field=interaction_sample(p.xz);
 if(extra.x==1.0)color=vec3(layers.w);if(extra.x==4.0)color=vec3(clamp(thickness/16.0,0.0,1.0));
 if(extra.x==6.0)color=vec3(clamp(depth_difference/foam_bands.y,0.0,1.0));
 if(extra.x==7.0)color=vec3(layers.x);if(extra.x==8.0)color=vec3(layers.z);
 if(extra.x==9.0)color=n*.5+.5;if(extra.x==10.0)color=vec3(fresnel);if(extra.x==11.0)color=vec3(refract_uv,0);
 if(extra.x==12.0)color=vec3(scene.w>0.0?caustic_pattern(scene.xz*caustic_params.x)*exp(-thickness*caustic_params.w)*caustic_params.z:0.0);
 if(extra.x==13.0)color=vec3(.5+water_scales(p.xz).x);if(extra.x==14.0)color=vec3(.5+water_scales(p.xz).y*2.0);
 if(extra.x==15.0)color=vec3(field.w);if(extra.x==16.0)color=vec3(clamp(field.x/8.0,0.0,1.0));
 if(extra.x==17.0)color=vec3(field.yz*.5+.5,0);if(extra.x==18.0)color=vec3(layers.y);
 if(extra.x==19.0)color=vec3(width/2.0);if(extra.x==20.0)color=vec3(influence*.5);
 attachment_01=vec4(color,1);
}
