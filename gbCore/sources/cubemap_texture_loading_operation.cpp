//
//  cubemap_texture_loading_operation.cpp
//  gbCore
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "cubemap_texture_loading_operation.h"
#include "cubemap_texture.h"
#include "cubemap_texture_serializer_png.h"
#include "cubemap_texture_commiter_png.h"

namespace gb
{
    cubemap_texture_loading_operation::cubemap_texture_loading_operation(const std::string& filename, const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        m_transfering_data = std::make_shared<cubemap_texture_transfering_data>();
    }
    
    void cubemap_texture_loading_operation::serialize()
    {
        assert(m_resource);
        m_status = e_resource_loading_operation_status_in_progress;
        m_serializer = std::make_shared<cubemap_texture_serializer_png>(m_filename, m_resource);
        m_serializer->serialize(m_transfering_data);
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void cubemap_texture_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        m_commiter = std::make_shared<cubemap_texture_commiter_png>(m_serializer->get_guid(), m_resource);
        m_commiter->commit(m_transfering_data);
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}
