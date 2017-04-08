//
//  glCommands.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef gl_commands_h
#define gl_commands_h

#if !defined(__NO_RENDER__)

#if defined(__OSX__)

#define __OPENGL_30__ 1

#elif defined(__WIN32__)

#define __OPENGL_20__ 1

#elif defined(__TVOS__)

#define __OPENGL_20__ 1

#elif defined(__IOS__)

#define __OPENGL_20__ 1

#endif

#if defined(__IOS__)

    #include <QuartzCore/QuartzCore.h>
    #include <sys/types.h>
    #include <sys/sysctl.h>

    #if defined(__OPENGL_20__)

        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>

    #elif defined(__OPENGL_30__)

        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>

    #endif

#elif defined(__TVOS__)

    #include <QuartzCore/QuartzCore.h>
    #include <sys/types.h>
    #include <sys/sysctl.h>

    #if defined(__OPENGL_20__)

        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>

    #elif defined(__OPENGL_30__)

        #include <OpenGLES/ES3/gl.h>
        #include <OpenGLES/ES3/glext.h>

    #endif

#elif defined(__OSX__)

#include <OpenGL/OpenGL.h>

    #if defined(__OPENGL_20__)

        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>

    #elif defined(__OPENGL_30__)

        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>

    #endif

#elif defined(__WIN32__)

    #if defined(__OPENGL_20__)

        #define GL_GLEXT_PROTOTYPES 1
        #include <gl/glew.h>

    #elif defined(__OPENGL_30__)

        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>

    #endif

#endif

#endif

#define CASE_STRING_VALUE(GL_ERROR, STRING_ERROR) case GL_ERROR: STRING_ERROR = #GL_ERROR; break;

namespace gb
{
    
#define GL_ERROR_ENABLED (1)
    
