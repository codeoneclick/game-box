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
vec3 bloom_bright(vec2 uv){
 vec4 source=texture2D(sampler_01,uv);
 vec3 c=post_flags.x>.5?max(source.rgb,vec3(0)):clamp(source.rgb,0.0,1.0);
 float peak=max(c.r,max(c.g,c.b)),threshold=post_bloom.x,knee=max(threshold*.25,.001);
 float soft=clamp(peak-threshold+knee,0.0,2.0*knee);soft=soft*soft/(4.0*knee);
 return c*(max(peak-threshold,soft)/max(peak,.0001))*(1.0-source.a);
}
void main(){
 if(post_flags.w<.5){attachment_01=vec4(0);return;}
 vec2 d=vec2(post_bloom.z*.25)/vec2(textureSize(sampler_01,0));
 vec3 c=(bloom_bright(v_texcoord+d)+bloom_bright(v_texcoord-d)+bloom_bright(v_texcoord+vec2(d.x,-d.y))+bloom_bright(v_texcoord+vec2(-d.x,d.y)))*.25;
 attachment_01=vec4(c,0);
}
