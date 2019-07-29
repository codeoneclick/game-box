//
//  File.metal
//  gbDemo
//
//  Created by serhii.s on 3/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <metal_stdlib>
#include <simd/simd.h>
#include "mtl_common.h"

using namespace metal;



//

float4 blur_texture(texture2d<half> texture,
                    float2 texcoord, float2 texel_size, float scale)
{
    const float pixel_offsets[5] = {0.f, 1.f, 2.f, 3.f, 4.f};
    const float weights[5] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};
    
    float4 color = (float4)texture.sample(linear_sampler, texcoord) * weights[0];
    for (int i = 1; i < 5; i++)
    {
        color += (float4)texture.sample(linear_sampler, texcoord + pixel_offsets[i] * scale * texel_size) * weights[i];
        color += (float4)texture.sample(linear_sampler, texcoord - pixel_offsets[i] * scale * texel_size) * weights[i];
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
                                                     texture2d<float> opaque_color_texture [[texture(0)]],
                                                     texture2d<float> transparent_color_texture [[texture(1)]],
                                                     texture2d<half> transparent_alpha_texture [[texture(2)]],
                                                     texture2d<half> lighting_texture [[texture(3)]])
{
    float4 opaque_color = opaque_color_texture.sample(linear_sampler, in.texcoord);
    float4 transparent_color = transparent_color_texture.sample(linear_sampler, in.texcoord);
    float transparent_alpha = (float)transparent_alpha_texture.sample(linear_sampler, in.texcoord).r;
    transparent_color = float4(transparent_color.rgb / max(transparent_color.a, 1e-5), 0.f);
    float4 color = opaque_color + transparent_color * (1.0 - transparent_alpha);
    
    float4 lighting = (float4)lighting_texture.sample(linear_sampler, in.texcoord);
    color.rbg = color.rbg * max(lighting.rbg, float3(0.05));
    return color;
}


//

