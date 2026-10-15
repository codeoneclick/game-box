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
void main(){
 vec3 p=v_screen_position.xyz;vec2 uv=vec2(.5+.5*v_position.x/v_position.w,.5-.5*v_position.y/v_position.w);
 if(extra.x!=0.0){attachment_01=vec4(0);return;}
 vec4 scene=texture2D(sampler_02,uv);float depth=-(view*vec4(p,1)).z;
 if(scene.w>0.0&&scene.w<depth-.015)discard;
 vec3 n=normalize(cross(dFdx(p),dFdy(p)));if(n.y<0.0)n=-n;
 float width,influence;vec4 layers=world_foam(p-vec3(0,foam_shape.w,0),width,influence);
 float shade=floor(max(dot(n,normalize(sun.xyz)),0.0)*5.0)/5.0;
 vec3 color=mix(foam_shadow.rgb,foam.rgb,.8+.2*shade)*1.24;float coverage=layers.w;
 attachment_01=vec4(color*coverage,coverage);
}
