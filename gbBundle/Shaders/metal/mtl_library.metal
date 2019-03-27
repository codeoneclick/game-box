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
    float4 tangent [[ attribute(3) ]];
    float4 color [[ attribute(4) ]];
} common_v_input_t;

typedef struct
{
    float4 position [[position]];
    float2 texcoord;
    float4 view_space_position;
    float3 normal;
    float3 tangent;
    float3 bitangent;
    float4 color;
} common_v_output_t;

typedef struct
{
    half4 color [[color(0)]];
    half4 normal [[color(1)]];
    float4 position [[color(2)]];
} g_buffer_output_t;

typedef struct
{
    float4x4 mat_m;
    float4x4 mat_v;
    float4x4 mat_p;
} __attribute__ ((aligned(256))) common_u_input_t;

typedef struct
{
    float4x4 mat_i_vp;
    float4 frame_size;
    float4 center_ray_length;
    float4 color;
    float4 camera_position;
} __attribute__ ((aligned(256))) deferred_light_source_u_input_t;

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

float4 blur_texture(texture2d<half> texture,
                    float2 texcoord, float2 texel_size, float scale)
{
    // float toon_thresholds[2] = { 0.8, 0.4 };
    // float toon_brightness_levels[3] = { 1.3, 0.9, 0.5 };
    
    float pixel_offsets[5];
    pixel_offsets[0] = 0.0;
    pixel_offsets[1] = 1.0;
    pixel_offsets[2] = 2.0;
    pixel_offsets[3] = 3.0;
    pixel_offsets[4] = 4.0;
    
    float weights[5];
    weights[0] = 0.2270270270;
    weights[1] = 0.1945945946;
    weights[2] = 0.1216216216;
    weights[3] = 0.0540540541;
    weights[4] = 0.0162162162;
    
    float4 color = (float4)texture.sample(trilinear_sampler, texcoord) * weights[0];
    for (int i = 1; i < 5; i++)
    {
        color += (float4)texture.sample(trilinear_sampler, texcoord + pixel_offsets[i] * scale * texel_size) * weights[i];
        color += (float4)texture.sample(trilinear_sampler, texcoord - pixel_offsets[i] * scale * texel_size) * weights[i];
    }
    return color;
}

//

