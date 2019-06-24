//
//  common.metal
//  gbDemo
//
//  Created by serhii.s on 6/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

constexpr sampler linear_sampler(mip_filter::linear, mag_filter::linear, min_filter::linear);
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
    float4 position [[position]];
    float2 texcoord;
    float4 view_space_position;
    float3 normal;
    float3 cube_texcoord;
    float3 camera_direction;
} reflect_v_output_t;

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
    float4x4 mat_n;
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
    float4 vignetting_color;
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

typedef struct
{
    float4 camera_position;
    float4x4 mat_i_v;
} __attribute__ ((aligned(256))) reflect_u_input_t;

inline float4x4 get_mat_m(common_u_input_t uniforms)
{
    return uniforms.mat_m;
}

inline float4x4 get_mat_v(common_u_input_t uniforms)
{
    return uniforms.mat_v;
}

inline float4x4 get_mat_p(common_u_input_t uniforms)
{
    return uniforms.mat_p;
}

inline float4x4 get_mat_n(common_u_input_t uniforms)
{
    return uniforms.mat_n;
}

inline float4x4 get_mat_mvp(common_u_input_t uniforms)
{
    return get_mat_p(uniforms) *  get_mat_v(uniforms) *  get_mat_m(uniforms);
}

//

inline float rand(float2 value)
{
    return fract(sin(dot(value.xy, float2(12.9898,78.233))) * 43758.5453);
}

//

inline float linear_ease(float begin, float change, float duration, float progress)
{
    return change * progress / duration + begin;
}

//

inline float sinusoidal_ease_in_out(float begin, float change, float duration, float progress)
{
    return -change / 2.0 * (cos(3.14 * progress / duration) - 1.0) + begin;
}

//

inline float4 adjust_saturation(float4 color, float saturation)
{
    float grey = dot(color.rbg, float3(0.3, 0.59, 0.11));
    return mix(float4(grey), color, saturation);
}

