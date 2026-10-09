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
// Summed moving kernels: the isocontour merges/splits rather than scrolling a texture.
float foam_metaballs(vec2 p,float t){
 vec2 cell=floor(p/3.0);float density=0.0;
 for(int y=-1;y<=1;y++)for(int x=-1;x<=1;x++){
  vec2 id=cell+vec2(x,y);
  float phase=6.2831853*field_hash(id+vec2(41,73));
  float rate=mix(.65,1.25,field_hash(id+vec2(19,11)));
  vec2 center=(id+.5)*3.0+vec2(sin(t*rate+phase),cos(t*rate*.83+phase*1.7))*.95;
  float radius=mix(.65,1.05,field_hash(id+vec2(97,23)));
  vec2 delta=p-center;density+=radius*radius/(dot(delta,delta)+.35)*(1.0-smoothstep(2.5,3.4,length(delta)));
 }
 return density;
}
vec4 world_foam(vec3 p,out float width,out float influence){
 vec4 f=interaction_sample(p.xz);float d=f.x,t=time_level.x*foam_bands.w;
 vec2 r=vec2(p.x*.7986-p.z*.6018,p.x*.6018+p.z*.7986);
 float large=field_noise(r*.137+vec2(.21,-.17)*t),medium=field_noise(vec2(-p.z,p.x)*.263+vec2(-.19,.31)*t+vec2(17,31));
 float blobs=foam_metaballs(p.xz,t);
 float shape=smoothstep(.75,1.65,blobs);
 float width_noise=field_noise(p.xz*.18+vec2(.15,-.11)*t);
 float amplitude=wave0.w+wave1.w+wave2.w+wave3.w;
 float crest=smoothstep(-.2,.65,(p.y-time_level.y)/max(amplitude,.001));
 vec2 dir=normalize(wave0.xy+wave1.xy*.35+vec2(large-.5,medium-.5)*.7);
 float facing=dot(f.yz,dir);influence=mix(.72,1.32,crest)*mix(.7,1.3,facing*.5+.5);
 width=mix(foam_shape.x,foam_shape.y,width_noise*.45+shape*.55)*influence;
 // Keep a solid contact core at raster-field precision; animate only its outer edge.
 float contact=foam_bands.x*mix(1.0,2.0,shape);
 float inner=1.0-smoothstep(.2,contact+.25,d);
 float mid=(1.0-smoothstep(contact*.5,width+contact,d))*smoothstep(.65,1.45,blobs)*(1.0-inner);
 vec2 warp=(vec2(field_noise(p.xz*.083+vec2(t*.13,-t*.09)),field_noise(r*.061+vec2(-t*.11,t*.17)+vec2(9,21)))-.5)*foam_shape.z*smoothstep(contact,width+contact,d);
 float outer_d=interaction_sample(p.xz+warp).x;
 float islands=smoothstep(.85,1.7,blobs)*smoothstep(.35,.65,large);
 float outer=smoothstep(width*.7,width+.7,d)*(1.0-smoothstep(width+1.2,width+4.2,outer_d))*islands*influence;
 return vec4(inner,mid,outer,smoothstep(.18,.34,clamp(inner+mid*foam_detail.y+outer,0.0,1.0)))*(1.0-smoothstep(.25,.75,f.w));
}
vec3 reflection_color(vec3 p,vec3 direction){
 vec3 sky=mix(vec3(.075,.2,.42),vec3(.32,.64,.8),clamp(direction.y,0.0,1.0));float previous=.2;
 for(int i=0;i<32;i++){
  float distance=.7+float(i)*1.1+float(i*i)*.09;
  vec3 q=p+direction*distance;vec4 clip=projection*view*vec4(q,1);if(clip.w<=0.0)break;
  vec2 uv=vec2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);if(!valid_uv(uv))break;
  vec4 scene=texture2D(sampler_02,uv);float delta=-(view*vec4(q,1)).z-scene.w;
  if(scene.w>0.0&&delta>0.0&&delta<2.5&&scene.y>time_level.y-.3){
   float lo=previous,hi=distance;
   for(int j=0;j<4;j++){
    float mid=(lo+hi)*.5;vec3 r=p+direction*mid;vec4 s=texture2D(sampler_02,screen_uv(r));
    if(s.w>0.0&&-(view*vec4(r,1)).z>s.w)hi=mid;else lo=mid;
   }
   uv=screen_uv(p+direction*hi);float edge=smoothstep(0.0,.06,min(min(uv.x,uv.y),min(1.0-uv.x,1.0-uv.y)));
   return mix(sky,texture2D(sampler_01,uv).rgb,edge);
  }previous=distance;
 }return sky;
}
void main(){
 vec3 p=v_screen_position.xyz;vec2 uv=vec2(.5+.5*v_position.x/v_position.w,.5-.5*v_position.y/v_position.w);
 vec4 scene=texture2D(sampler_02,uv);float depth=-(view*vec4(p,1)).z;
 if(scene.w>0.0&&scene.w<depth-.015)discard;
 if(extra.x==5.0){attachment_01=vec4(vec3((p.y-time_level.y+.6)/1.2),1);return;}
 float thickness=scene.w>0.0?max(p.y-scene.y,0.0):25.0;
 vec3 n=normalize(cross(dFdx(p),dFdy(p)));if(n.y<0.0)n=-n;
 vec3 v=normalize(camera.xyz-p),light=normalize(sun.xyz);
 vec2 distortion=n.xz*params.z*smoothstep(0.0,optics.y,thickness)/max(.35,abs(v.y)),refract_uv=clamp(uv+distortion,vec2(.002),vec2(.998));
 vec4 refracted_scene=texture2D(sampler_02,refract_uv);
 // The offset ray must not pull dry foreground geometry below the waterline.
 if(refracted_scene.w<=depth||refracted_scene.w<=0.0||refracted_scene.y>ocean_height(refracted_scene.xz))refract_uv=uv;
 vec3 bottom=texture2D(sampler_01,refract_uv).rgb;float transmission=scene.w>0.0?exp(-thickness*params.y):0.0;
 vec3 body=mix(deep.rgb,shallow.rgb,exp(-thickness/optics.x)),color=mix(body,bottom*vec3(.5,.85,.93),transmission*.6);
 color*=clamp(1.0+(dot(n,light)-light.y)*foam_shadow.w,.58,1.3);
 float fresnel=.025+optics.w*pow(1.0-max(dot(n,v),0.0),optics.z);
 if(extra.x!=3.0)color=mix(color,reflection_color(p+n*.05,reflect(-v,n)),fresnel);
 if(extra.x==2.0)color=body;
 float plane=clamp((dot(n,light)-light.y)*3.0+.5,0.0,1.0);plane=floor(plane*6.0)/6.0;
 float crest_highlight=smoothstep(.25,.7,(p.y-time_level.y)/max(wave0.w+wave1.w+wave2.w+wave3.w,.001));
 color+=vec3(.08,.29,.3)*plane*crest_highlight*params.w;
 color*=.97+.06*field_noise(p.xz*.039);

 float depth_difference=scene.w>0.0?max(scene.w-depth,0.0):1000.0;
 float width,influence;vec4 layers=world_foam(p,width,influence);vec4 field=interaction_sample(p.xz);
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
