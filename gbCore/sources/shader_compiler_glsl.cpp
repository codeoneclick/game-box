//
//  shader_compiler_glsl.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shader_compiler_glsl.h"
#include "resource_serializer.h"

namespace gb
{
    std::string shader_compiler_glsl::m_vs_shader_header = "#if defined(__IOS__)\n\
    #extension GL_APPLE_clip_distance : require\n\
    #extension GL_EXT_draw_instanced : require\n\
    #define gl_InstanceID gl_InstanceIDEXT\n\
    precision highp float;\n\
    #endif\n\
    #if defined(__OPENGL_30__)\n\
    layout (location = 0) in vec3 a_position;\n\
    layout (location = 1) in vec2 a_texcoord;\n\
    layout (location = 2) in vec4 a_normal;\n\
    layout (location = 3) in vec4 a_tangent;\n\
    layout (location = 4) in vec4 a_color;\n\
    layout (location = 5) in vec4 a_extra;\n\
    #else\n\
    attribute vec3 a_position;\n\
    attribute vec2 a_texcoord;\n\
    attribute vec4 a_normal;\n\
    attribute vec4 a_tangent;\n\
    attribute vec4 a_color;\n\
    attribute vec4 a_extra;\n\
    #endif\n";
    
    std::string shader_compiler_glsl::m_fs_shader_header = "#if defined(__IOS__)\n\
    #extension GL_EXT_shadow_samplers : require\n\
    precision highp float;\n\
    #endif\n\
    #if defined(__OPENGL_30__)\n\
    layout (location = 0) out vec4 attachment_01;\n\
    #define gl_FragColor attachment_01\n\
    #define texture2D texture\n\
    #define textureCube texture\n\
    #endif\n";
    
    ui32 shader_compiler_glsl::compile(const std::string& source_code, GLenum shader_type, std::string* out_message, bool* out_success)
    {
        ui32 handle = glCreateShader(shader_type);
        
        std::string shader_header;
        if(shader_type == GL_VERTEX_SHADER)
        {
            shader_header = m_vs_shader_header;
        }
        else if(shader_type == GL_FRAGMENT_SHADER)
        {
            shader_header = m_fs_shader_header;
        }
        
        std::string define = "";
#if defined(__OPENGL_30__)
        
#if defined(__OSX__)
        
        define.append("#version 410\n");
        
#elif defined(__IOS__)
        
        define.append("#version 300 es\n");
        
#endif
#endif
        
#if defined(__OSX__)
        
        define.append("#define __OSX__\n");
        
#elif defined(__IOS__)
        
        define.append("#define __IOS__\n");
        
#endif
        
#if defined(__OPENGL_30__)
        
        define.append("#define __OPENGL_30__\n");
        
#endif
        
        define.append(shader_header);
        
        char* shader_data = const_cast<char*>(source_code.c_str());
        char* define_data = const_cast<char*>(define.c_str());
        char* sources[2] = { define_data, shader_data};
        glShaderSource(handle, 2, sources, NULL);
        glCompileShader(handle);
        
        i32 success;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
        
        if(!success)
        {
            i32 message_size = 0;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &message_size);
            GLchar *message_string = new GLchar[message_size];
            memset(message_string, NULL, message_size * sizeof(GLchar));
            glGetShaderInfoLog(handle, message_size, NULL, message_string);
            if(out_message)
            {
                *out_message = message_string;
            }
        }
        if(out_success)
        {
            *out_success = success;
        }
        return handle;
    }
    
    ui32 shader_compiler_glsl::link(ui32 vs_handle, ui32 fs_handle, std::string* out_message, bool* out_success)
    {
        ui32 handle = glCreateProgram();
        glAttachShader(handle, vs_handle);
        glAttachShader(handle, fs_handle);
        glLinkProgram(handle);
        
        i32 success;
        glGetProgramiv(handle, GL_LINK_STATUS, &success);
        
        if(!success)
        {
            i32 message_size = 0;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &message_size);
            GLchar *message_string = new GLchar[message_size];
            memset(message_string, NULL, message_size * sizeof(GLchar));
            glGetShaderInfoLog(handle, message_size, NULL, message_string);
            if(out_message)
            {
                *out_message = message_string;
            }
        }
        if(out_success)
        {
            *out_success = success;
        }
        return handle;
    }
}