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

fragment half4 fragment_shader_ss_ssao(common_v_output_t in [[stage_in]],
                                       constant ssao_u_input_t& ssao_uniforms [[buffer(1)]],
                                       texture2d<half> normal_texture [[texture(0)]],
                                       texture2d<float> position_texture [[texture(1)]],
                                       texture2d<half> random_texture [[texture(2)]])
{
    const half range = .8f;
    const half half_range = range * .5f;
    const half bias_value = .4f;
    const half2 directions[4] = { half2(1.f, 0.f), half2(-1.f, 0.f), half2(0.f, 1.f), half2(0.f, -1.f) };
    const half4 normal = normal_texture.sample(linear_sampler, in.texcoord);
    const float4 position = position_texture.sample(linear_sampler, in.texcoord);
    const float4 position_in_view_space = ssao_uniforms.mat_v * float4(position.x, position.y, position.z, 1.0);
    const half2 random = random_texture.sample(repeat_sampler, in.texcoord * 4.f).xz;
    
    half ao = .0f;
    const half radius = range / position_in_view_space.z;

    const half sin_45 = .707f;
    const int iterations = 4;
    
    float2 texcoord_offset_1;
    float2 texcoord_offset_2;
    
    float4 position_with_offset;
    if (normal.w > 0.f)
    {
        for (int i = 0; i < iterations; ++i)
        {
            texcoord_offset_1 = static_cast<float2>(reflect(directions[i], random));
            texcoord_offset_2.x = texcoord_offset_1.x * sin_45 - texcoord_offset_1.y * sin_45;
            texcoord_offset_2.y = texcoord_offset_1.x * sin_45 + texcoord_offset_1.y * sin_45;
            
            texcoord_offset_1 *= radius;
            texcoord_offset_2 *= radius;
            
            position_with_offset = position_texture.sample(repeat_sampler, in.texcoord + texcoord_offset_1 * .25f);
            half3 ao_direction = static_cast<half3>(position_with_offset.xyz - position.xyz);
            half ao_distance = length(ao_direction);
            half intensity = saturate((dot(ao_direction / ao_distance, normal.xyz) - bias_value) * (1.f / (1.f + ao_distance)));
            half attenuation = smoothstep(range, half_range, ao_distance);
            ao += intensity * attenuation;
            
            position_with_offset = position_texture.sample(repeat_sampler, in.texcoord + texcoord_offset_2 * .5f);
            ao_direction = static_cast<half3>(position_with_offset.xyz - position.xyz);
            ao_distance = length(ao_direction);
            intensity = saturate((dot(ao_direction / ao_distance, normal.xyz) - bias_value) * (1.f / (1.f + ao_distance)));
            attenuation = smoothstep(range, half_range, ao_distance);
            ao += intensity * attenuation;
            
            position_with_offset = position_texture.sample(repeat_sampler, in.texcoord + texcoord_offset_1 * .75f);
            ao_direction = static_cast<half3>(position_with_offset.xyz - position.xyz);
            ao_distance = length(ao_direction);
            intensity = saturate((dot(ao_direction / ao_distance, normal.xyz) - bias_value) * (1.f / (1.f + ao_distance)));
            attenuation = smoothstep(range, half_range, ao_distance);
            ao += intensity * attenuation;
            
            position_with_offset = position_texture.sample(repeat_sampler, in.texcoord + texcoord_offset_2);
            ao_direction = static_cast<half3>(position_with_offset.xyz - position.xyz);
            ao_distance = length(ao_direction);
            intensity = saturate((dot(ao_direction / ao_distance, normal.xyz) - bias_value) * (1.f / (1.f + ao_distance)));
            attenuation = smoothstep(range, half_range, ao_distance);
            ao += intensity * attenuation;
        }
    }
    
    ao /= static_cast<half>(iterations) * 4.f;
    return half4(1.f - ao);
}