vertex common_v_output_t vertex_shader_ss_hdr(common_v_input_t in [[stage_in]],
                                                 constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_hdr(common_v_output_t in [[stage_in]],
                                         texture2d<half> color_texture [[texture(0)]])
{
    float3 hdr_color = (float3)color_texture.sample(linear_sampler, in.texcoord).rgb;
    const float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);
    float l_scale = dot(hdr_color, LUM_FACTOR);
    hdr_color *= l_scale;
    return half4((half3)hdr_color, 1.0);;
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
                                                  texture2d<half> hdr_texture [[texture(0)]])
{
    float width = hdr_texture.get_width();
    float4 color = blur_texture(hdr_texture, in.texcoord,
                                float2(1.f / width, .0f), 4.f);
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
                                                  texture2d<half> hdr_texture [[texture(0)]])
{
    float height = hdr_texture.get_height();
    float4 color = blur_texture(hdr_texture, in.texcoord,
                                float2(0.f, 1.f / height), 4.f);
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
    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_compose(common_v_input_t in [[stage_in]],
                                                 constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_compose(common_v_output_t in [[stage_in]],
                                         constant ss_output_u_input_t& uniforms [[ buffer(1) ]],
                                         texture2d<half> color_texture [[texture(0)]],
                                         texture2d<half> ao_texture [[texture(1)]],
                                         texture2d<half> emissive_texture [[texture(2)]],
                                         texture2d<half> mask_texture [[texture(3)]],
                                         texture2d<half> bloom_texture [[texture(4)]])
{
    const float bloom_intensity = 2.f;
    const float original_intensity = 2.f;
    const float bloom_saturation = .75f;
    const float original_saturation = 1.f;
    
    float2 motion_direction = uniforms.motion_direction.xy * 3.f;
    float motion_blur_power = uniforms.motion_direction.z;

    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    if (motion_blur_power > 0.f)
    {
        float4 motion_color = float4(0.f, 0.f, 0.f, 1.f);
        float2 texcoord = in.texcoord;
        for (int i = 0; i < 4; i++)
        {
            texcoord.x = texcoord.x - 0.001 * motion_direction.x * motion_blur_power;
            texcoord.y = texcoord.y - 0.001 * motion_direction.y * motion_blur_power;
            
            float4 mask_color = (float4)mask_texture.sample(linear_sampler, texcoord);
            
            motion_color += mix((float4)color_texture.sample(linear_sampler, texcoord), color, 1.0f - mask_color.r);
        }
        motion_color /= 4;
        float4 mask_color = (float4)mask_texture.sample(linear_sampler, in.texcoord);
        color = mix(motion_color, color, mask_color.r);
    }
    
    float4 bloom = (float4)bloom_texture.sample(linear_sampler, in.texcoord);
    float ao = (float)ao_texture.sample(linear_sampler, in.texcoord).r;
    float4 emissive = (float4)emissive_texture.sample(linear_sampler, in.texcoord);
    
    bloom = adjust_saturation(bloom, bloom_saturation) * bloom_intensity;
    color = adjust_saturation(color, original_saturation) * original_intensity;
    color *= pow(ao, 16);
    color += bloom;
    color += emissive;
    
    if (uniforms.parameters_01.y > 0.f)
    {
        float3 sepia_color;
        sepia_color.r = (color.r * 0.393) + (color.g * 0.769) + (color.b * 0.189);
        sepia_color.g = (color.r * 0.349) + (color.g * 0.686) + (color.b * 0.168);
        sepia_color.b = (color.r * 0.272) + (color.g * 0.534) + (color.b * 0.131);
        float4 mask_color = (float4)mask_texture.sample(linear_sampler, in.texcoord);
        if (mask_color.r != 1.f)
        {
            color.rgb = mix(color.rbg, sepia_color, (1.f - mask_color.r) * uniforms.parameters_01.y);
        }
    }
    
    float vignetting_size = uniforms.parameters_01.x;
    float edge = 0.1;
    float2 direction = max(abs(in.texcoord * 2.0 - 1.0) * (1.0 + edge) - edge + vignetting_size, float2(0.f));
    float vignetting_value = dot(direction, direction);
    vignetting_value = 1.f - vignetting_value / (1.f + vignetting_value);
    color.rgb += (1.f - vignetting_value) * uniforms.vignetting_color.rbg;
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_compose_ui(common_v_input_t in [[stage_in]],
                                                     constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_compose_ui(common_v_output_t in [[stage_in]],
                                             texture2d<half> color_texture [[texture(0)]],
                                             texture2d<half> ui_texture [[texture(1)]])
{
    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    float4 ui = (float4)ui_texture.sample(linear_sampler, in.texcoord);
    color = mix(color, ui, ui.a);
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

fragment half4 fragment_shader_ss_output(common_v_output_t in [[stage_in]],
                                         texture2d<half> color_texture [[texture(0)]])
{
    float4 color = (float4)color_texture.sample(linear_sampler, in.texcoord);
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_tv(common_v_input_t in [[stage_in]],
                                             constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_tv(common_v_output_t in [[stage_in]],
                                     constant ss_tv_u_input_t& uniforms [[ buffer(1) ]],
                                     texture2d<half> color_texture [[texture(0)]])
{
    float4 color;
    float2 uv = in.texcoord;
    
    if (uniforms.enabled > 0.0)
    {
        float time = uniforms.time * 0.001;
        
        float distance_to_center = length(uv - float2(0.5, 0.5));
        float blur = 0.0;
        blur = (1.0 + sin(time * 6.0)) * 0.5;
        blur *= 1.0 + sin(time * 16.0) * 0.5;
        blur = pow(blur, 3.0);
        blur *= 0.05;
        blur *= distance_to_center;
        
        color.r = (float)color_texture.sample(linear_sampler, float2(uv.x + blur,uv.y)).r;
        color.g = (float)color_texture.sample(linear_sampler, uv).g;
        color.b = (float)color_texture.sample(linear_sampler, float2(uv.x - blur,uv.y)).b;
        float scanline = sin(uv.y * 768.0) * 0.04;
        color -= scanline;
        color *= 1.0 - distance_to_center * 0.5;
        color.a = 1.0;
    }
    else
    {
        color = (float4)color_texture.sample(linear_sampler, uv);
    }
    
    return half4(color);
}

//

vertex common_v_output_t vertex_shader_ss_cross_fade(common_v_input_t in [[stage_in]],
                                                     constant common_u_input_t& uniforms [[ buffer(1) ]])
{
    common_v_output_t out;
    
    float4 in_position = float4(float3(in.position), 1.0);
    out.position = in_position;
    out.texcoord = (float2)in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_ss_cross_fade(common_v_output_t in [[stage_in]],
                                             constant ss_cross_fade_u_input_t& uniforms [[ buffer(1) ]],
                                             texture2d<half> color_texture [[texture(0)]])
{
    float4 color = float4(0.f, 0.f, 0.f, 1.0);
    float2 uv = in.texcoord;
    
    if (uniforms.enabled > 0.0)
    {
        float progress = uniforms.progress;
        float2 center = float2(linear_ease(0.5, 0.0, 1.0, progress),0.5);
        float strength = sinusoidal_ease_in_out(0.0, 1.0, 1.0, progress);
        
        float max_weight = 0.0;
        float2 to_center = center - uv;
        float offset = rand(float2(uv.y * progress));
        
        for (float i = 0.0; i <= 20.0; i++)
        {
            float percent = (i + offset) / 20.0;
            float weight = 1.0 * (percent - percent * percent);
            color += mix((float4)color_texture.sample(linear_sampler, uv + to_center * percent * strength), float4(0.0, 0.0, 0.0, 1.0), progress) * weight;
            max_weight += weight;
        }
        
        color = color / max_weight;
        color.a = 1.0;
    }
    else
    {
        color = (float4)color_texture.sample(linear_sampler, uv);
    }
    
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
    
    float4x4 mat_n = get_mat_n(uniforms);
    float3 normal = normalize((mat_n * in.normal).xyz);
    out.normal = normal;
    float3 tangent = normalize(mat_n * in.tangent).xyz;
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
    float4 color = (float4)diffuse_texture.sample(repeat_sampler, in.texcoord);
    out.color = color;
    
#if defined(TBN)
    
    float3x3 mat_tbn = float3x3(in.tangent, in.bitangent, in.normal);
    float3 normal_color = (float3)normal_texture.sample(repeat_sampler, in.texcoord).rgb * 2.0 - 1.0;
    
    out.normal = half4(half3(in.normal), 1.0);
    float3 normal_tbn = mat_tbn * normal_color;
    
#else
    
    out.normal = half4(half3(in.normal), 0.0);
    
#endif
    
    out.position = in.view_space_position;
    out.position.w = in.position.z;
    
    return out;
}

//

vertex reflect_v_output_t vertex_shader_shape_3d_reflect(common_v_input_t in [[stage_in]],
                                                         constant common_u_input_t& uniforms [[buffer(1)]],
                                                         constant reflect_u_input_t& reflect_uniforms [[buffer(2)]])
{
    reflect_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.position_m = get_mat_m(uniforms) * in_position;
    out.texcoord = in.texcoord;
    
    float4x4 mat_n = get_mat_n(uniforms);
    float3 normal = normalize((mat_n * in.normal).xyz);
    out.normal = normal;
    
    float3 normal_vm = normalize((get_mat_v(uniforms) * get_mat_m(uniforms) * in.normal).xyz);
    out.normal_vm = normal_vm;
    float3 position_vm = (get_mat_v(uniforms) * get_mat_m(uniforms) * in_position).xyz;
    out.camera_direction_vm = normalize(-position_vm);
    
    float3 camera_direction = normalize(out.position_m.xyz - reflect_uniforms.camera_position.xyz);
    out.cube_texcoord = reflect(camera_direction, normal);

    return out;
}

fragment g_buffer_output_t fragment_shader_shape_3d_reflect(reflect_v_output_t in [[stage_in]],
                                                            constant colorization_u_input_t& colorization_uniforms [[buffer(0)]],
                                                            texture2d<half> diffuse_texture [[texture(0)]],
                                                            texturecube<half> reflection_texture [[texture(1)]],
                                                            texture2d<half> color_body_mask_texture [[texture(2)]],
                                                            texture2d<half> color_windows_mask_texture [[texture(3)]])
{
    g_buffer_output_t out;
    
    float4 reflect_color = static_cast<float4>(reflection_texture.sample(linear_sampler, in.cube_texcoord));
    float4 color = static_cast<float4>(diffuse_texture.sample(linear_sampler, in.texcoord));
    float3 body_color = colorization_uniforms.body_color.rgb;
    float4 color_body_mask = static_cast<float4>(color_body_mask_texture.sample(linear_sampler, in.texcoord));
    color.rgb = mix(color.rgb, body_color, color_body_mask.r);
    float4 color_windows_mask = static_cast<float4>(color_windows_mask_texture.sample(linear_sampler, in.texcoord));
    float3 windows_color = colorization_uniforms.windows_color.rgb;
    color.rgb = mix(color.rgb, windows_color, color_windows_mask.r);
    out.color = mix(pow(reflect_color, 4), color, 0.66);
    out.color.a = 1.0;
    out.normal = half4(half3(in.normal), 1.f);
    out.position = in.position_m;
    out.position.w = in.position.z;
    
    float rim_power = 8.f;
    float rim = smoothstep(.7f , 1.f , 1.f - max(dot(in.camera_direction_vm, in.normal_vm), 0.f));
    out.color.rgb += rim * colorization_uniforms.body_color.rgb * color_body_mask.r * rim_power;
    
    return out;
}

//

vertex common_v_output_t vertex_shader_particle_emitter_emissive(common_v_input_t in [[stage_in]],
                                                                 constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_p(uniforms) * get_mat_v(uniforms) * float4x4(1.0);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_particle_emitter_emissive(common_v_output_t in [[stage_in]],
                                                         texture2d<half> emissive_texture [[texture(0)]])
{
    half4 color = (half4)in.color;
    color.a = emissive_texture.sample(linear_sampler, in.texcoord).a;
    
    return color;
}

//

vertex common_v_output_t vertex_shader_label_3d_emissive(common_v_input_t in [[stage_in]],
                                                         constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_label_3d_emissive(common_v_output_t in [[stage_in]],
                                                 texture2d<half> emissive_texture [[texture(0)]])
{
    half emissive = emissive_texture.sample(linear_sampler, in.texcoord).r;
    
    return half4(in.color.r, in.color.g, in.color.b, emissive * in.color.a);
}

//

vertex common_v_output_t vertex_shader_label_2d(common_v_input_t in [[stage_in]],
                                                constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_label_2d(common_v_output_t in [[stage_in]],
                                        texture2d<half> emissive_texture [[texture(0)]])
{
    half emissive = emissive_texture.sample(linear_sampler, in.texcoord).r;
    
    return half4(in.color * emissive);
}

//

vertex common_v_output_t vertex_shader_shape_2d_color(common_v_input_t in [[stage_in]],
                                                      constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_shape_2d_color(common_v_output_t in [[stage_in]])
{
    return half4(in.color);
}

//

vertex common_v_output_t vertex_shader_shape_2d_texture_color(common_v_input_t in [[stage_in]],
                                                              constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.color = in.color;
    out.texcoord = in.texcoord;
    
    return out;
}

fragment half4 fragment_shader_shape_2d_texture_color(common_v_output_t in [[stage_in]],
                                                      texture2d<half> diffuse_texture [[texture(0)]])
{
    float4 color = float4(in.color.rbg, 1.0);
    color.a = (float)diffuse_texture.sample(linear_sampler, in.texcoord).a * in.color.a;
    return half4(color);
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

fragment oit_buffer_output_t fragment_shader_particle_emitter(common_v_output_t in [[stage_in]],
                                                              texture2d<half> diffuse_texture [[texture(0)]],
                                                              texture2d<float> position_texture [[texture(1)]])
{
    uint2 screen_space_position = uint2(in.position.xy);
    float4 position = position_texture.read(screen_space_position);
    float depth = position.w;
    
    oit_buffer_output_t out;
    float4 color = float4(in.color.rbg, 1.0);
    color.a = diffuse_texture.sample(linear_sampler, in.texcoord).a * in.color.a;
    float weight = (depth - in.position.w) / 1.66;
    out.color = float4(color.rgb * color.a, color.a * weight);
    out.alpha = half(color.a * weight);
    
    return out;
}

//

vertex common_v_output_t vertex_shader_label(common_v_input_t in [[stage_in]],
                                                        constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    out.normal = (get_mat_m(uniforms) * in.normal).xyz;
    
    return out;
}

fragment oit_buffer_output_t fragment_shader_label(common_v_output_t in [[stage_in]],
                                                   texture2d<half> diffuse_texture [[texture(0)]])
{
    oit_buffer_output_t out;
    float4 color = float4(1.0, 1.0, 1.0, 1.0) * 4.0;
    color.a = diffuse_texture.sample(linear_sampler, in.texcoord).r;
    float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - in.position.z * 0.9, 3.0), 1e-2, 3e3);
    out.color = float4(color.rgb * color.a, color.a) * weight;
    out.alpha = half(color.a);
    
    return out;
}

//

vertex common_v_output_t vertex_shader_trail(common_v_input_t in [[stage_in]],
                                             constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_p(uniforms) * get_mat_v(uniforms) * float4x4(1.0);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    float3 normal = normalize(in.normal.xyz);
    out.normal = normal;
    float3 tangent = normalize(in.tangent.xyz);
    out.tangent = tangent;
    float3 bitangent = normalize(cross(-normal, tangent));
    out.bitangent = bitangent;
    
    return out;
}

fragment oit_buffer_output_t fragment_shader_trail(common_v_output_t in [[stage_in]],
                                                   texture2d<half> diffuse_texture [[texture(0)]])
{
    oit_buffer_output_t out;
    float4 color = float4(in.color.rbg * -1.0, 1.0);
    color.a = diffuse_texture.sample(linear_sampler, in.texcoord).a;
    float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - in.position.z * 0.9, 3.0), 1e-2, 3e3);
    out.color = float4(color.rgb * color.a, color.a) * weight;
    out.alpha = half(color.a);
    
    return out;
}

//

vertex common_v_output_t vertex_shader_arrow(common_v_input_t in [[stage_in]],
                                             constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.view_space_position = in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_arrow(common_v_output_t in [[stage_in]],
                                                   texture2d<half> emissive_texture [[texture(0)]])
{
    half emissive = emissive_texture.sample(linear_sampler, in.texcoord).a;
    
    return half4(in.color.r, in.color.g, in.color.b, emissive);
}

//

vertex common_v_output_t vertex_shader_deferred_point_light(common_v_input_t in [[stage_in]],
                                                            constant common_u_input_t& mvp_uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(mvp_uniforms);
    out.position = mvp * in_position;
    
    return out;
}

fragment half4 fragment_shader_deferred_point_light(common_v_output_t in [[stage_in]],
                                                    constant point_light_u_input_t& custom_uniforms [[buffer(0)]],
                                                    texture2d<half> normal_texture [[texture(0)]],
                                                    texture2d<float> position_texture [[texture(1)]])
{
    uint2 screen_space_position = uint2(in.position.xy);
    float2 texcoord = float2((float)screen_space_position.x / position_texture.get_width(),
                             (float)screen_space_position.y / position_texture.get_height());
    
    float4 position = position_texture.sample(linear_sampler, texcoord);
    float4 normal = (float4)normal_texture.sample(linear_sampler, texcoord);
    
    float light_range = custom_uniforms.light_position_and_ray_length.w;
    float3 light_vector = custom_uniforms.light_position_and_ray_length.xyz - position.xyz;
    float3 light_direction = normalize(light_vector);
    float light_distance = length(light_vector);
    
    float no_depth_fix = step(0.0, light_range - light_distance);
    float attenuation = 1.0 / ((((light_distance / (1.0 - ((light_distance / light_range) *
                                                           (light_distance / light_range)))) / light_range) + 1.0) *
                               (((light_distance / ( 1.0 - ((light_distance / light_range) *
                                                            (light_distance / light_range)))) / light_range) + 1.0));
    attenuation *= no_depth_fix;
    
    float intensity = saturate(dot(light_direction, normal.xyz));
    float4 color = custom_uniforms.light_color;
    color *= intensity;
    color *= attenuation;
    
    float specular_power = 16.0;
    float specular_intensity = intensity;
    float3 reflection_vector = normalize(reflect(-light_direction, normal.xyz));
    float3 camera_direction = normalize(custom_uniforms.camera_position.xyz - position.xyz);
    float4 specular = saturate(custom_uniforms.light_color * specular_intensity * pow(saturate(dot(reflection_vector, camera_direction)), specular_power));
    specular *= attenuation * normal.w;
    color += specular;
    
    return (half4)color;
}

//

vertex common_v_output_t vertex_shader_stencil_deferred_point_light(common_v_input_t in [[stage_in]],
                                                                    constant common_u_input_t& mvp_uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(mvp_uniforms);
    out.position = mvp * in_position;
    
    return out;
}

fragment half4 fragment_shader_stencil_deferred_point_light(common_v_output_t in [[stage_in]],
                                                    constant point_light_u_input_t& custom_uniforms [[buffer(0)]])
{
    return half4(1.0);
}

//

vertex common_v_output_t vertex_shader_deferred_spot_light(common_v_input_t in [[stage_in]],
                                                           constant common_u_input_t& mvp_uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(mvp_uniforms);
    out.position = mvp * in_position;
    
    return out;
}

fragment half4 fragment_shader_deferred_spot_light(common_v_output_t in [[stage_in]],
                                                        constant spot_light_u_input_t& custom_uniforms [[buffer(0)]],
                                                        texture2d<half> normal_texture [[texture(0)]],
                                                        texture2d<float> position_texture [[texture(1)]])
{
    uint2 screen_space_position = uint2(in.position.xy);
    float2 texcoord = float2((float)screen_space_position.x / position_texture.get_width(),
                             (float)screen_space_position.y / position_texture.get_height());
    
    float4 position = position_texture.sample(linear_sampler, texcoord);
    float4 normal = (float4)normal_texture.sample(linear_sampler, texcoord);
    
    float3 light_vector = custom_uniforms.light_position.xyz - position.xyz;
    float3 light_direction = normalize(light_vector);
    
    float theta = dot(light_direction, normalize(-custom_uniforms.light_direction.xyz));
    float epsilon = custom_uniforms.light_cutoff_angles.x - custom_uniforms.light_cutoff_angles.y;
    float attenuation = saturate((theta - custom_uniforms.light_cutoff_angles.y) / epsilon);
    float intensity = saturate(dot(light_direction, normal.xyz));
    
    float4 color = custom_uniforms.light_color;
    color *= intensity;
    color *= attenuation;
    
    float specular_power = 16.0;
    float specular_intensity = intensity;
    float3 reflection_vector = normalize(reflect(-light_direction, normal.xyz));
    float3 camera_direction = normalize(custom_uniforms.camera_position.xyz - position.xyz);
    float4 specular = saturate(custom_uniforms.light_color * specular_intensity * pow(saturate(dot(reflection_vector, camera_direction)), specular_power));
    specular *= attenuation * normal.w;
    color += specular;
    
    return (half4)color;
}

//

vertex common_v_output_t vertex_shader_car_mask(common_v_input_t in [[stage_in]],
                                                         constant common_u_input_t& uniforms [[buffer(1)]])
{
    common_v_output_t out;
    
    float4 in_position = float4(in.position, 1.0);
    float4x4 mvp = get_mat_mvp(uniforms);
    out.position = mvp * in_position;
    out.texcoord = in.texcoord;
    out.color = in.color;
    
    return out;
}

fragment half4 fragment_shader_car_mask(common_v_output_t in [[stage_in]])
{
    return half4(1.f);
}

