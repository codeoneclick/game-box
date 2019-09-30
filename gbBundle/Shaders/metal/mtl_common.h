//
//  common.metal
//  gbDemo
//
//  Created by serhii.s on 6/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

constexpr sampler linear_sampler(mip_filter::linear, address::clamp_to_edge, mag_filter::linear, min_filter::linear);
constexpr sampler repeat_sampler(coord::normalized, address::repeat, filter::linear);
constexpr sampler mirrored_repeat_sampler(coord::normalized, address::mirrored_repeat, filter::linear);

typedef struct
{
    vector_float3 position [[ attribute(0) ]];
    vector_float2 texcoord [[ attribute(1) ]];
    vector_float4 normal [[ attribute(2) ]];
    vector_float4 tangent [[ attribute(3) ]];
    vector_float4 color [[ attribute(4) ]];
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
    float3 normal;
    float3 tangent;
    float3 bitangent;
    float3 cube_texcoord;
    float4 position_m;
    float3 camera_position;
    float3 normal_vm;
    float3 camera_direction_vm;
} reflect_v_output_t;

typedef struct
{
    half4  color [[color(0)]];
    half4  normal [[color(1)]];
    float4 position [[color(2)]];
} g_buffer_output_t;

typedef struct
{
    matrix_float4x4 mat_m;
    matrix_float4x4 mat_v;
    matrix_float4x4 mat_p;
    matrix_float4x4 mat_n;
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
    float4 motion_direction;
    float4 vignetting_color;
    float4 parameters_01;
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

typedef struct
{
    float4 body_color;
    float4 windows_color;
} __attribute__ ((aligned(256))) colorization_u_input_t;

typedef struct
{
    float4x4 mat_v;
} __attribute__ ((aligned(256))) ssao_u_input_t;

inline float4x4 get_mat_m(constant common_u_input_t& uniforms)
{
    return uniforms.mat_m;
}

inline float4x4 get_mat_v(constant common_u_input_t& uniforms)
{
    return uniforms.mat_v;
}

inline float4x4 get_mat_p(constant common_u_input_t& uniforms)
{
    return uniforms.mat_p;
}

inline float4x4 get_mat_n(constant common_u_input_t& uniforms)
{
    return uniforms.mat_n;
}

inline float4x4 get_mat_mvp(constant common_u_input_t& uniforms)
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

//

inline float normpdf(float x, float sigma)
{
    return 0.39894 * exp(-0.5 * x * x / (sigma * sigma)) / sigma;
}

