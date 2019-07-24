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

float do_ambient_occlusion(float2 texcoord, half2 uv, float3 position, half3 normal, float range, texture2d<float> position_texture)
{
    const half bias_value = .4f;
    
    float3 offset_position = position_texture.sample(repeat_sampler, texcoord + (float2)uv).xyz;
    float3 ao_direction = offset_position - position;
    
    float ao_distance = length(ao_direction);
    half intensity = saturate((dot(ao_direction / ao_distance, (float3)normal) - bias_value) * (1.f / (1.f + ao_distance)));
    half attenuation = smoothstep(range, range * .5f, ao_distance);
    
    return intensity * attenuation;
}

fragment half4 fragment_shader_ss_ssao(common_v_output_t in [[stage_in]],
                                       constant ssao_u_input_t& ssao_uniforms [[buffer(1)]],
                                       texture2d<half> normal_texture [[texture(0)]],
                                       texture2d<float> position_texture [[texture(1)]],
                                       texture2d<half> random_texture [[texture(2)]])
{
    half range = .8f;
    const half2 directions[4] = {half2(1.f, 0.f), half2(-1.f, 0.f), half2(0.f, 1.f), half2(0.f, -1.f)};
    half3 normal = normal_texture.sample(linear_sampler, in.texcoord).xyz;
    float4 position = position_texture.sample(linear_sampler, in.texcoord);
    float4 position_in_view_space = float4(position.x, position.y, position.z, 1.0);
    position_in_view_space = ssao_uniforms.mat_v * position_in_view_space;
    half2 random = random_texture.sample(repeat_sampler, in.texcoord * 4.f).xz;
    
    float ao = 0.0;
    const half radius = range / position_in_view_space.z;

    const half sin_45 = 0.707;
    int iterations = 4;
    for (int i = 0; i < iterations; ++i)
    {
        half2 coord1 = reflect(directions[i], random);
        half2 coord2 = half2(coord1.x * sin_45 - coord1.y * sin_45,
                             coord1.x * sin_45 + coord1.y * sin_45);
        coord1 *= radius;
        coord2 *= radius;
        
        ao += do_ambient_occlusion(in.texcoord, coord1 * 0.25, position.xyz, normal, (float)range, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord2 * 0.5, position.xyz, normal, (float)range, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord1 * 0.75, position.xyz, normal, (float)range, position_texture);
        ao += do_ambient_occlusion(in.texcoord, coord2, position.xyz, normal, (float)range, position_texture);
    }
    
    ao /= (float)iterations * 4.f;
    return half4(1.f - ao);
}
