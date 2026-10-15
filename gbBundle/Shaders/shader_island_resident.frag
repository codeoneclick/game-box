uniform mat4 view;
uniform vec4 clothing_color;
uniform vec4 sun;
uniform vec4 light_sun;
uniform vec4 light_ambient;
uniform vec4 light_shape;
uniform vec4 light_options;
uniform vec4 light_material0;
uniform vec4 light_material1;
uniform vec4 light_camera;
layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
vec3 cel_light(float diffuse){
 float width=.01;
 vec3 tone=mix(vec3(.32,.38,.50),vec3(.62,.66,.72),smoothstep(.22-width,.22+width,diffuse));
 return mix(tone,vec3(1.02,.96,.83),smoothstep(.65-width,.65+width,diffuse));
}
vec3 texture_srgb_to_linear(vec3 c){return mix(c/12.92,pow((c+.055)/1.055,vec3(2.4)),step(vec3(.04045),c));}
// Settings append to the existing custom buffer; mode 0 retains cel lighting.
vec3 soft_light(vec3 base,vec3 n,vec3 l,vec3 v,float strength,float vegetation){
 float wrapped=clamp((dot(n,l)+light_shape.x)/(1.0+light_shape.x),0.0,1.0);
 float diffuse=pow(smoothstep(0.0,1.0,wrapped),light_shape.y);
 vec3 sunlight=light_sun.rgb*light_sun.a;
 vec3 color=base*(light_ambient.rgb*light_ambient.a+sunlight*diffuse);
 vec3 half_direction=l+v;
 vec3 h=half_direction/max(length(half_direction),.0001);
 float specular=pow(max(dot(n,h),0.0),light_material1.z)*max(dot(n,l),0.0);
 color+=sunlight*specular*strength*light_shape.w*light_options.y;
 float back=pow(max(dot(-n,l),0.0),2.0)*vegetation*light_options.x*light_options.z;
 return color+base*sunlight*back;
}
void main(){
 vec3 p=v_screen_position.xyz;
 vec3 n=normalize(cross(dFdx(p),dFdy(p)));
 if(dot(n,-(inverse(view)*vec4(0,0,1,0)).xyz)<0)n=-n;
 vec3 palette=texture_srgb_to_linear(texture2D(sampler_01,v_texcoord).rgb);
 float mask=texture2D(sampler_02,v_texcoord).r;
 vec3 base=mix(palette,clothing_color.rgb,mask),l=normalize(sun.xyz);
 vec3 color=light_shape.z<.5?base*mix(vec3(.32,.38,.50),cel_light(dot(n,l)),light_options.w):soft_light(base,n,l,normalize(light_camera.xyz-p),light_material1.x,0.0);
 attachment_01=vec4(color,1);attachment_02=vec4(n*.5+.5,sun.w);
 attachment_03=vec4(p,max(-(view*vec4(p,1)).z,.001));
}
