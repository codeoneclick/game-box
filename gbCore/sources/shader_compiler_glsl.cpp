//
//  shader_compiler_glsl.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shader_compiler_glsl.h"
#include "resource_serializer.h"
#include "vk_device.h"
#include "vk_initializers.h"

namespace gb
{
    std::string shader_compiler_glsl::m_vs_shader_header =
"#if defined(__IOS__) || defined(__TVOS__)\n\
    #extension GL_APPLE_clip_distance : require\n\
    #extension GL_EXT_draw_instanced : require\n\
    #define gl_InstanceID gl_InstanceIDEXT\n\
    precision highp float;\n\
#endif\n\
#if defined(USE_LAYOUTS)\n\
    layout (location = 0) in vec3 a_position;\n\
    layout (location = 1) in vec2 a_texcoord;\n\
    layout (location = 2) in vec4 a_normal;\n\
    layout (location = 3) in vec4 a_tangent;\n\
    layout (location = 4) in vec4 a_color;\n\
    layout (location = 5) in vec4 a_extra;\n\
    \n\
    layout(location = 1) out vec4 v_position;\n\
    layout(location = 2) out vec4 v_screen_position;\n\
    layout(location = 3) out vec2 v_texcoord;\n\
    layout(location = 4) out vec3 v_normal;\n\
    layout(location = 5) out vec3 v_tangent;\n\
    layout(location = 6) out vec3 v_binormal;\n\
    layout(location = 7) out vec4 v_color;\n\
    layout(location = 8) out mat3 v_mat_tbn;\n\
    \n\
    #if defined(VULKAN_API)\n\
        layout(binding = 0) uniform u_mat_m_struct { mat4 matrix; } u_mat_m;\n\
        layout(binding = 1) uniform u_mat_p_struct { mat4 matrix; } u_mat_p;\n\
        layout(binding = 2) uniform u_mat_v_struct { mat4 matrix; } u_mat_v;\n\
    #else\n\
        uniform mat4 u_mat_m;\n\
        uniform mat4 u_mat_p;\n\
        uniform mat4 u_mat_v;\n\
    #endif\n\
#else\n\
    attribute vec3 a_position;\n\
    attribute vec2 a_texcoord;\n\
    attribute vec4 a_normal;\n\
    attribute vec4 a_tangent;\n\
    attribute vec4 a_color;\n\
    attribute vec4 a_extra;\n\
    \n\
    varying vec4 v_position;\n\
    varying vec4 v_screen_position;\n\
    varying vec2 v_texcoord;\n\
    varying vec3 v_normal;\n\
    varying vec3 v_tangent;\n\
    varying vec3 v_binormal;\n\
    varying vec4 v_color;\n\
    varying mat3 v_mat_tbn;\n\
    \n\
    uniform mat4 u_mat_m;\n\
    uniform mat4 u_mat_p;\n\
    uniform mat4 u_mat_v;\n\
#endif\n\
mat4 get_mat_m(){\n\
#if defined(VULKAN_API)\n\
    return u_mat_m.matrix;\n\
#else\n\
    return u_mat_m;\n\
#endif\n\
}\n\
mat4 get_mat_v(){\n\
#if defined(VULKAN_API)\n\
    return u_mat_v.matrix;\n\
#else\n\
    return u_mat_v;\n\
#endif\n\
}\n\
mat4 get_mat_p(){\n\
#if defined(VULKAN_API)\n\
    return u_mat_p.matrix;\n\
#else\n\
    return u_mat_p;\n\
#endif\n\
}\n\
mat4 get_mat_mvp(){\n\
return get_mat_p() * get_mat_v() * get_mat_m();\n\
}\n\
vec4 get_pos_mvp() {\n\
return get_mat_mvp() * vec4(a_position, 1.0); \n\
}\n";

    std::string shader_compiler_glsl::m_fs_shader_header =
"#if defined(__IOS__) || defined(__TVOS__)\n\
    #extension GL_EXT_shadow_samplers : require\n\
    precision highp float;\n\
#endif\n\
#if defined(USE_LAYOUTS)\n\
    layout (location = 0) out vec4 attachment_01;\n\
    #define gl_FragColor attachment_01\n\
    #define texture2D texture\n\
    \n\
    layout(location = 1) in vec4 v_position;\n\
    layout(location = 2) in vec4 v_screen_position;\n\
    layout(location = 3) in vec2 v_texcoord;\n\
    layout(location = 4) in vec3 v_normal;\n\
    layout(location = 5) in vec3 v_tangent;\n\
    layout(location = 6) in vec3 v_binormal;\n\
    layout(location = 7) in vec4 v_color;\n\
    layout(location = 8) in mat3 v_mat_tbn;\n\
    \n\
    #if defined(USE_BINDINGS)\n\
        layout(binding = 0) uniform sampler2D sampler_01;\n\
        layout(binding = 1) uniform sampler2D sampler_02;\n\
        layout(binding = 2) uniform sampler2D sampler_03;\n\
        layout(binding = 3) uniform sampler2D sampler_04;\n\
        layout(binding = 4) uniform sampler2D sampler_05;\n\
        layout(binding = 5) uniform sampler2D sampler_06;\n\
        layout(binding = 6) uniform sampler2D sampler_07;\n\
        layout(binding = 7) uniform sampler2D sampler_08;\n\
    #else\n\
        uniform sampler2D sampler_01;\n\
        uniform sampler2D sampler_02;\n\
        uniform sampler2D sampler_03;\n\
        uniform sampler2D sampler_04;\n\
        uniform sampler2D sampler_05;\n\
        uniform sampler2D sampler_06;\n\
        uniform sampler2D sampler_07;\n\
        uniform sampler2D sampler_08;\n\
    #endif\n\
#else\n\
    varying vec4 v_position;\n\
    varying vec4 v_screen_position;\n\
    varying vec2 v_texcoord;\n\
    varying vec3 v_normal;\n\
    varying vec3 v_tangent;\n\
    varying vec3 v_binormal;\n\
    varying vec4 v_color;\n\
    varying mat3 v_mat_tbn;\n\
    \n\
    uniform sampler2D sampler_01;\n\
    uniform sampler2D sampler_02;\n\
    uniform sampler2D sampler_03;\n\
    uniform sampler2D sampler_04;\n\
    uniform sampler2D sampler_05;\n\
    uniform sampler2D sampler_06;\n\
    uniform sampler2D sampler_07;\n\
    uniform sampler2D sampler_08;\n\
#endif\n";
    
#if USED_GRAPHICS_API == VULKAN_API

