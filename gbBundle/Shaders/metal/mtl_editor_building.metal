#include "mtl_common.h"
struct building_values {float4 tint,sun;float4x4 view;float4 light_sun,light_ambient,light_shape,light_options,light_material0,light_material1,light_camera;};
struct building_vertex {float4 position [[position]];float2 uv;float3 normal;float4 world;};
vertex building_vertex vertex_shader_editor_building(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){
 building_vertex out;out.world=u.mat_m*float4(in.position,1);out.position=u.mat_p*u.mat_v*out.world;
 out.uv=in.texcoord;out.normal=normalize(float3x3(u.mat_m[0].xyz,u.mat_m[1].xyz,u.mat_m[2].xyz)*in.normal.xyz);return out;
}
// Settings append to the existing custom buffer; mode 0 retains cel lighting.
float3 building_soft_light(float3 base,float3 n,float3 l,float3 v,float strength,float vegetation,float4 light_sun,float4 light_ambient,float4 light_shape,float4 light_options,float4 light_material1){
 float wrapped=clamp((dot(n,l)+light_shape.x)/(1.0+light_shape.x),0.0,1.0);
 float diffuse=pow(smoothstep(0.0,1.0,wrapped),light_shape.y);
 float3 sunlight=light_sun.rgb*light_sun.a;
 float3 color=base*(light_ambient.rgb*light_ambient.a+sunlight*diffuse);
 float3 half_direction=l+v;
 float3 h=half_direction/max(length(half_direction),.0001);
 float specular=pow(max(dot(n,h),0.0),light_material1.z)*max(dot(n,l),0.0);
 color+=sunlight*specular*strength*light_shape.w*light_options.y;
 float back=pow(max(dot(-n,l),0.0),2.0)*vegetation*light_options.x*light_options.z;
 return color+base*sunlight*back;
}

float3 building_texture_linear(float3 c){return mix(c/12.92,pow((c+.055)/1.055,float3(2.4)),step(float3(.04045),c));}
fragment g_buffer_output_t fragment_shader_editor_building(building_vertex in [[stage_in]],constant building_values& u [[buffer(0)]],texture2d<half> image [[texture(0)]],texture2d<half> emission [[texture(1)]]){
 half4 texel=image.sample(linear_sampler,in.uv);if(texel.a<.1)discard_fragment();
 float3 n=normalize(in.normal);float light=dot(n,normalize(u.sun.xyz));
 float3 tone=mix(float3(.32,.38,.50),float3(.62,.66,.72),smoothstep(.21,.23,light));
 tone=mix(tone,float3(1.02,.96,.83),smoothstep(.64,.66,light));
 constexpr sampler emission_sampler(coord::normalized,address::clamp_to_edge,filter::nearest);
 float3 base=building_texture_linear(float3(texel.rgb));
 // Atlas lacks material IDs: match the conservative GLSL color classification.
 float leaves=base.g>base.r*1.2&&base.g>base.b*1.2?1.0:0.0;
 float wood=base.r>base.b*1.35&&base.r>base.g*1.08?1.0:0.0;
 float neutral=(u.light_material0.z+u.light_material1.x)*.5;
 float strength=mix(mix(neutral,u.light_material0.y,wood),u.light_material0.w,leaves);
 float3 color=u.light_shape.z<.5?base*mix(float3(.32,.38,.50),tone,u.light_options.w):building_soft_light(base,n,normalize(u.sun.xyz),normalize(u.light_camera.xyz-in.world.xyz),strength,leaves,u.light_sun,u.light_ambient,u.light_shape,u.light_options,u.light_material1);
 color+=float(emission.sample(emission_sampler,in.uv).r)*float3(3.4,1.65,.45);color=mix(color,u.tint.rgb,u.tint.a);
 // RGB keeps SSAO normals; alpha identifies the complete building instance.
 g_buffer_output_t out;out.color=half4(half3(color),1);out.normal=float4(n*.5+.5,u.sun.w);out.position=float4(in.world.xyz,max(-(u.view*in.world).z,.001));return out;
}
vertex building_vertex vertex_shader_building_aura(common_v_input_t in [[stage_in]],constant common_u_input_t& u [[buffer(1)]]){
 building_vertex out;out.world=u.mat_m*float4(in.position.x,0,in.position.y,1);out.position=u.mat_p*u.mat_v*out.world;out.uv=in.texcoord;return out;
}
fragment half4 fragment_shader_building_aura(building_vertex in [[stage_in]],constant building_values& u [[buffer(0)]]){
 float2 p=abs(in.uv-.5)*2;float d=max(p.x,p.y);float alpha=(exp(-abs(d-.88)*30)*.8+(1-smoothstep(.7,1.,d))*.16)*(1-smoothstep(.94,1.,d));return half4(half3(u.tint.rgb*alpha),half(alpha));
}
