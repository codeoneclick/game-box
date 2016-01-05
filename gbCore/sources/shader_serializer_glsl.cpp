//
//  shader_serializer_glsl.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shader_serializer_glsl.h"
#include "shader.h"

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
    
    void shader_serializer_glsl::serialize()
    {
        assert(m_resource);
        m_status = e_serializer_status_in_progress;
        
        std::shared_ptr<std::istream> filestream = resource_serializer::open_stream(m_vs_filename, &m_status);
        std::stringstream vs_string_stream;
        vs_string_stream<<filestream->rdbuf();
        std::string vs_source_code(vs_string_stream.str());
        resource_serializer::close_stream(filestream);
        
        filestream = resource_serializer::open_stream(m_fs_filename, &m_status);
        std::stringstream fs_string_stream;
        fs_string_stream<<filestream->rdbuf();
        std::string fs_source_code(fs_string_stream.str());
        resource_serializer::close_stream(filestream);
        
        shader_shared_ptr shader = std::static_pointer_cast<gb::shader>(m_resource);
        shader_transfering_data_shared_ptr data = std::make_shared<gb::shader_transfering_data>();
        data->m_vs_filename = m_vs_filename;
        data->m_fs_filename = m_fs_filename;
        data->m_vs_source_code = vs_source_code;
        data->m_fs_source_code = fs_source_code;
        
        m_status = e_serializer_status_success;
        resource_serializer::on_transfering_data_serialized(data);
    }
}