vertex common_v_output_t vertex_shader_ss_deferred_lighting(common_v_input_t in [[stage_in]],
                                                            constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment float4 fragment_shader_ss_deferred_lighting(common_v_output_t in [[stage_in]],
                                                    texture2d<half> color_texture [[texture(0)]],
                                                    texture2d<half> lighting_texture [[texture(1)]])
{
    float4 color = (float4)color_texture.sample(trilinear_sampler, in.texcoord);
    float4 lighting = (float4)lighting_texture.sample(trilinear_sampler, in.texcoord);
    color.rbg = color.rbg * max(lighting.rbg, float3(0.05));
    color.a = 1.0;
    return color;
}

//

vertex common_v_output_t vertex_shader_ss_bright(common_v_input_t in [[stage_in]],
                                                 constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_bright(common_v_output_t in [[stage_in]],
                                         texture2d<half> color_texture [[texture(0)]])
{
    const float threshold = 0.25;
    float4 color = (float4)color_texture.sample(trilinear_sampler, in.texcoord);
    color = saturate((color - threshold) / (1.0 - threshold));
    color.a = 1.0;
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_gaussian_blur_h(common_v_input_t in [[stage_in]],
                                                          constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_gaussian_blur_h(common_v_output_t in [[stage_in]],
                                                  texture2d<half> bright_texture [[texture(0)]])
{
    float4 color = blur_texture(bright_texture, in.texcoord,
                                float2(1.0 / 1024, 0.0), 5.0);
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_gaussian_blur_v(common_v_input_t in [[stage_in]],
                                                          constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_gaussian_blur_v(common_v_output_t in [[stage_in]],
                                                  texture2d<half> bright_texture [[texture(0)]])
{
    float4 color = blur_texture(bright_texture, in.texcoord,
                                float2(0.0, 1.0 / 768), 5.0);
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
                                           texture2d<half> color_texture [[texture(0)]])
{
    float4 color = (float4)color_texture.sample(trilinear_sampler, in.texcoord);
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_output(common_v_input_t in [[stage_in]],
                                                 constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

float4 adjust_saturation(float4 color, float saturation)
{
    float grey = dot(color.rbg, float3(0.3, 0.59, 0.11));
    return mix(float4(grey), color, saturation);
}

fragment half4 fragment_shader_ss_output(common_v_output_t in [[stage_in]],
                                         texture2d<half> color_texture [[texture(0)]],
                                         texture2d<half> bright_texture [[texture(1)]])
{
    const float bloom_intensity = 1.25;
    const float original_intensity = 1.0;
    const float bloom_saturation = 1.0;
    const float original_saturation = 1.0;
    
    float4 bloom = (float4)bright_texture.sample(trilinear_sampler, in.texcoord);
    float4 color = (float4)color_texture.sample(trilinear_sampler, in.texcoord);
    
    bloom = adjust_saturation(bloom, bloom_saturation) * bloom_intensity;
    color = adjust_saturation(color, original_saturation) * original_intensity;
    
    color *= (1.0 - saturate(bloom));
    color = color + bloom;
    
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
    
    float4x4 mat_m = get_mat_m(uniforms);
    float3x3 mat_n;
    mat_n[0].xyz = mat_m[0].xyz;
    mat_n[1].xyz = mat_m[1].xyz;
    mat_n[2].xyz = mat_m[2].xyz;
    
    float3 normal = normalize(mat_n * in.normal.xyz);
    out.normal = normal;
    float3 tangent = normalize(mat_n * in.tangent.xyz);
    out.tangent = tangent;
    float3 bitangent = normalize(cross(-normal, tangent));
    out.bitangent = bitangent;
    
    return out;
}

fragment g_buffer_output_t fragment_shader_shape_3d(common_v_output_t in [[stage_in]],
                                                    texture2d<half> diffuse_texture [[texture(0)]],
                                                    texture2d<half> normal_texture [[texture(1)]])
{
    g_buffer_output_t out;
    half4 color = diffuse_texture.sample(trilinear_sampler, in.texcoord);
    out.color = color;
    
    //float3x3 mat_tbn = float3x3(in.tangent, in.bitangent, in.normal);
    //float3 normal_color = (float3)normal_texture.sample(trilinear_sampler, in.texcoord).rgb * 2.0 - 1.0;
    //float3 normal_tbn = mat_tbn * normal_color;
    
    out.normal = half4(half3(in.normal), 1.0);
    out.position = in.view_space_position;
    
    return out;
}

//

vertex common_v_output_t vertex_shader_particle_emitter(common_v_input_t in [[stage_in]],
                                                        constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_p(uniforms) * get_mat_v(uniforms) * float4x4(1.0);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    out.normal = (get_mat_m(uniforms) * in.normal).xyz;
    
    return out;
}

fragment g_buffer_output_t fragment_shader_particle_emitter(common_v_output_t in [[stage_in]],
                                                            texture2d<half> diffuse_texture [[texture(0)]])
{
    g_buffer_output_t out;
    half4 color = diffuse_texture.sample(trilinear_sampler, in.texcoord);
    out.color = color;
    out.normal = half4(0.0, 1.0, 0.0, 1.0);
    out.position = in.view_space_position;
    
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
    
    return out;
}

fragment half4 fragment_shader_omni_deferred_light_source(common_v_output_t in [[stage_in]],
                                                          constant deferred_light_source_u_input_t& custom_uniforms [[buffer(0)]],
                                                          texture2d<half> normal_texture [[texture(0)]],
                                                          texture2d<float> position_texture [[texture(1)]])
{
    uint2 screen_space_position = uint2(in.position.xy);
    
    half4 normal = normal_texture.read(screen_space_position);
    float4 position = position_texture.read(screen_space_position);
    
    float light_range = custom_uniforms.center_ray_length.w;
    float3 light_vector = custom_uniforms.center_ray_length.xyz - position.xyz;
    float3 light_direction = normalize(light_vector);
    float light_distance = length(light_vector);
    
    float no_depth_fix = step(0.0, light_range - light_distance);
    float attenuation = 1.0 / ((((light_distance / (1.0 - ((light_distance / light_range) *
                                                           (light_distance / light_range)))) / light_range) + 1.0) *
                               (((light_distance / ( 1.0 - ((light_distance / light_range) *
                                                            (light_distance / light_range)))) / light_range) + 1.0));
    attenuation *= no_depth_fix;
    
    float intensity = saturate(dot(light_direction, float3(normal.xyz)));
    
    float4 color = intensity * custom_uniforms.color;
    
    float3 reflection_vector = normalize(reflect(-light_direction, float3(normal.xyz)));
    float3 camera_direction = normalize(custom_uniforms.camera_position.xyz - position.xyz);
    
    float specular_power = 4.0;
    float specular_intensity = 200.0;
    float4 specular = saturate(custom_uniforms.color * specular_intensity * pow(saturate(dot(reflection_vector, camera_direction)), specular_power));
    
    color = attenuation * (color + specular);
    return (half4)color;
}




