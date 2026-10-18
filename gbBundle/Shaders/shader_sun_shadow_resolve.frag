uniform mat4 shadow_near;
uniform mat4 shadow_far;
uniform vec4 shadow_sun;
uniform vec4 shadow_params;
float shadow_depth(vec3 rgb){return dot(rgb,vec3(1,1.0/255.0,1.0/65025.0));}
float sun_pcf(int cascade,vec3 world,vec3 normal){
 float texel=cascade==0?shadow_params.x:shadow_params.y;
 mat4 matrix=cascade==0?shadow_near:shadow_far;
 vec3 p=(matrix*vec4(world+normal*texel*.3,1)).xyz;
 vec2 uv=vec2(.5+.5*p.x,.5-.5*p.y);
#if !defined(VULKAN_API)
 uv.y=1.0-uv.y;
#endif
 if(any(lessThan(uv,vec2(.002)))||any(greaterThan(uv,vec2(.998)))||abs(p.z)>=1.0)return 0.0;
 float bias=(.012+texel*.5*(1.0-max(dot(normal,shadow_sun.xyz),0.0)))/shadow_params.z;
 float depth=p.z*.5+.5-bias;
 ivec2 size=cascade==0?textureSize(sampler_04,0):textureSize(sampler_05,0);
 vec3 right=normalize(vec3(matrix[0][0],matrix[1][0],matrix[2][0]));
 vec3 up=normalize(vec3(matrix[0][1],matrix[1][1],matrix[2][1]));
 float facing=dot(normal,shadow_sun.xyz);
 if(facing<=.025)return 0.0;
 float world_width=2.0/length(vec3(matrix[0][0],matrix[1][0],matrix[2][0]));
 vec2 slope=vec2(dot(normal,right),-dot(normal,up))*world_width/(facing*shadow_params.z);
#if !defined(VULKAN_API)
 slope.y=-slope.y;
#endif
 vec2 coord=uv*vec2(size)-.5,f=fract(coord);ivec2 base=ivec2(floor(coord));float result=0.0;
 // Filter comparisons, never packed depth. Fractional tent weights avoid
 // nearest-texel jumps while the receiver moves across the stable light grid.
 for(int y=-1;y<=2;++y)for(int x=-1;x<=2;++x){
  ivec2 at=clamp(base+ivec2(x,y),ivec2(0),size-ivec2(1));
  vec3 rgb=cascade==0?texelFetch(sampler_04,at,0).rgb:texelFetch(sampler_05,at,0).rgb;
  float weight=max(0.0,2.0-abs(float(x)-f.x))*max(0.0,2.0-abs(float(y)-f.y));
  // Compare against the receiver plane at each filter tap. A constant depth
  // across the kernel falsely shadows flat ground at low solar elevations.
  float receiver_depth=depth+dot(slope,(vec2(at)+.5)/vec2(size)-uv);
  result+=(receiver_depth>shadow_depth(rgb)?1.0:0.0)*weight;
 }return result/16.0;
}
void main(){
 vec4 color=texture2D(sampler_01,v_texcoord),geometry=texture2D(sampler_02,v_texcoord);
 vec4 normal=texture2D(sampler_03,v_texcoord);float water=texture2D(sampler_06,v_texcoord).a;
 if(shadow_params.w==2.0||shadow_params.w==3.0){vec3 d=shadow_params.w==2.0?texture2D(sampler_04,v_texcoord).rgb:texture2D(sampler_05,v_texcoord).rgb;attachment_01=vec4(vec3(shadow_depth(d)),0);return;}
 if(shadow_sun.w<.5||geometry.w<=0.0||normal.w<=0.0||water>.99){attachment_01=color;return;}
 vec3 n=normalize(normal.xyz*2.0-1.0);
 // Receiver-plane depth needs the geometric plane. The SNORM8 shading normal
 // tilts an axis-aligned top by ~0.6 degrees, amplifying PCF error at low sun.
 vec3 plane=cross(dFdx(geometry.xyz),dFdy(geometry.xyz));
 float plane_length=length(plane);
 if(plane_length>1.e-8){
  plane/=plane_length;plane*=dot(plane,n)<0.0?-1.0:1.0;
  if(dot(plane,n)>.9)n=plane; // Keep shading normals at silhouettes/discontinuities.
 }
 vec3 near_clip=(shadow_near*vec4(geometry.xyz,1)).xyz;
 float edge=max(abs(near_clip.x),abs(near_clip.y));
 float blend=smoothstep(.72,.94,edge);
 float shade=blend<1.0?sun_pcf(0,geometry.xyz,n):0.0;
 if(blend>0.0)shade=mix(shade,sun_pcf(1,geometry.xyz,n),blend);
 shade*=smoothstep(.015,.10,shadow_sun.y)*(1.0-water);
 if(shadow_params.w==1.0){attachment_01=vec4(vec3(1.0-shade),0);return;}
 // Preserve cool ambient fill instead of driving the HDR result to black.
 // Alpha is the existing outline coverage, not opacity. Preserve it exactly.
 attachment_01=vec4(color.rgb*mix(vec3(1),vec3(.38,.47,.61),shade*.85),color.a);
}
