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
    
    float3 offset_position = position_texture.sample(repeat_sampler, texcoord + uv).xyz;
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
    float2 random = (float2)random_texture.sample(repeat_sampler, in.texcoord * 16.f).xz;
    
    float ao = 0.0;
    float radius = range / position_in_view_space.z;

    float sin_45 = 0.707;
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

vertex common_v_output_t vertex_shader_ssao(common_v_input_t in [[stage_in]],
                                               constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ssao(common_v_output_t in [[stage_in]],
                                    constant ssao_u_input_t& ssao_uniforms [[buffer(1)]],
                                    texture2d<half> normal_texture [[texture(0)]],
                                    texture2d<float> position_texture [[texture(1)]],
                                    texture2d<half> random_texture [[texture(2)]])
{
    const float ao_bias = .05f;
    const float sample_radius = 2.5f;
    const int samples_num = 16;
    float3 samples[samples_num] =
    {
        float3(0.355512,    -0.709318,  -0.102371),
        float3(0.534186,    0.71511,    -0.115167),
        float3(-0.87866,    0.157139,   -0.115167),
        float3(0.140679,    -0.475516,  -0.0639818),
        float3(-0.0796121,  0.158842,   -0.677075),
        float3(-0.0759516,  -0.101676,  -0.483625),
        float3(0.12493,     -0.0223423, -0.483625),
        float3(-0.0720074,  0.243395,   -0.967251),
        float3(-0.207641,   0.414286,   0.187755),
        float3(-0.277332,   -0.371262,  0.187755),
        float3(0.63864,     -0.114214,  0.262857),
        float3(-0.184051,   0.622119,   0.262857),
        float3(0.110007,    -0.219486,  0.435574),
        float3(0.235085,    0.314707,   0.696918),
        float3(-0.290012,   0.0518654,  0.522688),
        float3(0.0975089,   -0.329594,  0.609803)
    };
    
    float4 position = position_texture.sample(linear_sampler, in.texcoord);
    float depth = position.w;
    
    float3 origin = depth;
    
    float4 normal = (float4)normal_texture.sample(linear_sampler, in.texcoord);
    float3 random_normal = (float3)random_texture.sample(repeat_sampler, in.texcoord * 16.f).xyz;
    
    float3 tangent = normalize(random_normal - normal.xyz * dot(random_normal, normal.xyz));
    float3 bitangent = cross(normal.xyz, tangent);
    float3x3 tbn = float3x3(tangent, bitangent, normal.xyz);
    
    // IN.TexCoord.x += 1.0/1600.0;
    // IN.TexCoord.y += 1.0/1200.0;
    //float3 se = depth * normalize(ssao_uniforms.camera_direction.xyz);
    float ao = 0.f;
    
    for (int i = 0; i < samples_num; i++)
    {
        float3 sample = tbn * samples[i];
        sample = origin + sample * sample_radius;
        
        float4 offset = float4(sample, 1.f);
        offset = ssao_uniforms.mat_p * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * .5f + .5f;
        
        float sample_depth = position_texture.sample(linear_sampler, offset.xy).w;
        
        float range_check = abs(origin.z - sample_depth) < sample_radius ? 1.0 : 0.0;
        ao += (sample_depth <= sample.z ? 1.0 : 0.0) * range_check;
        
        //float range_check = smoothstep(0.f, 1.f, sample_radius / abs(depth - sample_depth));
        //ao += (sample_depth >= sample.z + ao_bias ? 1.f : 0.f) * range_check;
        
        //float3 ray = reflect(samples[i].xyz, random_normal) * sample_radius;
        //float4 sample = float4(se + ray, 1.f);
        //float4 ss = sample * ssao_uniforms.mat_p;
        
        //float2 sample_texcoord = 0.5f * ss.xy / ss.w + float2(.5f, .5f);
        
        // sampleTexCoord.x += 1.0/1600.0;
        // sampleTexCoord.y += 1.0/1200.0;
        //float sample_depth = position_texture.sample(linear_sampler, sample_texcoord).w;
        
        //if (sample_depth == 1.0)
        //{
        //    ao += 1.f;
        //}
        //else
        //{
        //    float occlusion = distance_scale * max(sample_depth - depth, .0f);
        //    ao += 1.f / (1.f + occlusion * occlusion * .1f);
        //}
    }
    
    return half4(half3(1.f - ao / (float)samples_num), 1.f);
}