    inline ui32 gl_get_error()
    {
        
#if !defined(__NO_RENDER__)
        
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
#if !defined(__NO_RENDER__)

        glEnable(caption);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_disable(ui32 caption)
    {
#if !defined(__NO_RENDER__)

        glDisable(caption);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_depth_function(ui32 function)
    {
#if !defined(__NO_RENDER__)

        glDepthFunc(function);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_depth_mask(ui8 flag)
    {
#if !defined(__NO_RENDER__)

        glDepthMask(flag);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_cull_face(ui32 mode)
    {
#if !defined(__NO_RENDER__)

        glCullFace(mode);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_blend_function(ui32 source_factor, ui32 destination_factor)
    {
#if !defined(__NO_RENDER__)

        glBlendFunc(source_factor, destination_factor);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_blend_equation(ui32 equation)
    {
#if !defined(__NO_RENDER__)

        glBlendEquation(equation);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_clear(ui32 mask)
    {
#if !defined(__NO_RENDER__)

        glClear(mask);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_clear_color(f32 red, f32 green, f32 blue, f32 alpha)
    {
#if !defined(__NO_RENDER__)

        glClearColor(red, green, blue, alpha);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_viewport(i32 x, i32 y, i32 width, i32 height)
    {
#if !defined(__NO_RENDER__)

        glViewport(x, y, width, height);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_render_buffers(i32 counts, const ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glDeleteRenderbuffers(counts, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_render_buffers(i32 count, ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glGenRenderbuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_render_buffer_storage(ui32 target, ui32 internalformat, i32 width, i32 height)
    {
#if !defined(__NO_RENDER__)

        glRenderbufferStorage(target, internalformat, width, height);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_frame_buffer(ui32 target, ui32 framebuffer)
    {
#if !defined(__NO_RENDER__)

        glBindFramebuffer(target, framebuffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_render_buffer(ui32 target, ui32 renderbuffer)
    {
#if !defined(__NO_RENDER__)

        glBindRenderbuffer(target, renderbuffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_frame_buffers(i32 count, const ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glDeleteFramebuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_frame_buffers(i32 count, ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glGenFramebuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_attach_frame_buffer_texture2d(ui32 target, ui32 attachment, ui32 textarget, ui32 texture, i32 level)
    {
#if !defined(__NO_RENDER__)

        glFramebufferTexture2D(target, attachment, textarget, texture, level);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_attach_frame_buffer_render_buffer(ui32 target, ui32 attachment, ui32 renderbuffertarget, ui32 renderbuffer)
    {
#if !defined(__NO_RENDER__)

        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_textures(i32 count, ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glGenTextures(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_textures(i32 count, const ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glDeleteTextures(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_texture(ui32 target, ui32 texture)
    {
#if !defined(__NO_RENDER__)

        glBindTexture(target, texture);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_texture_parameter_i(ui32 target, ui32 name, i32 parameter)
    {
#if !defined(__NO_RENDER__)

        glTexParameteri(target, name, parameter);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_texture_image2d(ui32 target, i32 level, i32 internalformat, i32 width, i32 height, i32 border, ui32 format, ui32 type, const void *pixels)
    {
#if !defined(__NO_RENDER__)

        glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_compressed_texture_image2d(ui32 target, i32 level, ui32 internalformat, i32 width, i32 height, i32 border, i32 imageSize, const void *data)
    {
#if !defined(__NO_RENDER__)

        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }
    
    inline void gl_generate_mipmap(ui32 target)
    {
#if !defined(__NO_RENDER__)

        glGenerateMipmap(target);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_buffers(i32 count, ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glGenBuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_buffers(i32 count, const ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

        glDeleteBuffers(count, pointers);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_buffer(ui32 target, ui32 buffer)
    {
#if !defined(__NO_RENDER__)

        glBindBuffer(target, buffer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_push_buffer_data(ui32 target, i32 size, const void *data, ui32 usage)
    {
#if !defined(__NO_RENDER__)

        glBufferData(target, size, data, usage);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_push_buffer_sub_data(ui32 target, i32 offset, i32 size, const void *data)
    {
#if !defined(__NO_RENDER__)

        glBufferSubData(target, offset, size, data);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_enable_vertex_attribute(i32 index)
    {
#if !defined(__NO_RENDER__)

        glEnableVertexAttribArray(index);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_disable_vertex_attribute(i32 index)
    {
#if !defined(__NO_RENDER__)

        glDisableVertexAttribArray(index);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_vertex_attribute(ui32 index, i32 size, ui32 type, ui8 normalized, i32 stride, const void *pointer)
    {
#if !defined(__NO_RENDER__)

        glVertexAttribPointer(index, size, type, normalized, stride, pointer);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_set_vertex_attribute_divisor(ui32 index, ui32 divisor)
    {
#if !defined(__NO_RENDER__)

#if defined(__OSX__)
        
#if defined(__OPENGL_20__)
        
        glVertexAttribDivisorEXT(index, divisor);
        
#else
        
        glVertexAttribDivisor(index, divisor);
        
#endif
        
#elif defined(__IOS__)

        glVertexAttribDivisorEXT(index, divisor);
#endif

#if defined(__WIN32__)
		glVertexAttribDivisor(index, divisor);
#endif

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }

    inline void gl_bind_vertex_array(ui32 array)
    {
#if !defined(__NO_RENDER__)

#if defined(__OSX__)

        glBindVertexArray(array);

#endif

#if defined(__IOS__) 
	#if defined(__OPENGL_30__)
        glBindVertexArray(array);
	#else
        glBindVertexArrayOES(array);
	#endif
#endif
#if defined(__WIN32__)
		glBindVertexArray(array);
#endif 

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_vertex_arrays(i32 count, const ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

#if defined(__OSX__)

        glDeleteVertexArrays(count, pointers);

#endif

#if defined(__IOS__) 

	#if defined(__OPENGL_30__)

        glDeleteVertexArrays(count, pointers);

	#else

        glDeleteVertexArraysOES(count, pointers);

	#endif

#endif

#if defined(__WIN32__)

		glDeleteVertexArrays(count, pointers);

#endif   

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_vertex_arrays(i32 count, ui32 *pointers)
    {
#if !defined(__NO_RENDER__)

#if defined(__OSX__)

        glGenVertexArrays(count, pointers);

#endif

#if defined(__IOS__) 

	#if defined(__OPENGL_30__)

        glGenVertexArrays(count, pointers);

	#else

        glGenVertexArraysOES(count, pointers);

	#endif

#endif

#if defined(__WIN32__)

		glGenVertexArrays(count, pointers);

#endif   

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_draw_elements(ui32 mode, i32 count, ui32 type, const void *indices)
    {
#if !defined(__NO_RENDER__)

        glDrawElements(mode, count, type, indices);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_draw_elements_instanced(ui32 mode, i32 count, ui32 type, const void *indices, i32 instancecount)
    {
#if !defined(__NO_RENDER__)

#if defined(__OSX__)

        glDrawElementsInstanced(mode, count, type, indices, instancecount);

#elif defined(__IOS__)

#if defined(__OPENGL_20__)

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
#if !defined(__NO_RENDER__)

        return glGetUniformLocation(program, (char *)name);
#else
		return -1;

#endif
    };
    
    inline i32 gl_get_attribute_location(ui32 program, const char *name)
    {
#if !defined(__NO_RENDER__)

        return glGetAttribLocation(program, (char *)name);

#else
		return -1;

#endif
    };
    
    inline void gl_get_uniform_matrix_3fv(i32 location, i32 count, ui8 transpose, const f32 *value)
    {
#if !defined(__NO_RENDER__)

        glUniformMatrix3fv(location, count, transpose, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_matrix_4fv(i32 location, i32 count, ui8 transpose, const f32 *value)
    {
#if !defined(__NO_RENDER__)

        glUniformMatrix4fv(location, count, transpose, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_2fv(i32 location, i32 count, const f32 *value)
    {
#if !defined(__NO_RENDER__)

        glUniform2fv(location, count, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_3fv(i32 location, i32 count, const f32 *value)
    {
#if !defined(__NO_RENDER__)
		
        glUniform3fv(location, count, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_4fv(i32 location, i32 count, const f32 *value)
    {
#if !defined(__NO_RENDER__)

        glUniform4fv(location, count, value);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_1f(i32 location, f32 v0)
    {
#if !defined(__NO_RENDER__)

        glUniform1f(location, v0);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_1i(i32 location, i32 v0)
    {
#if !defined(__NO_RENDER__)

        glUniform1i(location, v0);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_set_active_texture(ui32 texture)
    {
#if !defined(__NO_RENDER__)

        glActiveTexture(texture);

#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_use_program(i32 program)
    {
#if !defined(__NO_RENDER__)

        glUseProgram(program);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_stencil_function(ui32 function, i32 reference, ui32 mask)
    {
#if !defined(__NO_RENDER__)

        glStencilFunc(function, reference, mask);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_stencil_mask(ui32 mask)
    {
#if !defined(__NO_RENDER__)

        glStencilMask(mask);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_color_mask(ui8 red_flag, ui8 green_flag, ui8 blue_flag, ui8 alpha_flag)
    {
#if !defined(__NO_RENDER__)

        glColorMask(red_flag, green_flag, blue_flag, alpha_flag);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_color_mask(ui8 depth_flag)
    {
#if !defined(__NO_RENDER__)

        glDepthMask(depth_flag);

#endif

#if defined(DEBUG)
        gl_get_error();
#endif
    };
};

#endif
