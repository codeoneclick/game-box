//
//  mtl_ssao.metal
//  gbDemo
//
//  Created by serhii.s on 6/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <metal_stdlib>
#include "mtl_common.h"

using namespace metal;

vertex common_v_output_t vertex_shader_ss_ssao(common_v_input_t in [[stage_in]],
                                               constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

float do_ambient_occlusion(float2 texcoord, float2 uv, float3 position, float3 normal, texture2d<float> position_texture)
{
    const float g_scale = 0.5;
    const float g_bias = 0.05;
    const float g_intensity = 2.0;
    
    float3 diff = (float3)position_texture.sample(linear_sampler, texcoord + uv).xyz - position;
    const float3 v = normalize(diff);
    const float d = length(diff) * g_scale;
    return max(0.0, dot(normal, v) - g_bias) * (1.0 / (1.0 + d)) * g_intensity;
}

fragment half4 fragment_shader_ss_ssao(common_v_output_t in [[stage_in]],
                                       texture2d<half> normal_texture [[texture(0)]],
                                       texture2d<float> position_texture [[texture(1)]],
                                       texture2d<half> random_texture [[texture(2)]])
{
    float g_sample_rad = 1.0;
    const float2 directions[4] = {float2(1, 0),float2(-1, 0), float2(0, 1),float2(0, -1)};
    float4 normal = (float4)normal_texture.sample(linear_sampler, in.texcoord);
    float4 position = position_texture.sample(linear_sampler, in.texcoord);
    float2 random = (float2)random_texture.sample(linear_sampler, in.texcoord).xy;
    
    float ao = 0.0;
    float radius = g_sample_rad / position.z;
    
    int iterations = 4;
    for (int i = 0; i < iterations; ++i)
    {
        float2 coord1 = reflect(directions[i], random) * radius;
        float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
        
        ao += do_ambient_occlusion(in.texcoord, coord1 * 0.25, position.xyz, normal.xyz, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord2 * 0.5, position.xyz, normal.xyz, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord1 * 0.75, position.xyz, normal.xyz, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord2, position.xyz, normal.xyz, position_texture);
    }
    
    ao /= (float)iterations * 4.0;
    return half4(clamp(ao, 0.0, 1.0));
}
