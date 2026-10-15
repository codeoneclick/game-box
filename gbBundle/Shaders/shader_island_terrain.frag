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
uniform vec4 light_sun;
uniform vec4 light_ambient;
uniform vec4 light_shape;
uniform vec4 light_options;
uniform vec4 light_material0;
uniform vec4 light_material1;
uniform vec4 light_camera;

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
layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
vec3 cel_light(float diffuse){
 // Three lighting tones with narrow, portable transition smoothing.
 float width=.01;
 vec3 tone=mix(vec3(.32,.38,.50),vec3(.62,.66,.72),smoothstep(.22-width,.22+width,diffuse));
 return mix(tone,vec3(1.02,.96,.83),smoothstep(.65-width,.65+width,diffuse));
}
// Settings append to the existing custom buffer; mode 0 retains cel lighting.
vec3 soft_light(vec3 base,vec3 n,vec3 l,vec3 v,float strength,float vegetation){
 float wrapped=clamp((dot(n,l)+light_shape.x)/(1.0+light_shape.x),0.0,1.0);
 float diffuse=pow(smoothstep(0.0,1.0,wrapped),light_shape.y);
 vec3 sunlight=light_sun.rgb*light_sun.a;
 vec3 color=base*(light_ambient.rgb*light_ambient.a+sunlight*diffuse);
 vec3 half_direction=l+v;
 vec3 h=half_direction/max(length(half_direction),.0001);
 float specular=pow(max(dot(n,h),0.0),light_material1.z)*max(dot(n,l),0.0);
 color+=sunlight*specular*strength*light_shape.w*light_options.y;
 float back=pow(max(dot(-n,l),0.0),2.0)*vegetation*light_options.x*light_options.z;
 return color+base*sunlight*back;
}
// Materials follow the triangle's constant logical terrace ID, independent of palette UVs.
float terrain_material(float terrace){
 if(terrace<=0.0)return 0.0; // submerged limestone
 if(terrace<1.5)return 1.0; // coast sand
 if(terrace<2.5)return 2.0; // shoreline grass
 return 3.0; // Reference: living grass above soil/mineral ledges at every height.
}
vec3 terrain_texel(float material,float side,vec2 uv,float lod){
 // The PNG stores six separate pixel-art resolutions; the GPU image has one mip.
 float resolution=32.0*exp2(-lod);
 vec2 pixel=vec2(material*36.0+2.0,lod*80.0+side*40.0+1.0)
           +min(floor(uv*resolution),vec2(resolution-1.0));
 vec2 atlas_uv=(pixel+.5)/vec2(256.0,480.0);
 atlas_uv.y=1.0-atlas_uv.y; // PNG serializer reverses rows on every backend.
 vec3 c=texture2D(sampler_03,atlas_uv).rgb;
 return mix(c/12.92,pow((c+.055)/1.055,vec3(2.4)),step(vec3(.04045),c));
}
vec3 terrain_texture(vec3 p,vec3 n,float terrace,float material){
 float layer_height=max(extra.z,.001);
 float top_y=terrace==1.0?extra.w:(terrace>=2.0?(terrace-1.0)*layer_height:terrace*layer_height);
 vec2 top_q=p.xz*2.0;
 vec2 side_q=vec2((abs(n.x)>abs(n.z)?p.z:p.x)*2.0,(top_y-p.y)/layer_height*4.0);
 // Derivatives precede quantization. Each world pixel chooses its own palette
 // entry; there is no repeated 2m block, border, bevel or per-block outline.
 vec2 top_footprint=fwidth(top_q),side_footprint=fwidth(side_q);
 float side=n.y>.5?0.0:1.0;
 vec2 footprint=mix(top_footprint,side_footprint,side);
 vec2 cell=floor(top_q),seed=cell+vec2(material*37.0,material*71.0);
 vec2 uv=(floor(vec2(hash21(seed),hash21(seed+vec2(19.1,83.7)))*4.0)+.5)/4.0;
 if(side>.5){
  float column=floor(hash21(vec2(floor(side_q.x),material+17.0))*4.0);
  uv=vec2((column+.5)/4.0,clamp(side_q.y/4.0,0.0,1.0));
 }
 float visible=1.0-smoothstep(.5,2.0,max(footprint.x,footprint.y));
 return mix(terrain_texel(material,side,uv,5.0),terrain_texel(material,side,uv,0.0),visible);
}
vec3 tree_pixel_color(vec3 base,vec2 uv){
 vec2 pixel=uv*6.0,footprint=fwidth(pixel);
 float visible=1.0-smoothstep(.5,2.0,max(footprint.x,footprint.y));
 float grain=floor(hash21(floor(pixel))*4.0)/3.0;
 float leaves=base.g>max(base.r,base.b)?1.0:0.0;
 float stripes=floor(hash21(vec2(floor(uv.x*4.0),floor(uv.y*2.0)))*4.0)/3.0;
 float detail=mix(.7+.3*stripes+.3*grain,.6+.8*grain,leaves);
 return base*mix(1.0,detail,visible);
}
vec3 grass_pixel_color(vec3 base,vec2 uv){
 // Four by eight blade-local pixels stay attached as wind bends the card.
 vec2 pixel=uv*vec2(4.0,8.0),footprint=fwidth(pixel);
 float visible=1.0-smoothstep(.5,2.0,max(footprint.x,footprint.y));
 float dx=dFdx(uv.y),dy=dFdy(uv.y);
 vec3 slope=(dFdx(base)*dx+dFdy(base)*dy)/max(dx*dx+dy*dy,.00000001);
 float center=(floor(pixel.y)+.5)/8.0;
 vec3 stepped=max(base+slope*(center-uv.y),vec3(0));
 float tone=.8+.2*floor(hash21(floor(pixel))*3.0);
 return mix(base,stepped*tone,visible);
}
void main(){
 if(v_color.a>.49&&v_color.a<.51){
  float x=v_tangent.z>.5?1.0-v_texcoord.x:v_texcoord.x;
  float row=floor(clamp(v_texcoord.y,0.0,.9999)*8.0);
  float column=floor(clamp(x,0.0,.9999)*4.0),left=floor(row*.5);
  if(column<left||column>=min(left+2.0,4.0))discard;
 }
 vec3 p=v_screen_position.xyz,n=normalize(v_normal);float water=ocean_height(p.xz),submerged=max(water-p.y,0.0);
 // Dry surfaces above the maximum wave crest cannot acquire animated wetness.
 // Keep stored buoyancy wetness independent so exposed props can dry over time.
 float crest_level=time_level.y+1.1*(wave0.w+wave1.w+wave2.w+wave3.w);
 float reachable=1.0-smoothstep(crest_level-.05,crest_level,p.y);
 float wet=max(extra.y*(1.0-smoothstep(0.0,1.5,p.y-time_level.y)),reachable*(1.0-smoothstep(0.0,.65,p.y-water)));
 bool terrain=v_tangent.x>.5;
 float terrace=floor(v_color.a+.5)-11.0,material=terrain_material(terrace);
 vec3 base_color=terrain?terrain_texture(p,n,terrace,material):v_color.rgb;
 if(v_tangent.y>.5)base_color=tree_pixel_color(base_color,v_texcoord);
 if(v_color.a>.49&&v_color.a<.51)base_color=grass_pixel_color(base_color,v_texcoord);
 vec3 l=normalize(sun.xyz),v=normalize(camera.xyz-p);
 float grass=v_color.a>.49&&v_color.a<.51?1.0:0.0;
 float leaves=v_tangent.y>.5&&base_color.g>max(base_color.r,base_color.b)?1.0:0.0;
 // Grass/tree flags are explicit; remaining brown props approximate wood, others stone.
 float strength=base_color.r>base_color.b*1.25&&base_color.r>base_color.g*1.1?light_material0.y:light_material0.z;
 strength=mix(strength,light_material0.x,grass);strength=mix(strength,light_material0.w,leaves);
 float vegetation=max(grass,leaves);
 if(terrain){
  float grassy_top=n.y>.5&&material>=2.0&&material<=5.0?1.0:0.0;
  strength=mix(light_material0.z,light_material0.x,grassy_top);vegetation=grassy_top;
 }
 vec3 color=light_shape.z<.5?base_color*mix(vec3(.32,.38,.50),cel_light(dot(n,l)),light_options.w):soft_light(base_color,n,l,v,strength,vegetation);
 color*=mix(1.0,.64,wet);
 if(submerged>0.0){
  float caustics=caustic_pattern(p.xz*caustic_params.x)*max(n.y,.12)+caustic_pattern(vec2(p.x+p.z,p.y)*caustic_params.x)*(1.0-abs(n.y))*.4;
  color+=vec3(.36,.54,.39)*caustics*exp(-submerged*caustic_params.w)*caustic_params.z;
 }
 vec3 half_direction=v+l,h=half_direction/max(length(half_direction),.0001);
 if(light_shape.z<.5)color+=wet*.2*light_options.w*smoothstep(.94,.955,max(dot(n,h),0.0))*vec3(1,.94,.82);
 else color+=wet*.2*pow(max(dot(n,h),0.0),light_material1.z)*max(dot(n,l),0.0)*light_sun.rgb*light_sun.a*light_shape.w*light_options.y;
 float outline_id=v_color.a>=2.0?floor(v_color.a+.5):v_color.a;
 attachment_01=vec4(color,1);attachment_02=vec4(n*.5+.5,outline_id);attachment_03=vec4(p,max(-(view*vec4(p,1)).z,.001));
}
