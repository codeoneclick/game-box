uniform vec4 field_debug;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 camera;
uniform vec4 time_level;
uniform vec4 sun;
// Normal alpha stores exact object IDs. Only selected silhouettes get ink;
// facets belonging to the same object never create internal edges.
float scene_edge(ivec2 pixel,vec2 fraction,vec2 footprint,vec4 p){
 ivec2 size=textureSize(sampler_08,0);
 float object_id=texelFetch(sampler_08,pixel,0).w;
 if(sun.w<.5||p.w<=0.0||object_id<2.0||time_level.w!=0.0)return 0.0;
 float edge=0.0;
 for(int i=0;i<4;i++){
  ivec2 offset=i==0?ivec2(1,0):i==1?ivec2(-1,0):i==2?ivec2(0,1):ivec2(0,-1);
  ivec2 adjacent=pixel+offset;
  if(any(lessThan(adjacent,ivec2(0)))||any(greaterThanEqual(adjacent,size)))continue;
  float adjacent_id=texelFetch(sampler_08,adjacent,0).w;
  // Visible grass covers the terrain, without creating an outline around it.
  if(adjacent_id>0.0&&adjacent_id<1.0)continue;
  if(adjacent_id!=object_id){
   float distance=i==0?1.0-fraction.x:i==1?fraction.x:i==2?1.0-fraction.y:fraction.y;
   float pixel_footprint=i<2?footprint.x:footprint.y;
   // A .5 screen-pixel half-width replaces the former full adjacent pixels.
   // Integrate the band over one fragment footprint; corners use max, not sum.
   const float half_width=.5;
   float coverage=clamp(half_width+.5-distance/pixel_footprint,0.0,1.0);
   edge=max(edge,coverage);
  }
 }
 return edge*(object_id<=27.0?.18:object_id>=8000000.0?.55:.35);
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
// Screen-space data is suitable for contact only, not sparse long cast shadows.
float island_sun_shadow(vec4 geometry,vec2 uv,float water_alpha){
 if(field_debug.w<.5||geometry.w<=0.0||water_alpha>.5||sun.y<=.03)return 0.0;
 ivec2 size=textureSize(sampler_07,0),pixel=clamp(ivec2(uv*vec2(size)),ivec2(0),size-ivec2(1));
 vec4 receiver=texelFetch(sampler_08,pixel,0);
 if(receiver.w<1.0)return 0.0; // Cutout grass must not become a solid receiver.
 vec3 normal=normalize(receiver.xyz*2.0-1.0),light=normalize(sun.xyz);
 if(dot(normal,light)<0.0)return 0.0;
 vec3 origin=geometry.xyz+normal*.025;
 float shadow=0.0;
 // Adjacent samples overlap the thin depth interval: no separated silhouette copies.
 for(int i=0;i<32;i++){
  float step_distance=(float(i)+1.0)*.046875; // At most 1.5 metres.
  vec3 q=origin+light*step_distance;vec4 clip=projection*view*vec4(q,1);
  if(clip.w<=0.0)break;
  vec2 sample_uv=vec2(.5+.5*clip.x/clip.w,.5-.5*clip.y/clip.w);
  if(any(lessThan(sample_uv,vec2(.002)))||any(greaterThan(sample_uv,vec2(.998))))break;
  ivec2 at=clamp(ivec2(sample_uv*vec2(size)),ivec2(0),size-ivec2(1));
  if(all(equal(at,pixel)))continue;
  vec4 scene=texelFetch(sampler_07,at,0);float id=texelFetch(sampler_08,at,0).w;
  float delta=-(view*vec4(q,1)).z-scene.w;
  // Depth alone treats unrelated foreground as an occluder. Also require a
  // nearby world-space surface above the receiver's tangent plane.
  if(scene.w>0.0&&id>=1.0&&delta>.005&&delta<.09&&
     length(scene.xyz-q)<.12&&dot(scene.xyz-geometry.xyz,normal)>.04){
   float border=smoothstep(0.0,.04,min(min(sample_uv.x,sample_uv.y),min(1.0-sample_uv.x,1.0-sample_uv.y)));
   float contact=(1.0-smoothstep(.8,1.5,step_distance))*(1.0-smoothstep(.04,.09,delta));
   shadow=max(shadow,border*contact);
  }
 }return shadow*.45*smoothstep(.03,.12,sun.y);
}
vec3 atmosphere_scatter(vec3 color,vec3 ray,float distance){
 // Horizon scattering is independent of the optional post-process distance fog.
 if(time_level.z<.5||time_level.w!=0.0||distance<=1800.0)return color;
 float end=min(distance,12000.0),step_length=(end-1800.0)/8.0;
 float mu=dot(ray,normalize(sun.xyz)),g=.65;
 float forward=(1.0-g*g)/pow(max(1.0+g*g-2.0*g*mu,.08),1.5);
 vec3 light=island_horizon_color(ray,normalize(sun.xyz));
 vec3 scatter=vec3(0);float transmission=1.0;
 for(int i=0;i<8;i++){
  vec3 p=camera.xyz+ray*(1800.0+(float(i)+.5)*step_length);
  float clouds=.65+.25*sin(p.x*.008+sin(p.z*.006+time_level.x*.025))*cos(p.y*.017-p.z*.003)
                  +.1*sin(dot(p,vec3(-.005,.013,.009))-time_level.x*.018);
  float density=exp(-max(p.y-time_level.y,0.0)/240.0)*clouds;
  float opacity=1.0-exp(-density*.00018*step_length);
  scatter+=transmission*opacity*light*(.95+.1*clouds);
  transmission*=1.0-opacity;
 }
 return color*transmission+scatter;
}
void main()
{
    if(field_debug.x>0.0){vec4 f=texture2D(sampler_04,v_texcoord);vec3 c=vec3(clamp(f.x/8.0,0.0,1.0));if(field_debug.x==15.0){vec2 sides=texture2D(sampler_05,v_texcoord).rg;c=vec3(min(sides.r,sides.g));}if(field_debug.x==17.0)c=vec3(f.yz*.5+.5,0);attachment_01=vec4(c,0);return;}
    float ao=texture2D(sampler_06,v_texcoord).r;
    if(field_debug.y>0.0){attachment_01=vec4(vec3(ao),0.0);return;}
    vec4 water=texture2D(sampler_02,v_texcoord),foam=texture2D(sampler_03,v_texcoord);
    vec4 geometry=texture2D(sampler_07,v_texcoord);
    float shade=island_sun_shadow(geometry,v_texcoord,water.a);
    vec3 opaque=texture2D(sampler_01,v_texcoord).rgb*ao*mix(vec3(1),vec3(.55,.64,.78),shade*.9);
    vec3 color=mix(opaque,water.rgb,water.a);
    color=color*(1.0-foam.a)+foam.rgb;
    vec2 ndc=vec2(v_texcoord.x*2.0-1.0,1.0-v_texcoord.y*2.0);
    vec3 ray=normalize(transpose(mat3(view))*vec3((ndc+vec2(projection[2][0],projection[2][1]))/vec2(projection[0][0],projection[1][1]),-1));
    float horizon_pixel=max(fwidth(ray.y),.0001),horizon_elevation=ray.y;
    ivec2 outline_size=textureSize(sampler_08,0);
    vec2 outline_texel=v_texcoord*vec2(outline_size);
    // Derivatives precede per-fragment visibility branches.
    vec2 outline_footprint=max(fwidth(outline_texel),vec2(.0001));
    float edge=0.0;
    if(sun.w>.5&&geometry.w>0.0&&water.a<.5&&time_level.w==0.0)
    {
        ivec2 pixel=clamp(ivec2(outline_texel),ivec2(0),outline_size-ivec2(1));
        edge=scene_edge(pixel,fract(outline_texel),outline_footprint,geometry)*(1.0-smoothstep(.1,.5,water.a));
    }
    float distance=0.0; // Sky already contains its own atmospheric gradient.
    if(water.a>.5&&ray.y<-.00001)distance=max(0.0,(time_level.y-camera.y)/ray.y);
    else if(geometry.w>0.0){distance=length(geometry.xyz-camera.xyz);ray=normalize(geometry.xyz-camera.xyz);}
    color=atmosphere_scatter(color,ray,distance);
    if(time_level.z>.5&&time_level.w==0.0&&(water.a>.5||geometry.w<=0.0)){
     float day=smoothstep(-.12,.12,sun.y);
     float core=exp(-pow(horizon_elevation/(horizon_pixel*1.25),2.0));
     float halo=exp(-pow(horizon_elevation/.012,2.0));
     float shoulder=exp(-pow(horizon_elevation/.035,2.0));
     vec3 air=mix(vec3(.018,.035,.075),vec3(.16,.42,.66),day);
     color=mix(color,air,.35*halo+.10*shoulder);
     color+=day*(vec3(1.0,1.8,2.4)*core+vec3(.04,.10,.16)*halo);
    }
    attachment_01=vec4(color,edge);
}
