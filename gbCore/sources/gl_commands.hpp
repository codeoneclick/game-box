//
//  glCommands.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef gl_commands_h
#define gl_commands_h

#if defined(__OSX__)

#define __OPENGL_30__ 1

#else

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

#elif defined(__OSX__)

#include <OpenGL/OpenGL.h>

#if defined(__OPENGL_20__)

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#elif defined(__OPENGL_30__)

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#endif

#endif

#define CASE_STRING_VALUE(GL_ERROR, STRING_ERROR) case GL_ERROR: STRING_ERROR = #GL_ERROR; break;

namespace gb
{
    inline GLenum gl_get_error(void)
    {
        GLenum error = glGetError();
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
                char buffer[32] = { };
                sprintf(buffer, "%u", error);
                string_error = "UNKNOWN OpenGL ERROR" + std::string(buffer);
            }
        }
        if(string_error.length() != 0)
        {
            std::cout<<"OpenGL error: "<<string_error<<std::endl;
        }
        return error;
    };
    
    inline void gl_enable(GLenum caption)
    {
        glEnable(caption);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_disable(GLenum caption)
    {
        glDisable(caption);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_depth_function(GLenum function)
    {
        glDepthFunc(function);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_depth_mask(GLboolean flag)
    {
        glDepthMask(flag);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_cull_face(GLenum mode)
    {
        glCullFace(mode);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_blend_function(GLenum source_factor, GLenum destination_factor)
    {
        glBlendFunc(source_factor, destination_factor);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_clear(GLbitfield mask)
    {
        glClear(mask);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_clear_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
    {
        glClearColor(red, green, blue, alpha);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_viewport(int x, int y, GLsizei width, GLsizei height)
    {
        glViewport(x, y, width, height);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_render_buffers(GLsizei counts, const GLuint *pointers)
    {
        glDeleteRenderbuffers(counts, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_render_buffers(GLsizei count, GLuint *pointers)
    {
        glGenRenderbuffers(count, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_render_buffer_storage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
    {
        glRenderbufferStorage(target, internalformat, width, height);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_frame_buffer(GLenum target, GLuint framebuffer)
    {
        glBindFramebuffer(target, framebuffer);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_render_buffer(GLenum target, GLuint renderbuffer)
    {
        glBindRenderbuffer(target, renderbuffer);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_frame_buffers(GLsizei count, const GLuint *pointers)
    {
        glDeleteFramebuffers(count, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_frame_buffers(GLsizei count, GLuint *pointers)
    {
        glGenFramebuffers(count, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_attach_frame_buffer_texture2d(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
    {
        glFramebufferTexture2D(target, attachment, textarget, texture, level);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_attach_frame_buffer_render_buffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
    {
        glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_textures(GLsizei count, GLuint *pointers)
    {
        glGenTextures(count, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_textures(GLsizei count, const GLuint *pointers)
    {
        glDeleteTextures(count, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_texture(uint32_t target, uint32_t texture)
    {
        glBindTexture(target, texture);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_texture_parameter_i(GLenum target, GLenum name, GLint parameter)
    {
        glTexParameteri(target, name, parameter);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_texture_image2d(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
    {
        glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_compressed_texture_image2d(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
    {
        glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }
    
    inline void gl_generate_mipmap(GLenum target)
    {
        glGenerateMipmap(target);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_buffers(GLsizei count, GLuint *pointers)
    {
        glGenBuffers(count, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_buffers(GLsizei count, const GLuint *pointers)
    {
        glDeleteBuffers(count, pointers);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_buffer(uint32_t target, uint32_t buffer)
    {
        glBindBuffer(target, buffer);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_push_buffer_data(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)
    {
        glBufferData(target, size, data, usage);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_push_buffer_sub_data(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
    {
        glBufferSubData(target, offset, size, data);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_enable_vertex_attribute(GLuint index)
    {
        glEnableVertexAttribArray(index);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_disable_vertex_attribute(GLuint index)
    {
        glDisableVertexAttribArray(index);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_bind_vertex_attribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)
    {
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_set_vertex_attribute_divisor(GLuint index, GLuint divisor)
    {
#if defined(__OSX__)
        
        glVertexAttribDivisor(index, divisor);
        
#elif defined(__IOS__)
#if defined(__OPENGL_20__)
        
        glVertexAttribDivisorEXT(index, divisor);
#else
        
        glVertexAttribDivisor(index, divisor);
        
#endif
#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }
    
    inline void gl_bind_vertex_array(GLuint array)
    {
#if defined(__OSX__)
        glBindVertexArray(array);
#elif defined(__IOS__) && defined(__OPENGL_30__)
        glBindVertexArray(array);
#else
        glBindVertexArrayOES(array);
#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_delete_vertex_arrays(GLsizei count, const GLuint *pointers)
    {
#if defined(__OSX__)
        glDeleteVertexArrays(count, pointers);
#elif defined(__IOS__) && defined(__OPENGL_30__)
        glDeleteVertexArrays(count, pointers);
#else
        glDeleteVertexArraysOES(count, pointers);
#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_create_vertex_arrays(GLsizei count, GLuint *pointers)
    {
#if defined(__OSX__)
        glGenVertexArrays(count, pointers);
#elif defined(__IOS__) && defined(__OPENGL_30__)
        glGenVertexArrays(count, pointers);
#else
        glGenVertexArraysOES(count, pointers);
#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_draw_elements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
    {
        glDrawElements(mode, count, type, indices);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei instancecount)
    {
#if defined(__OSX__)
        
        glDrawElementsInstanced(mode, count, type, indices, instancecount);
        
#elif defined(__IOS__)
#if defined(__OPENGL_20__)
        
        glDrawElementsInstancedEXT(mode, count, type, indices, instancecount);
        
#else
        
        glDrawElementsInstanced(mode, count, type, indices, instancecount);
        
#endif
#endif
        
#if defined(DEBUG)
        gl_get_error();
#endif
    }
    
    inline GLint gl_get_uniform_location(GLuint program, const GLchar *name)
    {
        return glGetUniformLocation(program, name);
    };
    
    inline GLint gl_get_attribute_location(GLuint program, const GLchar *name)
    {
        return glGetAttribLocation(program, name);
    };
    
    inline void gl_get_uniform_matrix_3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
    {
        glUniformMatrix3fv(location, count, transpose, value);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_matrix_4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
    {
        glUniformMatrix4fv(location, count, transpose, value);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_2fv(GLint location, GLsizei count, const GLfloat *value)
    {
        glUniform2fv(location, count, value);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_3fv(GLint location, GLsizei count, const GLfloat *value)
    {
        glUniform3fv(location, count, value);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_vector_4fv(GLint location, GLsizei count, const GLfloat *value)
    {
        glUniform4fv(location, count, value);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_1f(GLint location, GLfloat v0)
    {
        glUniform1f(location, v0);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_get_uniform_1i(GLint location, GLint v0)
    {
        glUniform1i(location, v0);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_set_active_texture(GLenum texture)
    {
        glActiveTexture(texture);
        
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_use_program(GLuint program)
    {
        glUseProgram(program);
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_stencil_function(GLenum function, GLint reference, GLuint mask)
    {
        glStencilFunc(function, reference, mask);
#if defined(DEBUG)
        gl_get_error();
#endif
    };
    
    inline void gl_stencil_mask(GLuint mask)
    {
        glStencilMask(mask);
#if defined(DEBUG)
        gl_get_error();
#endif
    };
};

#endif
