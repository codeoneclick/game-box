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
vec3 linear_to_srgb(vec3 c){
 c=max(c,vec3(0));return mix(12.92*c,1.055*pow(c,vec3(1.0/2.4))-.055,step(vec3(.0031308),c));
}
void main(){vec4 c=texture2D(sampler_01,v_texcoord);vec4 debug_ui=texture2D(sampler_02,v_texcoord);c.rgb=mix(c.rgb,debug_ui.rgb,debug_ui.a);if(post_display.x>.5)c.rgb=linear_to_srgb(c.rgb);attachment_01=vec4(c.rgb,1);}
