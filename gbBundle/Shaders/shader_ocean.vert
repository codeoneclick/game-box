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
 p+=vec2(sin(p.y*.9+time_level.x*.65),cos(p.x*.8-time_level.x*.4))*.35;
 float a=sin(p.x*2.7+sin(p.y*1.9+time_level.x*.6)),b=sin(p.y*2.9+sin(p.x*2.1-time_level.x*.7));
 return pow(clamp(1.0-abs(a+b)*1.7,0.0,1.0),5.0);
}
void main(){
 vec2 rest=(get_mat_m()*vec4(a_position,1)).xz;
 vec3 p=vec3(rest.x,time_level.y,rest.y),tx=vec3(1,0,0),tz=vec3(0,0,1);
 float extent=max(abs(rest.x),abs(rest.y));
 if(extent<720.0)p.y=mix(time_level.y,ocean_height(rest),1.0-smoothstep(384.0,720.0,extent));
 gl_Position=get_mat_p()*get_mat_v()*vec4(p,1);
 v_position=gl_Position;v_screen_position=vec4(p,1);v_normal=normalize(cross(tz,tx));v_color=vec4(1);
}
