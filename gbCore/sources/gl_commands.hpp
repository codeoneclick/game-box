//
//  glCommands.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

// #define USED_GRAPHICS_API NO_GRAPHICS_API

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#if defined(__OSX__)

#define USED_GRAPHICS_API OPENGL_30_API

#elif defined(__WINOS__)

#define USED_GRAPHICS_API OPENGL_30_API
// #define USED_GRAPHICS_API VULKAN_API

#elif defined(__TVOS__)

#define USED_GRAPHICS_API OPENGL_20_API

#elif defined(__IOS__)

#define USED_GRAPHICS_API OPENGL_20_API

#endif

#if defined(__IOS__)

    #include <QuartzCore/QuartzCore.h>
    #include <sys/types.h>
    #include <sys/sysctl.h>

    #if USED_GRAPHICS_API == OPENGL_20_API

        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>

    #elif USED_GRAPHICS_API == OPENGL_30_API

        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>

    #endif

#elif defined(__TVOS__)

    #include <QuartzCore/QuartzCore.h>
    #include <sys/types.h>
    #include <sys/sysctl.h>

    #if USED_GRAPHICS_API == OPENGL_20_API

        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>

    #elif USED_GRAPHICS_API == OPENGL_30_API

        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>

    #endif

#elif defined(__OSX__)

#include <OpenGL/OpenGL.h>

    #if USED_GRAPHICS_API == OPENGL_20_API

        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>

    #elif USED_GRAPHICS_API == OPENGL_30_API

        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>

    #endif

#elif defined(__WINOS__)

    #if USED_GRAPHICS_API == OPENGL_20_API

        #define GL_GLEXT_PROTOTYPES 1
        #include <gl/glew.h>

    #elif USED_GRAPHICS_API == OPENGL_30_API

		#define GL_GLEXT_PROTOTYPES 1
		#include <gl/glew.h>

    #elif USED_GRAPHICS_API == VULKAN_API

        #define VK_USE_PLATFORM_WIN32_KHR
        #include <vulkan/vulkan.h>
        #include <shaderc/shaderc.hpp>

    #endif

#endif

#endif

#define CASE_STRING_VALUE(GL_ERROR, STRING_ERROR) case GL_ERROR: STRING_ERROR = #GL_ERROR; break;

namespace gb
{
    
#define GL_ERROR_ENABLED (1)
    
    inline ui32 gl_get_error()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
#if defined(GL_ERROR_ENABLED)
        
        ui32 error = glGetError();
        std::string string_error = "";
        switch (error)
        {
            case GL_NO_ERROR:
            {
                
            }
                break;
                
                CASE_STRING_VALUE(GL_INVALID_ENUM, string_error)
                CASE_STRING_VALUE(GL_INVALID_VALUE, string_error)
                CASE_STRING_VALUE(GL_INVALID_OPERATION, string_error)
                CASE_STRING_VALUE(GL_OUT_OF_MEMORY, string_error)
                CASE_STRING_VALUE(GL_INVALID_FRAMEBUFFER_OPERATION, string_error)
                
            default:
            {
                std::stringstream str_stream;
                str_stream<<"unknown - "<<error;
                string_error = str_stream.str();
            }
        }
        if(string_error.length() != 0)
        {
            std::cout<<"OpenGL error: "<<string_error<<std::endl;
        }
        return error;
        
#else
        
        return 0;
        
#endif
        
#else
        
