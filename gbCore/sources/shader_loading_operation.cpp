//
//  shader_loading_operation.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shader_loading_operation.h"
#include "shader.h"
#include "shader_serializer_glsl.h"
#include "shader_commiter_glsl.h"

namespace gb
{
    shader_loading_operation::shader_loading_operation(const std::string& vs_filename,
                                                       const std::string& fs_filename,
                                                       const resource_shared_ptr& resource) :
    gb::resource_loading_operation(std::string().append(vs_filename).append(fs_filename), resource),
    m_vs_filename(vs_filename),
    m_fs_filename(fs_filename)
    {
        
    }
    
    shader_loading_operation::~shader_loading_operation()
    {
        
    }
    
    void shader_loading_operation::serialize()
    {
        assert(m_resource != nullptr);
        m_status = e_resource_loading_operation_status_in_progress;
        m_serializer = std::make_shared<shader_serializer_glsl>(m_vs_filename,
                                                                m_fs_filename,
                                                                m_resource);
        m_serializer->serialize();
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void shader_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        
        shader_shared_ptr shader = std::static_pointer_cast<gb::shader>(m_resource);
        m_commiter = std::make_shared<shader_commiter_glsl>(m_serializer->get_guid(),
                                                            m_vs_filename,
                                                            m_fs_filename,
                                                            shader->get_vs_source_code(),
                                                            shader->get_fs_source_code(),
                                                            m_resource);
        m_commiter->commit();
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}