	VkPipelineShaderStageCreateInfo shader_compiler_glsl::compile(const std::string& source_code, ui32 shader_type, std::string* out_message, bool* out_success)

#else

	ui32 shader_compiler_glsl::compile(const std::string& source_code, ui32 shader_type, std::string* out_message, bool* out_success)

#endif
    {

#if USED_GRAPHICS_API == VULKAN_API

		VkPipelineShaderStageCreateInfo handle = {};

#else

		ui32 handle = 0;

#endif

#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API

        handle = gl::command::create_shader(shader_type);

#endif
        
        std::string shader_header;
        
        if(shader_type == gl::constant::vertex_shader)
        {
            shader_header = m_vs_shader_header;
        }
        else if(shader_type == gl::constant::fragment_shader)
        {
            shader_header = m_fs_shader_header;
        }
        
        std::string define = "";
#if USED_GRAPHICS_API == OPENGL_30_API
        
#if defined(__OSX__)
        
        define.append("#version 410\n");
        
#elif defined(__IOS__) || defined(__TVOS__)
        
        define.append("#version 300 es\n");

#elif defined(__WINOS__)

		define.append("#version 420\n");
        define.append("#define USE_BINDINGS\n");
        
#endif

#endif
        
#if defined(__OSX__)
        
        define.append("#define __OSX__\n");
        
#elif defined(__IOS__) || defined(__TVOS__)
        
        define.append("#define __IOS__\n");
        
#endif
        
#if USED_GRAPHICS_API == OPENGL_30_API
        
		define.append("#define USE_LAYOUTS\n");
        
#endif

#if USED_GRAPHICS_API == VULKAN_API

		define.append("#define VULKAN_API\n");

#endif
        
        define.append(shader_header);
        
#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API

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

#elif USED_GRAPHICS_API == VULKAN_API

		std::string source_code_spv = define;
		source_code_spv.append(source_code);

		shaderc::Compiler compiler;
		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source_code_spv.c_str(), source_code_spv.length(), shader_type == gl::constant::vertex_shader ? shaderc_glsl_vertex_shader : shaderc_glsl_fragment_shader, "shader");
		if (module.GetCompilationStatus() != shaderc_compilation_status_success)
		{
			if (out_success)
			{
				*out_success = false;
			}
			if (out_message)
			{
				*out_message = module.GetErrorMessage();
			}
		}
		else
		{
			std::vector<ui32> spirv = { module.cbegin(), module.cend() };

			VkShaderModuleCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			create_info.codeSize = spirv.size() * 4;
			create_info.pCode = (ui32*)spirv.data();

			VkDevice device = vk_device::get_instance()->get_logical_device();
			VkShaderModule shader_module;
			if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
			{
				if (out_success)
				{
					*out_success = false;
				}
				if (out_message)
				{
					*out_message = "failed to create shader module";
				}
			}
			else
			{
				handle.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				handle.stage = shader_type == gl::constant::vertex_shader ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
				handle.module = shader_module;
				handle.pName = "main";
			}
		}

#else
        
		if (out_success)
		{
			*out_success = true;
		}
        
#endif

        return handle;
    }
    
    ui32 shader_compiler_glsl::link(ui32 vs_handle, ui32 fs_handle, std::string* out_message, bool* out_success)
    {
		ui32 handle = 0;

#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API

        handle = glCreateProgram();
        glAttachShader(handle, vs_handle);
        glAttachShader(handle, fs_handle);
        glLinkProgram(handle);
        
        i32 success;
        glGetProgramiv(handle, GL_LINK_STATUS, &success);
        
        if(!success)
        {
            i32 message_size = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &message_size);
            GLchar *message_string = new GLchar[message_size];
            memset(message_string, NULL, message_size * sizeof(GLchar));
            glGetProgramInfoLog(handle, message_size, NULL, message_string);
            if(out_message)
            {
                *out_message = message_string;
            }
        }
        if(out_success)
        {
            *out_success = success;
        }

#else

		if (out_success)
		{
			*out_success = true;
		}

#endif

        return handle;
    }
}