        return 0;
        
#endif
    };
    
    inline void gl_enable(ui32 caption)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glEnable(caption);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_disable(ui32 caption)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDisable(caption);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_depth_function(ui32 function)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDepthFunc(function);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_depth_mask(ui8 flag)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDepthMask(flag);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_cull_face(ui32 mode)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glCullFace(mode);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_blend_function(ui32 source_factor, ui32 destination_factor)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBlendFunc(source_factor, destination_factor);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_blend_equation(ui32 equation)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBlendEquation(equation);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_clear(ui32 mask)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glClear(mask);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_clear_color(f32 red, f32 green, f32 blue, f32 alpha)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glClearColor(red, green, blue, alpha);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_viewport(i32 x, i32 y, i32 width, i32 height)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glViewport(x, y, width, height);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_render_buffers(i32 counts, const ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDeleteRenderbuffers(counts, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_render_buffers(i32 count, ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glGenRenderbuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_render_buffer_storage(ui32 target, ui32 internalformat, i32 width, i32 height)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glRenderbufferStorage(target, internalformat, width, height);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_frame_buffer(ui32 target, ui32 framebuffer)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBindFramebuffer(target, framebuffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_render_buffer(ui32 target, ui32 renderbuffer)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBindRenderbuffer(target, renderbuffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_frame_buffers(i32 count, const ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDeleteFramebuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_frame_buffers(i32 count, ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glGenFramebuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_attach_frame_buffer_texture2d(ui32 target, ui32 attachment, ui32 textarget, ui32 texture, i32 level)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glFramebufferTexture2D(target, attachment, textarget, texture, level);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_attach_frame_buffer_render_buffer(ui32 target, ui32 attachment, ui32 renderbuffertarget, ui32 renderbuffer)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_textures(i32 count, ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glGenTextures(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_textures(i32 count, const ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDeleteTextures(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_texture(ui32 target, ui32 texture)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBindTexture(target, texture);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_texture_parameter_i(ui32 target, ui32 name, i32 parameter)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glTexParameteri(target, name, parameter);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_texture_image2d(ui32 target, i32 level, i32 internalformat, i32 width, i32 height, i32 border, ui32 format, ui32 type, const void *pixels)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_compressed_texture_image2d(ui32 target, i32 level, ui32 internalformat, i32 width, i32 height, i32 border, i32 imageSize, const void *data)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }
    
    inline void gl_generate_mipmap(ui32 target)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glGenerateMipmap(target);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_buffers(i32 count, ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glGenBuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_buffers(i32 count, const ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDeleteBuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_buffer(ui32 target, ui32 buffer)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBindBuffer(target, buffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_push_buffer_data(ui32 target, i32 size, const void *data, ui32 usage)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBufferData(target, size, data, usage);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_push_buffer_sub_data(ui32 target, i32 offset, i32 size, const void *data)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glBufferSubData(target, offset, size, data);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_enable_vertex_attribute(i32 index)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glEnableVertexAttribArray(index);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_disable_vertex_attribute(i32 index)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDisableVertexAttribArray(index);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_vertex_attribute(ui32 index, i32 size, ui32 type, ui8 normalized, i32 stride, const void *pointer)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glVertexAttribPointer(index, size, type, normalized, stride, pointer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_set_vertex_attribute_divisor(ui32 index, ui32 divisor)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

#if defined(__OSX__)
        
#if USED_GRAPHICS_API == OPENGL_20_API
        
        glVertexAttribDivisorEXT(index, divisor);
        
#else
        
        glVertexAttribDivisor(index, divisor);
        
#endif
        
#elif defined(__IOS__)

        glVertexAttribDivisorEXT(index, divisor);
#endif

#if defined(__WINOS__)
		glVertexAttribDivisor(index, divisor);
#endif

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }

    inline void gl_bind_vertex_array(ui32 array)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

#if defined(__OSX__)

        glBindVertexArray(array);

#endif

#if defined(__IOS__) 
	#if USED_GRAPHICS_API == OPENGL_30_API
        glBindVertexArray(array);
	#elif USED_GRAPHICS_API == OPENGL_20_API
        glBindVertexArrayOES(array);
	#endif
#endif
#if defined(__WINOS__)
		glBindVertexArray(array);
#endif 

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_vertex_arrays(i32 count, const ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

#if defined(__OSX__)

        glDeleteVertexArrays(count, pointers);

#endif

#if defined(__IOS__) 

	#if USED_GRAPHICS_API == OPENGL_30_API

        glDeleteVertexArrays(count, pointers);

	#elif USED_GRAPHICS_API == OPENGL_20_API

        glDeleteVertexArraysOES(count, pointers);

	#endif

#endif

#if defined(__WINOS__)

		glDeleteVertexArrays(count, pointers);

#endif   

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_vertex_arrays(i32 count, ui32 *pointers)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

#if defined(__OSX__)

        glGenVertexArrays(count, pointers);

#endif

#if defined(__IOS__) 

	#if USED_GRAPHICS_API == OPENGL_30_API

        glGenVertexArrays(count, pointers);

	#elif USED_GRAPHICS_API == OPENGL_20_API

        glGenVertexArraysOES(count, pointers);

	#endif

#endif

#if defined(__WINOS__)

		glGenVertexArrays(count, pointers);

#endif   

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_draw_elements(ui32 mode, i32 count, ui32 type, const void *indices)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDrawElements(mode, count, type, indices);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_draw_elements_instanced(ui32 mode, i32 count, ui32 type, const void *indices, i32 instancecount)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

#if defined(__OSX__)

        glDrawElementsInstanced(mode, count, type, indices, instancecount);

#elif defined(__IOS__)

#if USED_GRAPHICS_API == OPENGL_20_API

        glDrawElementsInstancedEXT(mode, count, type, indices, instancecount);

#else

        glDrawElementsInstanced(mode, count, type, indices, instancecount);

#endif

#endif

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }
    
    inline i32 gl_get_uniform_location(ui32 program, const char *name)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        return glGetUniformLocation(program, (char *)name);
#else
		return -1;

#endif
    };
    
    inline i32 gl_get_attribute_location(ui32 program, const char *name)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        return glGetAttribLocation(program, (char *)name);

#else
		return -1;

#endif
    };
    
    inline void gl_get_uniform_matrix_3fv(i32 location, i32 count, ui8 transpose, const f32 *value)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glUniformMatrix3fv(location, count, transpose, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_matrix_4fv(i32 location, i32 count, ui8 transpose, const f32 *value)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glUniformMatrix4fv(location, count, transpose, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_2fv(i32 location, i32 count, const f32 *value)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glUniform2fv(location, count, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_3fv(i32 location, i32 count, const f32 *value)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API
		
        glUniform3fv(location, count, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_4fv(i32 location, i32 count, const f32 *value)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glUniform4fv(location, count, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_1f(i32 location, f32 v0)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glUniform1f(location, v0);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_1i(i32 location, i32 v0)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glUniform1i(location, v0);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_set_active_texture(ui32 texture)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glActiveTexture(texture);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_use_program(i32 program)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glUseProgram(program);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_stencil_function(ui32 function, i32 reference, ui32 mask)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glStencilFunc(function, reference, mask);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_stencil_mask(ui32 mask)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glStencilMask(mask);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_color_mask(ui8 red_flag, ui8 green_flag, ui8 blue_flag, ui8 alpha_flag)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glColorMask(red_flag, green_flag, blue_flag, alpha_flag);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_color_mask(ui8 depth_flag)
    {
#if USED_GRAPHICS_API != NO_GRAPHICS_API

        glDepthMask(depth_flag);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };

	inline ui32 gl_create_shader(ui32 shader_type)
	{
#if USED_GRAPHICS_API != NO_GRAPHICS_API

		ui32 handle = glCreateShader(shader_type);

#endif

#if defined(DEBUG)
		gl_get_error();
#endif
		return handle;
	};
};
