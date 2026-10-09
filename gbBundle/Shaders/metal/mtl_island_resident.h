#ifndef GB_ISLAND_RESIDENT_METAL
#define GB_ISLAND_RESIDENT_METAL
struct resident_v_input {
 float3 position [[attribute(0)]];float2 texcoord [[attribute(1)]];
 float4 weights [[attribute(4)]];float4 bone_ids [[attribute(5)]];
};
struct resident_bones {float4x4 u_bones[64];};
struct resident_fragment {float4x4 view;float4 clothing_color,sun;};
vertex common_v_output_t vertex_shader_island_resident(resident_v_input in [[stage_in]],constant common_u_input_t& mvp [[buffer(1)]],constant resident_bones& bones [[buffer(2)]]){
 common_v_output_t out={};float4 weights=in.weights/max(dot(in.weights,float4(1)),.00001f),p=float4(0);
 for(int i=0;i<4;i++)p+=bones.u_bones[clamp(int(in.bone_ids[i]),0,63)]*float4(in.position,1)*weights[i];
 out.view_space_position=get_mat_m(mvp)*p;out.position=get_mat_p(mvp)*get_mat_v(mvp)*out.view_space_position;out.texcoord=in.texcoord;return out;
}
fragment g_buffer_output_t fragment_shader_island_resident(common_v_output_t in [[stage_in]],constant resident_fragment& u [[buffer(0)]],texture2d<float> palette [[texture(0)]],texture2d<float> mask [[texture(1)]]){
 g_buffer_output_t out;float3 p=in.view_space_position.xyz,n=normalize(cross(dfdx(p),dfdy(p)));
 if(dot(n,-(transpose(u.view)*float4(0,0,1,0)).xyz)<0)n=-n;
 constexpr sampler nearest_sampler(coord::normalized,address::clamp_to_edge,filter::nearest);
 float3 color=mix(palette.sample(nearest_sampler,in.texcoord).rgb,u.clothing_color.rgb,mask.sample(nearest_sampler,in.texcoord).r)*(.5+.5*max(dot(n,normalize(u.sun.xyz)),0.0));
 out.color=half4(half3(color),1);out.normal=half4(half3(n*.5+.5),1);out.position=float4(p,max(-(u.view*float4(p,1)).z,.001));return out;
}
#endif
