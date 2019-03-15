//
//  gl_commands.cpp
//  gbCore
//
//  Created by serhii.s on 3/4/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "main_headers.h"

namespace gb
{
    namespace gl
    {
        namespace constant
        {
            
#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API
            
            ui32 yes = GL_TRUE;
            ui32 no = GL_FALSE;
            ui32 static_draw = GL_STATIC_DRAW;
            ui32 dynamic_draw = GL_DYNAMIC_DRAW;
            ui32 f32_t = GL_FLOAT;
            ui32 i32_t = GL_INT;
            ui32 ui32_t = GL_UNSIGNED_INT;
            
#if USED_GRAPHICS_API == OPENGL_30_API
            
            ui32 ui24_8_t = GL_UNSIGNED_INT_24_8;
            
#elif USED_GRAPHICS_API == OPENGL_20_API
            
            
            ui32 ui24_8_t = GL_UNSIGNED_INT_24_8_OES;
#endif
            
            ui32 i16_t = GL_SHORT;
            ui32 ui16_t = GL_UNSIGNED_SHORT;
            ui32 ui5_6_5_t = GL_UNSIGNED_SHORT_5_6_5;
            ui32 i8_t = GL_BYTE;
            ui32 ui8_t = GL_UNSIGNED_BYTE;
            ui32 rgb_t = GL_RGB;
            ui32 rgba_t = GL_RGBA;
            ui32 texture_2d = GL_TEXTURE_2D;
            ui32 texture_wrap_s = GL_TEXTURE_WRAP_S;
            ui32 texture_wrap_t = GL_TEXTURE_WRAP_T;
            ui32 texture_mag_filter = GL_TEXTURE_MAG_FILTER;
            ui32 texture_min_filter = GL_TEXTURE_MIN_FILTER;
            ui32 linear = GL_LINEAR;
            ui32 nearest = GL_NEAREST;
            ui32 clamp_to_edge = GL_CLAMP_TO_EDGE;
            ui32 repeat = GL_REPEAT;
            ui32 mirrored_repeat = GL_MIRRORED_REPEAT;
            ui32 linear_mipmap_nearest = GL_LINEAR_MIPMAP_NEAREST;
            ui32 front = GL_FRONT;
            ui32 back = GL_BACK;
            ui32 src_color = GL_SRC_COLOR;
            ui32 src_alpha = GL_SRC_ALPHA;
            ui32 one = GL_ONE;
            ui32 zero = GL_ZERO;
            ui32 one_minus_src_color = GL_ONE_MINUS_SRC_COLOR;
            ui32 one_minus_dst_color = GL_ONE_MINUS_DST_COLOR;
            ui32 one_minus_src_alpha = GL_ONE_MINUS_SRC_ALPHA;
            ui32 one_minus_dst_alpha = GL_ONE_MINUS_DST_ALPHA;
            ui32 dst_alpha = GL_DST_ALPHA;
            ui32 constant_alpha = GL_CONSTANT_ALPHA;
            ui32 always = GL_ALWAYS;
            ui32 equal = GL_EQUAL;
            ui32 lequal = GL_LEQUAL;
            ui32 notequal = GL_NOTEQUAL;
            ui32 func_add = GL_FUNC_ADD;
            ui32 blend = GL_BLEND;
            ui32 depth_test = GL_DEPTH_TEST;
            ui32 cull_face = GL_CULL_FACE;
            ui32 stencil_test = GL_STENCIL_TEST;
            ui32 frame_buffer = GL_FRAMEBUFFER;
            ui32 render_buffer = GL_RENDERBUFFER;
            ui32 frame_buffer_complete = GL_FRAMEBUFFER_COMPLETE;
            ui32 frame_buffer_binding = GL_FRAMEBUFFER_BINDING;
            ui32 render_buffer_binding = GL_RENDERBUFFER_BINDING;
            ui32 render_buffer_width = GL_RENDERBUFFER_WIDTH;
            ui32 render_buffer_height = GL_RENDERBUFFER_HEIGHT;
            ui32 color_attachment_0 = GL_COLOR_ATTACHMENT0;
            ui32 depth_attachment = GL_DEPTH_ATTACHMENT;
            ui32 stencil_attachment = GL_STENCIL_ATTACHMENT;
            
#if USED_GRAPHICS_API == OPENGL_30_API
            
            ui32 depth_stencil_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            ui32 texture_compare_mode = GL_TEXTURE_COMPARE_MODE;
            ui32 compare_ref_to_texture = GL_COMPARE_REF_TO_TEXTURE;
            ui32 texture_compare_func = GL_TEXTURE_COMPARE_FUNC;
            
#elif USED_GRAPHICS_API == OPENGL_20_API
            
            ui32 depth_stencil_attachment = 0;
            ui32 texture_compare_mode = GL_TEXTURE_COMPARE_MODE_EXT;
            ui32 compare_ref_to_texture = GL_COMPARE_REF_TO_TEXTURE_EXT;
            ui32 texture_compare_func = GL_TEXTURE_COMPARE_FUNC_EXT;
            
#endif
            
