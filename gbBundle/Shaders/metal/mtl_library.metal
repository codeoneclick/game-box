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
constexpr sampler repeat_sampler(coord::normalized, address::repeat, filter::linear);

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
    float4 color [[color(0)]];
    half4  normal [[color(1)]];
    float4 position [[color(2)]];
} g_buffer_output_t;

typedef struct
{
    float4 color [[color(0)]];
    half alpha [[color(1)]];
    
} oit_buffer_output_t;

typedef struct
{
    float4x4 mat_m;
    float4x4 mat_v;
    float4x4 mat_p;
} __attribute__ ((aligned(256))) common_u_input_t;

typedef struct
{
    float4 light_position_and_ray_length;
    float4 light_color;
    float4 camera_position;
} __attribute__ ((aligned(256))) point_light_u_input_t;

typedef struct
{
    float4 light_position;
    float4 light_direction;
    float4 light_cutoff_angles;
    float4 light_color;
    float4 camera_position;
} __attribute__ ((aligned(256))) spot_light_u_input_t;

typedef struct
{
    float vignetting_edge_size;
} __attribute__ ((aligned(256))) ss_output_u_input_t;

typedef struct
{
    float enabled;
    float time;
} __attribute__ ((aligned(256))) ss_tv_u_input_t;

typedef struct
{
    float enabled;
    float progress;
} __attribute__ ((aligned(256))) ss_cross_fade_u_input_t;

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

float3 mod289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

//

float2 mod289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

//

float3 permute(float3 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}

//

