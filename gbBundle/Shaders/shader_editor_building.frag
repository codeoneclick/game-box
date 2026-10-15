uniform vec4 building_tint;
uniform vec4 building_sun;
uniform mat4 building_view;
uniform vec4 light_sun;
uniform vec4 light_ambient;
uniform vec4 light_shape;
uniform vec4 light_options;
uniform vec4 light_material0;
uniform vec4 light_material1;
uniform vec4 light_camera;
layout(location=1) out vec4 attachment_02;
layout(location=2) out vec4 attachment_03;
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
 vec4 texel=texture2D(sampler_01,v_texcoord);if(texel.a<.1)discard;
 vec3 n=normalize(v_normal);float light=dot(n,normalize(building_sun.xyz));
 vec3 tone=mix(vec3(.32,.38,.50),vec3(.62,.66,.72),smoothstep(.21,.23,light));
 tone=mix(tone,vec3(1.02,.96,.83),smoothstep(.64,.66,light));
 vec3 base=texture_srgb_to_linear(texel.rgb);
 // Atlas has no material IDs: conservative color classes approximate leaves/wood.
 // Neutral texels blend stone and plaster; metal needs future asset metadata.
 float leaves=base.g>base.r*1.2&&base.g>base.b*1.2?1.0:0.0;
 float wood=base.r>base.b*1.35&&base.r>base.g*1.08?1.0:0.0;
 float neutral=(light_material0.z+light_material1.x)*.5;
 float strength=mix(mix(neutral,light_material0.y,wood),light_material0.w,leaves);
 vec3 color=light_shape.z<.5?base*mix(vec3(.32,.38,.50),tone,light_options.w):soft_light(base,n,normalize(building_sun.xyz),normalize(light_camera.xyz-v_screen_position.xyz),strength,leaves);
 color+=texture2D(sampler_02,v_texcoord).r*vec3(3.4,1.65,.45);
 color=mix(color,building_tint.rgb,building_tint.a);
 attachment_01=vec4(color,1);
 // RGB keeps SSAO normals; alpha identifies the complete building instance.
 attachment_02=vec4(n*.5+.5,building_sun.w);
 attachment_03=vec4(v_screen_position.xyz,max(-(building_view*v_screen_position).z,.001));
}
