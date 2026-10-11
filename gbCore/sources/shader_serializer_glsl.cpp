//
//  shader_serializer_glsl.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shader_serializer_glsl.h"
#include "shader.h"
#include "shader_compiler_glsl.h"

namespace gb
{
    shader_serializer_glsl::shader_serializer_glsl(const std::string& vs_filename,
                                                   const std::string& fs_filename,
                                                   const resource_shared_ptr& resource) :
    gb::resource_serializer(std::string().append(vs_filename).append(fs_filename), resource),
    m_vs_filename(vs_filename),
    m_fs_filename(fs_filename)
    {
        
    }
    
    shader_serializer_glsl::~shader_serializer_glsl()
    {
        
    }
    
    void shader_serializer_glsl::serialize(const resource_transfering_data_shared_ptr& transfering_data)
    {
        assert(m_resource);
        m_status = e_serializer_status_in_progress;
        shader_transfering_data_shared_ptr shader_transfering_data = std::static_pointer_cast<gb::shader_transfering_data>(transfering_data);
        
#if USED_GRAPHICS_API == METAL_API
        
        shader_transfering_data->m_vs_source_code = "";
        shader_transfering_data->m_fs_source_code = "";
        
#else
        
        std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(m_vs_filename, &m_status);
        if(m_status == e_serializer_status_failure) return;
        std::stringstream vs_string_stream;
        vs_string_stream<<filestream->rdbuf();
        std::string vs_source_code(vs_string_stream.str());
        resource_serializer::close_stream(filestream);
        
        filestream = resource_serializer::open_stream(m_fs_filename, &m_status);
        if(m_status == e_serializer_status_failure) return;
        std::stringstream fs_string_stream;
        fs_string_stream<<filestream->rdbuf();
        std::string fs_source_code(fs_string_stream.str());
        resource_serializer::close_stream(filestream);
        
        shader_transfering_data->m_vs_source_code = std::move(vs_source_code);
        shader_transfering_data->m_fs_source_code = std::move(fs_source_code);

#if USED_GRAPHICS_API == VULKAN_API
        std::string message;
        if(!shader_compiler_glsl::precompile(shader_transfering_data->m_vs_source_code, gl::constant::vertex_shader, &message) ||
           !shader_compiler_glsl::precompile(shader_transfering_data->m_fs_source_code, gl::constant::fragment_shader, &message))
        {
            std::cerr << "shader compile failed: " << m_vs_filename << ", " << m_fs_filename << ": " << message << std::endl;
            m_status = e_serializer_status_failure;
            return;
        }
#endif
        
#endif
        
        m_status = e_serializer_status_success;
        resource_serializer::on_transfering_data_serialized(shader_transfering_data);
    }
}
