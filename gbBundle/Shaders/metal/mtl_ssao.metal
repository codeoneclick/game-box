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

float do_ambient_occlusion(float2 texcoord, float2 uv, float3 position, float3 normal, float range, texture2d<float> position_texture)
{
    const float bias_value = 0.4;
    
    float3 offset_position = position_texture.sample(linear_sampler, texcoord + uv).xyz;
    float3 offset_direction = offset_position - position;
    
    float ao_distance = length(offset_direction);
    float intensity = saturate((dot(normalize(offset_direction), normal) - bias_value) * (1.f / (1.f + ao_distance)));
    float attenuation = smoothstep(range, range * 0.5, ao_distance);
    
    return intensity * attenuation;
}

fragment half4 fragment_shader_ss_ssao(common_v_output_t in [[stage_in]],
                                       constant ssao_u_input_t& ssao_uniforms [[buffer(1)]],
                                       texture2d<half> normal_texture [[texture(0)]],
                                       texture2d<float> position_texture [[texture(1)]],
                                       texture2d<half> random_texture [[texture(2)]])
{
    float range = .8f;
    const float2 directions[4] = {float2(1, 0),float2(-1, 0), float2(0, 1),float2(0, -1)};
    float3 normal = normalize((float3)normal_texture.sample(linear_sampler, in.texcoord).xyz);
    float4 position = position_texture.sample(linear_sampler, in.texcoord);
    float4 position_in_view_space = float4(position.x, position.y, position.z, 1.0);
    position_in_view_space = ssao_uniforms.mat_v * position_in_view_space;
    float2 random = (float2)random_texture.sample(repeat_sampler, in.texcoord * 4.f).xz;
    
    float ao = 0.0;
    const float radius = range / position_in_view_space.z;

    const float sin_45 = 0.707;
    int iterations = 4;
    for (int i = 0; i < iterations; ++i)
    {
        float2 coord1 = reflect(directions[i], random);
        float2 coord2 = float2(coord1.x * sin_45 - coord1.y * sin_45,
                               coord1.x * sin_45 + coord1.y * sin_45);
        coord1 *= radius;
        coord2 *= radius;
        
        ao += do_ambient_occlusion(in.texcoord, coord1 * 0.25, position.xyz, normal, range, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord2 * 0.5, position.xyz, normal, range, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord1 * 0.75, position.xyz, normal, range, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord2, position.xyz, normal, range, position_texture);
    }
    
    ao /= (float)iterations * 4.f;
    const float ao_power = 16.f;
    return half4(pow(1.f - saturate(ao), ao_power));
}
