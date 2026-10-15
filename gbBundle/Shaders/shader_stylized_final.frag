uniform vec4 post_flags;
uniform vec4 post_color;
uniform vec4 post_shadow;
uniform vec4 post_highlight;
uniform vec4 post_bloom;
uniform vec4 post_fog;
uniform vec4 post_ranges;
uniform vec4 post_dof;
uniform vec4 post_extra;
uniform vec4 post_camera;
uniform vec4 post_display;
uniform mat4 post_view;
uniform mat4 post_projection;
// Narkowicz ACES fitted curve (MIT/CC0), linear input and linear output.
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 stylized_tone(vec3 c){
 c=max(c,vec3(0))*exp2(post_color.x);
 return post_flags.y>.5?clamp((c*(2.51*c+.03))/(c*(2.43*c+.59)+.14),0.0,1.0):c;
}
vec3 stylized_grade(vec3 c,vec4 surface){
 if(post_flags.z<.5)return c;
 float object_id=surface.w;
 float luminance=dot(c,vec3(.2126,.7152,.0722));
 if(object_id>=12000000.0){
  // Lift the toe/midtones without raising true black or flattening cel bands.
  c+=post_highlight.w*sqrt(max(c,vec3(0)))*(1.0-smoothstep(.1,.65,luminance));
  float chroma=max(c.r,max(c.g,c.b))-min(c.r,min(c.g,c.b));
  float neutral=1.0-smoothstep(.02,.12,chroma);
  vec3 mineral=mix(vec3(.96,1.0,1.04),vec3(1.02,1.01,.97),smoothstep(.35,.75,luminance));
  c*=mix(vec3(1),mineral,neutral);
 }
 c=mix(vec3(dot(c,vec3(.2126,.7152,.0722))),c,post_color.y);
 c=(c-vec3(.18))*post_color.z+vec3(.18);
 // Keep bright foliage while suppressing the most acid green chroma.
 float green=smoothstep(.02,.15,c.g-max(c.r,c.b))*smoothstep(.35,.85,c.g);
 if(object_id>0.0)c=mix(c,vec3(dot(c,vec3(.2126,.7152,.0722))),.16*green);
 if(object_id>=2.0&&object_id<=27.0&&surface.g<.9)c*=vec3(1.025,1.0,.985);
 c*=vec3(1.0+post_color.w,1.0+post_shadow.w,1.0-post_color.w);
 float balance=smoothstep(.08,.75,luminance);
 c*=mix(post_shadow.rgb,post_highlight.rgb,balance);
 return max(c,vec3(0));
}
vec2 stylized_depth(vec2 uv){
 vec4 geometry=texture2D(sampler_05,uv);
 float water=texture2D(sampler_06,uv).a;
 vec2 ndc=vec2(uv.x*2.0-1.0,1.0-uv.y*2.0);
 vec3 view_ray=normalize(vec3((ndc+vec2(post_projection[2][0],post_projection[2][1]))/vec2(post_projection[0][0],post_projection[1][1]),-1));
 vec3 ray=transpose(mat3(post_view))*view_ray;
 if(water>.5&&ray.y<-.00001){float d=max(0.0,(post_extra.z-post_camera.y)/ray.y);return vec2(d,d*-view_ray.z);}
 if(geometry.w>0.0)return vec2(length(geometry.xyz-post_camera.xyz),geometry.w);
 return vec2(post_ranges.y);
}
void main(){
 vec4 source=texture2D(sampler_01,v_texcoord);
 vec3 hdr=source.rgb;
 hdr=post_flags.x>.5?hdr:clamp(hdr,0.0,1.0);
 if(post_ranges.z>.5){float fog=post_fog.w*smoothstep(post_ranges.x,post_ranges.y,stylized_depth(v_texcoord).x);hdr=mix(hdr,post_fog.rgb,fog);}
 vec3 toned=stylized_tone(hdr);
 vec3 graded=stylized_grade(toned,texture2D(sampler_02,v_texcoord));
 vec3 color=graded;
 color=mix(color,vec3(.05126946),source.a);
 // The HDR view uses compression solely for displaying values above one.
 int debug_view=int(post_bloom.w+.5);
 if(debug_view==1)color=max(source.rgb,vec3(0))/(1.0+max(source.rgb,vec3(0)));
 else if(debug_view==4)color=toned;
 else if(debug_view==5)color=graded;
 attachment_01=vec4(color,1);
}
