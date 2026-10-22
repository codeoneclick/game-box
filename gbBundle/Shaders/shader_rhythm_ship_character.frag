layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
void main(){vec4 c=texture2D(sampler_01,v_texcoord);if(c.a<.5)discard;attachment_01=vec4(c.rgb,1);attachment_02=vec4(normalize(v_normal)*.5+.5,.5);attachment_03=v_screen_position;}