            ui32 depth_component = GL_DEPTH_COMPONENT;
            ui32 depth_component16 = GL_DEPTH_COMPONENT16;
            
#if USED_GRAPHICS_API == OPENGL_30_API
            
            ui32 depth_component24 = GL_DEPTH_COMPONENT24;
            ui32 depth_stencil = GL_DEPTH_STENCIL;
            ui32 depth24_stencil8 = GL_DEPTH24_STENCIL8;
            
#elif USED_GRAPHICS_API == OPENGL_20_API
            
            ui32 depth_component24 = GL_DEPTH_COMPONENT24_OES;
            ui32 depth_stencil = GL_DEPTH_STENCIL_OES;
            ui32 depth24_stencil8 = GL_DEPTH24_STENCIL8_OES;
            
#endif
            
            ui32 keep = GL_KEEP;
            ui32 replace = GL_REPLACE;
            ui32 color_buffer_bit = GL_COLOR_BUFFER_BIT;
            ui32 depth_buffer_bit = GL_DEPTH_BUFFER_BIT;
            ui32 stencil_buffer_bit = GL_STENCIL_BUFFER_BIT;
            ui32 vertex_shader = GL_VERTEX_SHADER;
            ui32 fragment_shader = GL_FRAGMENT_SHADER;
            ui32 texture_0 = GL_TEXTURE0;
            
#if USED_GRAPHICS_API == OPENGL_30_API
            
            ui32 red = GL_RED;
            ui32 luminance = 0;
            
#endif
            
#if USED_GRAPHICS_API == OPENGL_20_API
            
            ui32 red = 0;
            ui32 luminance = GL_LUMINANCE;

#endif
            
            ui32 array_buffer = GL_ARRAY_BUFFER;
            ui32 element_array_buffer = GL_ELEMENT_ARRAY_BUFFER;
            ui32 triangles = GL_TRIANGLES;
            
#else
            
            ui32 yes = 0;
            ui32 no = 0;
            ui32 static_draw = 0;
            ui32 dynamic_draw = 0;
            ui32 f32_t = 0;
            ui32 i32_t = 0;
            ui32 ui32_t = 0;
            ui32 ui24_8_t = 0;
            ui32 i16_t = 0;
            ui32 ui16_t = 0;
            ui32 ui5_6_5_t = 0;
            ui32 i8_t = 0;
            ui32 ui8_t = 0;
            ui32 rgb_t = 0;
            ui32 rgba_t = 0;
            ui32 texture_2d = 0;
            ui32 texture_wrap_s = 0;
            ui32 texture_wrap_t = 0;
            ui32 texture_mag_filter = 0;
            ui32 texture_min_filter = 0;
            ui32 linear = 0;
            ui32 nearest = 0;
            ui32 clamp_to_edge = 0;
            ui32 repeat = 0;
            ui32 mirrored_repeat = 0;
            ui32 linear_mipmap_nearest = 0;
            ui32 front = 0;
            ui32 back = 0;
            ui32 src_color = 0;
            ui32 src_alpha = 0;
            ui32 one = 0;
            ui32 zero = 0;
            ui32 one_minus_src_color = 0;
            ui32 one_minus_dst_color = 0;
            ui32 one_minus_src_alpha = 0;
            ui32 one_minus_dst_alpha = 0;
            ui32 dst_alpha = 0;
            ui32 constant_alpha = 0;
            ui32 always = 0;
            ui32 equal = 0;
            ui32 lequal = 0;
            ui32 notequal = 0;
            ui32 func_add = 0;
            ui32 blend = 0;
            ui32 depth_test = 0;
            ui32 cull_face = 0;
            ui32 stencil_test = 0;
            ui32 frame_buffer = 0;
            ui32 render_buffer = 0;
            ui32 frame_buffer_complete = 0;
            ui32 frame_buffer_binding = 0;
            ui32 render_buffer_binding = 0;
            ui32 render_buffer_width = 0;
            ui32 render_buffer_height = 0;
            ui32 color_attachment_0 = 0;
            ui32 depth_attachment = 0;
            ui32 stencil_attachment = 0;
            ui32 depth_stencil_attachment = 0;
            ui32 texture_compare_mode = 0;
            ui32 compare_ref_to_texture = 0;
            ui32 texture_compare_func = 0;
            ui32 depth_component = 0;
            ui32 depth_component16 = 0;
            ui32 depth_component24 = 0;
            ui32 depth_stencil = 0;
            ui32 depth24_stencil8 = 0;
            ui32 keep = 0;
            ui32 replace = 0;
            ui32 color_buffer_bit = 0;
            ui32 depth_buffer_bit = 0;
            ui32 stencil_buffer_bit = 0;
            ui32 vertex_shader = 0;
            ui32 fragment_shader = 0;
            ui32 texture_0 = 0;
            ui32 red = 0;
            ui32 luminance = 0;
            ui32 array_buffer = 0;
            ui32 element_array_buffer = 0;
            ui32 triangles = 0;
            
#endif
            
        };
    };
};
