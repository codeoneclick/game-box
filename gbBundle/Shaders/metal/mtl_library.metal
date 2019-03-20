//
//  File.metal
//  gbDemo
//
//  Created by serhii.s on 3/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

constexpr sampler trilinear_sampler(filter::linear, mip_filter::linear);

typedef struct
{
    float3 position [[ attribute(0) ]];
    float2 texcoord [[ attribute(1) ]];
    float4 normal [[ attribute(2) ]];
    uchar4 tangent [[ attribute(3) ]];
    uchar4 color [[ attribute(4) ]];
} common_v_input_t;

typedef struct
{
    float4 position [[position]];
    float2 texcoord;
    float4 view_space_position;
    float3 normal;
    float3 tangent;
    float3 binormal;
    float4 color;
} common_v_output_t;

typedef struct
{
    half4 color [[color(0)]];
    half4 normal [[color(1)]];
    float depth [[color(2)]];
} g_buffer_output_t;

typedef struct
{
    float4x4 mat_m;
    float4x4 mat_v;
    float4x4 mat_p;
} __attribute__ ((aligned(256))) common_u_input_t;

typedef struct
{
    float4x4 mat_i_p;
    float4 frame_size;
    float4 center_radius;
    float4 color;
} __attribute__ ((aligned(256))) omni_deferred_light_source_u_input_t;

float4x4 get_mat_m(common_u_input_t uniforms)
{
    return uniforms.mat_m;
}

float4x4 get_mat_v(common_u_input_t uniforms)
{
    return uniforms.mat_v;
}

float4x4 get_mat_p(common_u_input_t uniforms)
{
    return uniforms.mat_p;
}

float4x4 get_mat_mvp(common_u_input_t uniforms)
{
    return get_mat_p(uniforms) *  get_mat_v(uniforms) *  get_mat_m(uniforms);
}

//

vertex common_v_output_t vertex_shader_ss_deferred_lighting(uint v_index[[vertex_id]],
                                                            device common_v_input_t* vertices [[ buffer(0) ]],
                                                            constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(vertices[v_index].position), 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = vertices[v_index].texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_deferred_lighting(common_v_output_t in [[stage_in]],
                                                    texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = (float4)diffuse_texture.sample(trilinear_sampler, in.texcoord);
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_compose_ui(uint v_index[[vertex_id]],
                                                     device common_v_input_t* vertices [[ buffer(0) ]],
                                                     constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(vertices[v_index].position), 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = (float2)vertices[v_index].texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_compose_ui(common_v_output_t in [[stage_in]],
                                             texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = (float4)diffuse_texture.sample(trilinear_sampler, in.texcoord);
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_screen_quad(common_v_input_t in [[stage_in]],
                                                   constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_screen_quad(common_v_output_t in [[stage_in]],
                                           texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = (float4)diffuse_texture.sample(trilinear_sampler, in.texcoord);
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_shape_3d(common_v_input_t in [[stage_in]],
                                                constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.view_space_position = get_mat_m(uniforms) * in_position;
    out.texcoord = in.texcoord;
    out.normal = (get_mat_m(uniforms) * in.normal).xyz;
    
    return out;
}

fragment g_buffer_output_t fragment_shader_shape_3d(common_v_output_t in [[stage_in]],
                                        texture2d<half> diffuse_texture [[texture(0)]])
{
    g_buffer_output_t out;
    half4 color = diffuse_texture.sample(trilinear_sampler, in.texcoord);
    out.color = color;
    out.normal = half4(half3(normalize(in.normal)), 1.0);
    out.depth = in.position.z;
    
    return out;
}

//

vertex common_v_output_t vertex_shader_shape_3d_forward_normal_ws(uint v_index[[vertex_id]],
                                                                  device common_v_input_t* vertices [[ buffer(0) ]],
                                                                  constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(vertices[v_index].position), 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = (float2)vertices[v_index].texcoord;
    
    return out;
}

fragment half4 fragment_shader_shape_3d_forward_normal_ws(common_v_output_t in [[stage_in]],
                                                          texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = (float4)diffuse_texture.sample(trilinear_sampler, in.texcoord);
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_omni_deferred_light_source(common_v_input_t in [[stage_in]],
                                                                  constant common_u_input_t& mvp_uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(mvp_uniforms);
    out.position = mvp * in_position;
    //out.view_space_position = get_mat_m(mvp_uniforms) * in_position;
    //out.texcoord = in.texcoord;
    // out.normal = (get_mat_m(mvp_uniforms) * in.normal).xyz;
    // out.color = custom_uniforms.color;
    
    return out;
}

fragment g_buffer_output_t fragment_shader_omni_deferred_light_source(common_v_output_t in [[stage_in]],
                                                                      constant omni_deferred_light_source_u_input_t& custom_uniforms [[buffer(0)]],
                                                                      texture2d<half> normal_texture [[texture(0)]],
                                                                      texture2d<float> depth_texture [[texture(1)]])
{
    g_buffer_output_t out;
    
    uint2 screen_space_position = uint2(in.position.xy);
    
    half4 normal = normal_texture.read(screen_space_position);
    float depth = depth_texture.read(screen_space_position).x;
    
    float2 normalized_screen_position;
    normalized_screen_position.x = 2.0  * ((screen_space_position.x/(float)custom_uniforms.frame_size.x) - 0.5);
    normalized_screen_position.y = 2.0  * ((1.0 - (screen_space_position.y/(float)custom_uniforms.frame_size.y)) - 0.5);
    
    float4 ndc_fragment_position = float4 (normalized_screen_position.x, normalized_screen_position.y, depth, 1.0);
    ndc_fragment_position = custom_uniforms.mat_i_p * ndc_fragment_position;
    float3 view_space_fragment_position = ndc_fragment_position.xyz / ndc_fragment_position.w;
    
    float3 light_direction = custom_uniforms.center_radius.xyz - view_space_fragment_position.xyz;
    float light_distance = length(light_direction);
    float light_radius = custom_uniforms.center_radius.w;
    float attenuation = 1.0 - (light_distance / light_radius);
    light_direction = normalize(light_direction);
    
    float4 color = attenuation * max(dot(float3(normal.xyz), light_direction), 0.0) * custom_uniforms.color;
    
    //float3 view_space_fragment_position = in.view_space_position.xyz * (depth / in.view_space_position.z);
    
    out.color = (half4)color;
    out.normal = normal;
    return out;
}




