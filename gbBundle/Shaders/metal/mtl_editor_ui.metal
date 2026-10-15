#include "mtl_common.h"
using namespace metal;
float3 editor_display_linear(float3 c){c=max(c,float3(0));return mix(c/12.92,pow((c+.055)/1.055,float3(2.4)),step(float3(.04045),c));}


vertex common_v_output_t vertex_shader_editor_ui(common_v_input_t in [[stage_in]],
    constant common_u_input_t& u [[buffer(1)]]) {
    common_v_output_t out = {};
    out.position = get_mat_mvp(u) * float4(in.position, 1);
    out.texcoord = in.texcoord; out.color = in.color; return out;
}
fragment half4 fragment_shader_editor_ui(common_v_output_t in [[stage_in]]) {
    float2 size = 1.0 / max(fwidth(in.texcoord), float2(.00001));
    float radius = min(12.0, min(size.x,size.y) * .5 - 1.0);
    float2 p = (in.texcoord - .5) * size;
    float2 q = abs(p) - (size * .5 - float2(2.0 + radius));
    float distance = length(max(q,0.0)) + min(max(q.x,q.y),0.0) - radius;
    float selected = step(.45,in.color.g) * step(.55,in.color.b) * (1.0-step(.4,in.color.r));
    float edge = 1.0 - smoothstep(.7,1.7,abs(distance+.8));
    float3 border = mix(float3(.32,.53,.61),float3(.30,.88,1),selected);
    float3 fill = in.color.rgb * (1.0 + (.5-in.texcoord.y)*.16);
    float3 color = mix(fill,border,edge*mix(.7,1.0,selected));
    float alpha = (1.0-smoothstep(-.5,.7,distance))*in.color.a;
    float glow = selected*exp(-abs(distance)*.65)*.45;
    color += float3(.04,.30,.36)*glow;
    return half4(half3(editor_display_linear(color)),half(max(alpha,glow)));
}
vertex common_v_output_t vertex_shader_editor_icon(common_v_input_t in [[stage_in]],
    constant common_u_input_t& u [[buffer(1)]]) {
    common_v_output_t out = {};
    out.position = get_mat_mvp(u) * float4(in.position, 1);
    out.texcoord = in.texcoord; out.color = in.color; return out;
}
fragment half4 fragment_shader_editor_icon(common_v_output_t in [[stage_in]],
    texture2d<half> image [[texture(0)]]) {
    constexpr sampler sample(filter::linear, address::clamp_to_edge);
    float4 color=float4(image.sample(sample,in.texcoord))*in.color;return half4(float4(editor_display_linear(color.rgb),color.a));
}
