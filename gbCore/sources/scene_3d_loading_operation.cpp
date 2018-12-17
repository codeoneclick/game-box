//
//  scene_3d_loading_operation.cpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#include "scene_3d_loading_operation.h"
#include "scene_3d.h"
#include "scene_3d_serializer_gbscene3d.h"
#include "scene_3d_commiter_gbscene3d.h"

namespace gb
{
    scene_3d_loading_operation::scene_3d_loading_operation(const std::string& filename, const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        m_transfering_data = nullptr;
    }
    
    scene_3d_loading_operation::~scene_3d_loading_operation()
    {
        
    }
    
    void scene_3d_loading_operation::serialize()
    {
        assert(m_resource);
        m_status = e_resource_loading_operation_status_in_progress;
        
        if(m_filename.find(".GB3D_SCENE") != std::string::npos)
        {
            m_serializer = std::make_shared<scene_3d_serializer_gbscene3d>(m_filename, m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_serializer->serialize(m_transfering_data);
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void scene_3d_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        
        if(m_filename.find(".GB3D_SCENE") != std::string::npos)
        {
            m_commiter = std::make_shared<scene_3d_commiter_gbscene3d>(m_serializer->get_guid(), m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_commiter->commit(m_transfering_data);
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}
