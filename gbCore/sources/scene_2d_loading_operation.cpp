//
//  scene_2d_loading_operation.cpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "scene_2d_loading_operation.h"
#include "scene_2d.h"
#include "scene_2d_serializer_tmx.h"
#include "scene_2d_commiter_tmx.h"

namespace gb
{
    scene_2d_loading_operation::scene_2d_loading_operation(const std::string& filename, const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        m_transfering_data = nullptr;
    }
    
    scene_2d_loading_operation::~scene_2d_loading_operation()
    {
        
    }
    
    void scene_2d_loading_operation::serialize()
    {
        assert(m_resource);
        m_status = e_resource_loading_operation_status_in_progress;
        
        if(m_filename.find(".tmx") != std::string::npos)
        {
            m_serializer = std::make_shared<scene_2d_serializer_tmx>(m_filename, m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_serializer->serialize(m_transfering_data);
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void scene_2d_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        
        if(m_filename.find(".tmx") != std::string::npos)
        {
            m_commiter = std::make_shared<scene_2d_commiter_tmx>(m_serializer->get_guid(), m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_commiter->commit(m_transfering_data);
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}
