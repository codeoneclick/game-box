uniform vec4 stage_glow;
layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
void main(){vec3 n=normalize(v_normal);float light=.42+.58*max(dot(n,normalize(vec3(.35,.8,.45))),0.);vec3 color=v_color.rgb*mix(light,1.25+stage_glow.x,v_color.a);attachment_01=vec4(color,1);attachment_02=vec4(n*.5+.5,1);attachment_03=v_screen_position;}
