//
//  shader_commiter_glsl.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/18/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shader_commiter_glsl.h"
#include "shader_compiler_glsl.h"
#include "resource.h"
#include "shader.h"

namespace gb
{
    shader_commiter_glsl::shader_commiter_glsl(const std::string& guid, const resource_shared_ptr& resource) :
    gb::resource_commiter(guid, resource)
    {
        
    }
    
    shader_commiter_glsl::~shader_commiter_glsl()
    {
        
    }
    
    ui32 shader_commiter_glsl::compile(const std::string &source_code, ui32 shader_type)
    {
        std::string out_message = "";
        bool out_success = false;
        ui32 handle = shader_compiler_glsl::compile(source_code, shader_type, &out_message, &out_success);
        
        if (out_success == false)
        {
			std::cout << "shader guid: " << m_guid << std::endl;
			std::cout << (shader_type == GL_VERTEX_SHADER ? "vs" : "fs") << std::endl;
            std::cout << out_message << std::endl;
            m_status = e_commiter_status_failure;
            assert(false);
        }
        return handle;
    }
    
    ui32 shader_commiter_glsl::link(ui32 vs_handle, ui32 fs_handle)
    {
        std::string out_message = "";
        bool out_success = false;
        ui32 handle = shader_compiler_glsl::link(vs_handle, fs_handle, &out_message, &out_success);
        
        if (out_success == false)
        {
			std::cout << "shader guid: " << m_guid << std::endl;
            std::cout<<out_message<<std::endl;
            m_status = e_commiter_status_failure;
            assert(false);
        }
        return handle;
    }
    
    void shader_commiter_glsl::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        shader_transfering_data_shared_ptr shader_transfering_data = std::static_pointer_cast<gb::shader_transfering_data>(transfering_data);

		ui32 shader_id = 0;

#if !defined(__NO_RENDER__)

        ui32 vs_handle = shader_commiter_glsl::compile(shader_transfering_data->m_vs_source_code, GL_VERTEX_SHADER);
        if(m_status == e_commiter_status_failure)
        {
            return;
        }

        ui32 fs_handle = shader_commiter_glsl::compile(shader_transfering_data->m_fs_source_code, GL_FRAGMENT_SHADER);
        if(m_status == e_commiter_status_failure)
        {
            return;
        }
        
        shader_id = shader_commiter_glsl::link(vs_handle, fs_handle);
        if(m_status == e_commiter_status_failure)
        {
            return;
        }

#endif

        assert(m_resource != nullptr);
        assert(m_resource->is_loaded() == true);

        m_status = m_status == e_commiter_status_in_progress ? e_commiter_status_success : e_commiter_status_failure;
        shader_transfering_data->m_shader_id = shader_id;
        resource_commiter::on_transfering_data_commited(shader_transfering_data);
    }
}