float snoise(float2 v)
{
    const float4 noise = float4(0.211324865405187,
                                0.366025403784439,
                                -0.577350269189626,
                                0.024390243902439);
    float2 i = floor(v + dot(v, noise.yy));
    float2 x0 = v - i + dot(i, noise.xx);
    
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float4 x12 = x0.xyxy + noise.xxzz;
    x12.xy -= i1;
    
    i = mod289(i);
    float3 p = permute(permute(i.y + float3(0.0, i1.y, 1.0 ))
                       + i.x + float3(0.0, i1.x, 1.0 ));
    
    float3 m = max(0.5 - float3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    
    float3 x = 2.0 * fract(p * noise.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;
    
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
    
    float3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

//

float rand(float2 value)
{
    return fract(sin(dot(value.xy, float2(12.9898,78.233))) * 43758.5453);
}

//

float linear_ease(float begin, float change, float duration, float progress)
{
    return change * progress / duration + begin;
}

float exponential_ease_in_out(float begin, float change, float duration, float progress)
{
    if (progress == 0.0)
        return begin;
    else if (progress == duration)
        return begin + change;
    progress = progress / (duration / 2.0);
    if (progress < 1.0)
        return change / 2.0 * pow(2.0, 10.0 * (progress - 1.0)) + begin;
    return change / 2.0 * (-pow(2.0, -10.0 * (progress - 1.0)) + 2.0) + begin;
}

float sinusoidal_ease_in_out(float begin, float change, float duration, float progress)
{
    return -change / 2.0 * (cos(3.14 * progress / duration) - 1.0) + begin;
}

//

float4 adjust_saturation(float4 color, float saturation)
{
    float grey = dot(color.rbg, float3(0.3, 0.59, 0.11));
    return mix(float4(grey), color, saturation);
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
                                                     texture2d<float> opaque_color_texture [[texture(0)]],
                                                     texture2d<float> transparent_color_texture [[texture(1)]],
                                                     texture2d<half> transparent_alpha_texture [[texture(2)]],
                                                     texture2d<half> lighting_texture [[texture(3)]])
{
    float4 opaque_color = opaque_color_texture.sample(trilinear_sampler, in.texcoord);
    float4 transparent_color = transparent_color_texture.sample(trilinear_sampler, in.texcoord);
    float transparent_alpha = (float)transparent_alpha_texture.sample(trilinear_sampler, in.texcoord).r;
    transparent_color = float4(transparent_color.rgb / max(transparent_color.a, 1e-5), 0.f);
    float4 color = opaque_color + transparent_color * (1.0 - transparent_alpha);
    
    float4 lighting = (float4)lighting_texture.sample(trilinear_sampler, in.texcoord);
    color.rbg = color.rbg * max(lighting.rbg, float3(0.05));
    return color;
}

//

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
    
    float3 diff = (float3)position_texture.sample(trilinear_sampler, texcoord + uv).xyz - position;
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
    float4 normal = (float4)normal_texture.sample(trilinear_sampler, in.texcoord);
    float4 position = position_texture.sample(trilinear_sampler, in.texcoord);
    float2 random = (float2)random_texture.sample(trilinear_sampler, in.texcoord).xy;
    
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
                                         texture2d<half> bloom_texture [[texture(1)]],
                                         texture2d<half> ao_texture [[texture(2)]],
                                         texture2d<half> emissive_texture [[texture(3)]])
{
    const float bloom_intensity = 1.25;
    const float original_intensity = 1.0;
    const float bloom_saturation = 1.0;
    const float original_saturation = 1.0;
    
    float4 bloom = (float4)bloom_texture.sample(trilinear_sampler, in.texcoord);
    float4 color = (float4)color_texture.sample(trilinear_sampler, in.texcoord);
    float4 ao = (float4)ao_texture.sample(trilinear_sampler, in.texcoord);
    float4 emissive = (float4)emissive_texture.sample(trilinear_sampler, in.texcoord);
    color.rgb *= ao.r;
    
    bloom = adjust_saturation(bloom, bloom_saturation) * bloom_intensity;
    color = adjust_saturation(color, original_saturation) * original_intensity;
    
    color *= (1.0 - saturate(bloom));
    color = color + bloom + emissive;
    
    float2 uv = in.texcoord;
    float edge = 0.1;
    float dark = uniforms.vignetting_edge_size;
    float2 dir = max(abs(uv * 2.0 - 1.0) * (1.0 + edge) - edge + dark, float2(0));
    float v = dot(dir, dir);
    v = 1.0 - v / (1.0 + v);
    
    color.rgb += (1.0 - v) * float3(1.0, 0.0, 0.0);
    
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
    float4 color = (float4)color_texture.sample(trilinear_sampler, in.texcoord);
    float4 ui = (float4)ui_texture.sample(trilinear_sampler, in.texcoord);
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
    float4 color = (float4)color_texture.sample(trilinear_sampler, in.texcoord);
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
        
        /*float noise = max(0.0, snoise(float2(time, uv.y * 0.3)) - 0.3) * (1.0 / 0.7);
        noise = noise + (snoise(float2(time * 2.0, uv.y * 2.4)) - 0.5) * 0.15;
        float xpos = uv.x - noise * noise * 0.25;
        float4 color = (float4)color_texture.sample(trilinear_sampler, float2(xpos, uv.y));
        color.rgb = mix(color.rgb, float3(rand(float2(uv.y * time))), noise * 0.3).rgb;
        
        if (floor(fmod(uv.y * 0.25, 2.0)) == 0.0)
        {
            color.rgb *= 1.0 - (0.15 * noise);
        }

        color.g = mix(color.r, (float)color_texture.sample(trilinear_sampler, float2(xpos + noise * 0.05, uv.y)).g, 0.25);
        color.b = mix(color.r, (float)color_texture.sample(trilinear_sampler, float2(xpos - noise * 0.05, uv.y)).b, 0.25);*/
        
        float distance_to_center = length(uv - float2(0.5, 0.5));
        float blur = 0.0;
        blur = (1.0 + sin(time * 6.0)) * 0.5;
        blur *= 1.0 + sin(time * 16.0) * 0.5;
        blur = pow(blur, 3.0);
        blur *= 0.05;
        blur *= distance_to_center;
        
        color.r = (float)color_texture.sample(trilinear_sampler, float2(uv.x + blur,uv.y)).r;
        color.g = (float)color_texture.sample(trilinear_sampler, uv).g;
        color.b = (float)color_texture.sample(trilinear_sampler, float2(uv.x - blur,uv.y)).b;
        float scanline = sin(uv.y * 768.0) * 0.04;
        color -= scanline;
        color *= 1.0 - distance_to_center * 0.5;
        color.a = 1.0;
    }
    else
    {
        color = (float4)color_texture.sample(trilinear_sampler, uv);
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
            color += mix((float4)color_texture.sample(trilinear_sampler, uv + to_center * percent * strength), float4(0.0, 0.0, 0.0, 1.0), progress) * weight;
            max_weight += weight;
        }
        
        color = color / max_weight;
        color.a = 1.0;
    }
    else
    {
        color = (float4)color_texture.sample(trilinear_sampler, uv);
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
    float4 color = (float4)diffuse_texture.sample(repeat_sampler, in.texcoord);
    out.color = color;
    
#if defined(TBN)
    
    float3x3 mat_tbn = float3x3(in.tangent, in.bitangent, in.normal);
    float3 normal_color = (float3)normal_texture.sample(repeat_sampler, in.texcoord).rgb * 2.0 - 1.0;
    float3 normal_tbn = mat_tbn * normal_color;
    
#endif
    
    out.normal = half4(half3(in.normal), 1.0);
    out.position = in.view_space_position;
    
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
    color.a = emissive_texture.sample(trilinear_sampler, in.texcoord).a;
    
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
    half emissive = emissive_texture.sample(trilinear_sampler, in.texcoord).r;
    
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
    half emissive = emissive_texture.sample(trilinear_sampler, in.texcoord).r;
    
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
                                                              texture2d<half> diffuse_texture [[texture(0)]])
{
    oit_buffer_output_t out;
    float4 color = float4(in.color.rbg, 1.0);
    color.a = diffuse_texture.sample(trilinear_sampler, in.texcoord).a * in.color.a;
    float weight = clamp(pow(min(1.0, color.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - in.position.z * 0.9, 3.0), 1e-2, 3e3);
    out.color = float4(color.rgb * color.a, color.a) * weight;
    out.alpha = half(color.a);
    
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
    color.a = diffuse_texture.sample(trilinear_sampler, in.texcoord).r;
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
    color.a = diffuse_texture.sample(trilinear_sampler, in.texcoord).a;
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
    half emissive = emissive_texture.sample(trilinear_sampler, in.texcoord).a;
    
    return half4(in.color.r, in.color.g, in.color.b, emissive);
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
    
    half4 normal = normal_texture.read(screen_space_position);
    float4 position = position_texture.read(screen_space_position);
    
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
    
    float intensity = saturate(dot(light_direction, float3(normal.xyz)));
    
    float4 color = intensity * custom_uniforms.light_color;
    
    float3 reflection_vector = normalize(reflect(-light_direction, float3(normal.xyz)));
    float3 camera_direction = normalize(custom_uniforms.camera_position.xyz - position.xyz);
    
    float specular_power = 4.0;
    float specular_intensity = 200.0;
    float4 specular = saturate(custom_uniforms.light_color * specular_intensity * pow(saturate(dot(reflection_vector, camera_direction)), specular_power));
    
    float4 brdf = 1.5 * float4(.10, .11, .11, 1.0);
    brdf += 1.30 * intensity * float4(1., .9, .75, 1.0);
    color *= brdf;
    
    color = attenuation * (color + specular);
    return (half4)color;
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
    
    half4 normal = normal_texture.read(screen_space_position);
    float4 position = position_texture.read(screen_space_position);
    
    float3 light_vector = custom_uniforms.light_position.xyz - position.xyz;
    float3 light_direction = normalize(light_vector);
    
    float theta = dot(light_direction, normalize(-custom_uniforms.light_direction.xyz));
    float epsilon = custom_uniforms.light_cutoff_angles.x - custom_uniforms.light_cutoff_angles.y;
    float attenuation = clamp((theta - custom_uniforms.light_cutoff_angles.y) / epsilon, 0.0, 1.0);
    
    float intensity = saturate(dot(light_direction, float3(normal.xyz)));
    float4 color = intensity * custom_uniforms.light_color * attenuation;
    return (half4)color;
}


