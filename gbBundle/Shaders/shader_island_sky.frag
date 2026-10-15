uniform vec4 sun;
// GB_SAMPLER_02_CUBE selects the engine's existing cube sampler slot.
layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
vec3 sky_linear(vec3 c){return mix(c/12.92,pow((c+.055)/1.055,vec3(2.4)),step(vec3(.04045),c));}
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
void main(){
#if defined(USE_LAYOUTS)
 vec3 c=texture(sampler_02,normalize(v_normal)).rgb;
#else
 vec3 c=textureCube(sampler_02,normalize(v_normal)).rgb;
#endif
 attachment_01=vec4(island_sky_color(v_normal,normalize(sun.xyz))+sky_linear(c)*.001,1);attachment_02=vec4(0);attachment_03=vec4(0);
}
