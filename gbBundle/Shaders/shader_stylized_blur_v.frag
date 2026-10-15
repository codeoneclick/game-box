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
void main(){
 if(post_flags.w<.5){attachment_01=vec4(0);return;}
 vec2 d=vec2(0,1)/vec2(textureSize(sampler_01,0));
 vec3 c=texture2D(sampler_01,v_texcoord).rgb*.227027;
 for(int i=1;i<=4;i++){
  float w=i==1?.1945946:i==2?.1216216:i==3?.054054:.016216;
  c+=(texture2D(sampler_01,v_texcoord+d*float(i)).rgb+texture2D(sampler_01,v_texcoord-d*float(i)).rgb)*w;
 }
 attachment_01=vec4(c,0);
}
