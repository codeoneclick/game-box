#include <metal_stdlib>
#include "mtl_common.h"
using namespace metal;
constexpr sampler ssao_nearest(coord::normalized,address::clamp_to_edge,filter::nearest);
constexpr sampler ssao_noise(coord::normalized,address::repeat,filter::nearest);
vertex common_v_output_t vertex_shader_ss_ssao(common_v_input_t in [[stage_in]])
{
    common_v_output_t out;
    out.position=float4(in.position,1.0);
    out.texcoord=float2(in.texcoord);
    return out;
}
fragment half4 fragment_shader_ss_ssao(common_v_output_t in [[stage_in]],
    constant ssao_u_input_t& uniforms [[buffer(1)]],
    texture2d<float> normals [[texture(0)]],texture2d<float> positions [[texture(1)]],
    texture2d<float> noise [[texture(2)]])
{
    float4 p = positions.sample(ssao_nearest,in.texcoord);
    float4 ns = normals.sample(ssao_nearest,in.texcoord);
    float3 n = uniforms.ao_params.w > 0.5 ? ns.xyz * 2.0 - 1.0 : ns.xyz;
    float3 vp = (uniforms.mat_v * float4(p.xyz, 1.0)).xyz;
    float depth = -vp.z;
    if (p.w <= 0.0 || ns.w <= 0.0 || dot(n,n) < 0.01 || depth <= 0.001 || uniforms.ao_params.z <= 0.0)
    {
        return half4(float4(1.0));
    }
    n = normalize((uniforms.mat_v * float4(n,0.0)).xyz);
    float radius = max(uniforms.ao_params.x, 0.001);
    float2 uv_radius = 0.5 * abs(float2(uniforms.mat_p[0][0],uniforms.mat_p[1][1])) * radius / depth;
    float rotation = noise.sample(ssao_noise,in.texcoord*32.0).r * 6.2831853;
    float occlusion = 0.0;
    // Fixed denominator: offscreen/background samples contribute no occlusion.
    for (int direction = 0; direction < 8; ++direction)
    {
        float angle = rotation + float(direction) * 0.7853982;
        float2 direction_vector = float2(cos(angle),sin(angle));
        for (int ring = 0; ring < 4; ++ring)
        {
            float fraction = (float(ring)+0.5)/4.0;
            float2 uv = in.texcoord + direction_vector*uv_radius*fraction;
            if (any(uv < float2(0.0)) || any(uv > float2(1.0))) continue;
            float4 q = positions.sample(ssao_nearest,uv);
            if (q.w <= 0.0) continue;
            float3 delta = (uniforms.mat_v * float4(q.xyz,1.0)).xyz - vp;
            float distance_value = length(delta);
            if (distance_value < 0.03 || distance_value >= radius) continue;
            float horizon = max(dot(n,delta/distance_value)-uniforms.ao_params.y,0.0);
            occlusion += horizon * (1.0-smoothstep(radius*0.25,radius,distance_value));
        }
    }
    float visibility = pow(1.0-clamp(occlusion/32.0*uniforms.ao_params.z*2.0,0.0,0.85),1.5);
    return half4(float4(float3(visibility),1.0));
}

vertex common_v_output_t vertex_shader_ss_ssao_blur(common_v_input_t in [[stage_in]])
{
    common_v_output_t out;
    out.position=float4(in.position,1.0);
    out.texcoord=float2(in.texcoord);
    return out;
}
fragment half4 fragment_shader_ss_ssao_blur(common_v_output_t in [[stage_in]],
    texture2d<float> ao [[texture(0)]],texture2d<float> positions [[texture(1)]],texture2d<float> normals [[texture(2)]])
{
    float4 p = positions.sample(ssao_nearest,in.texcoord);
    float4 ns = normals.sample(ssao_nearest,in.texcoord);
    if (p.w <= 0.0 || ns.w <= 0.0) { return half4(1.0); }
    float3 n = normalize(ns.xyz*2.0-1.0);
    float2 pixel = 1.0/float2(ao.get_width(),ao.get_height());
    float total = 0.0, weights = 0.0;
    for (int y=-2; y<=2; ++y) for (int x=-2; x<=2; ++x)
    {
        float2 uv = in.texcoord + float2(x,y)*pixel;
        if (any(uv < float2(0.0)) || any(uv > float2(1.0))) continue;
        float4 q = positions.sample(ssao_nearest,uv);
        float4 nq = normals.sample(ssao_nearest,uv);
        if (q.w <= 0.0 || nq.w <= 0.0) continue;
        float weight = exp(-float(x*x+y*y)/8.0-abs(q.w-p.w)/0.36);
        weight *= pow(max(dot(n,normalize(nq.xyz*2.0-1.0)),0.0),8.0);
        total += ao.sample(ssao_nearest,uv).r*weight;
        weights += weight;
    }
    return half4(float4(float3(total/max(weights,0.0001)),1.0));
}
