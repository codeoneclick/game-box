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
    uchar4 normal [[ attribute(2) ]];
    uchar4 tangent [[ attribute(3) ]];
    uchar4 color [[ attribute(4) ]];
} common_v_input_t;

typedef struct
{
    float4 position [[position]];
    float2 texcoord;
    float4 screen_position;
    float3 normal;
    float3 tangent;
    float3 binormal;
    float4 color;
} common_v_output_t;

typedef struct
{
    float4x4 mat_m;
    float4x4 mat_v;
    float4x4 mat_p;
} common_u_input_t;

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

vertex common_v_output_t vertex_shader_screen_quad(uint v_index[[vertex_id]],
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

fragment half4 fragment_shader_screen_quad(common_v_output_t in [[stage_in]],
                                           texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = (float4)diffuse_texture.sample(trilinear_sampler, in.texcoord);
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_shape_3d(common_v_input_t vertices [[ stage_in ]],
                                                constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(vertices.position), 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.screen_position = get_mat_m(uniforms) * in_position;
    out.texcoord = vertices.texcoord;
    
    //uint32_t packed_texcoord = vertices[v_index].texcoord;
    //uint32_t Mask16((1 << 16) - 1);
    //uint32_t A((packed_texcoord >>  0) & Mask16);
    //uint32_t B((packed_texcoord >> 16) & Mask16);
    //return detail::tvec2<detail::float32>(
    //                                      A * 1.0f / 65535.0f,
    //                                      B * 1.0f / 65535.0f);
    
    //out.texcoord = float2(A * 1.0f / 65535.0f, B * 1.0f / 65535.0f);//(float2)vertices[v_index].texcoord;
    //out.normal = (get_mat_m(uniforms) * (float4)vertices[v_index].normal).xyz;
    
    return out;
}

fragment half4 fragment_shader_shape_3d(common_v_output_t in [[stage_in]],
                                        texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = (float4)diffuse_texture.sample(trilinear_sampler, in.texcoord);
    
    //float3 light_position = float3(0.0, 128.0, 0.0);
    //float3 light_direction = (light_position - in.screen_position.xyz);
    // color = color * max(dot(normalize(in.normal), normalize(light_direction)), 0.0);
    
    return (half4)color; //(half4)float4(normalize(in.normal) * 0.5 + 0.5, 1.0);
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

vertex common_v_output_t vertex_shader_omni_deferred_light_source(uint v_index[[vertex_id]],
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

fragment half4 fragment_shader_omni_deferred_light_source(common_v_output_t in [[stage_in]],
                                                          texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = (float4)diffuse_texture.sample(trilinear_sampler, in.texcoord);
    
    return half4(color);
}




