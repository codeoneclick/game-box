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
    static const std::string k_vs_extension = ".vert";
    static const std::string k_fs_extension = ".frag";
    
    shader_loading_operation::shader_loading_operation(const std::string& filename,
                                                       const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        m_transfering_data = std::make_shared<shader_transfering_data>();
    }
    
    shader_loading_operation::~shader_loading_operation()
    {
        
    }
    
    void shader_loading_operation::serialize()
    {
        assert(m_resource);
        m_status = e_resource_loading_operation_status_in_progress;
        m_serializer = std::make_shared<shader_serializer_glsl>(m_filename + k_vs_extension,
                                                                m_filename + k_fs_extension,
                                                                m_resource);
        m_serializer->serialize(m_transfering_data);
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void shader_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        
        shader_shared_ptr shader = std::static_pointer_cast<gb::shader>(m_resource);
        m_commiter = std::make_shared<shader_commiter_glsl>(m_serializer->get_guid(), m_resource);
        m_commiter->commit(m_transfering_data);
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}
