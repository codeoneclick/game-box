uniform mat4 view;
uniform vec4 clothing_color;
uniform vec4 sun;
layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
void main(){
 vec3 p=v_screen_position.xyz;
 vec3 n=normalize(cross(dFdx(p),dFdy(p)));
 if(dot(n,-(inverse(view)*vec4(0,0,1,0)).xyz)<0)n=-n;
 vec3 palette=texture2D(sampler_01,v_texcoord).rgb;
 float mask=texture2D(sampler_02,v_texcoord).r;
 vec3 color=mix(palette,clothing_color.rgb,mask)*(.5+.5*max(dot(n,normalize(sun.xyz)),0));
 attachment_01=vec4(color,1);attachment_02=vec4(n*.5+.5,1);
 attachment_03=vec4(p,max(-(view*vec4(p,1)).z,.001));
}
