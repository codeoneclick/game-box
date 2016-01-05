//
//  texture_loading_operation.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "texture_loading_operation.h"
#include "texture.h"
#include "texture_serializer_png.h"
#include "texture_commiter_png.h"

namespace gb
{
    texture_loading_operation::texture_loading_operation(const std::string& filename, const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        
    }
    
    texture_loading_operation::~texture_loading_operation()
    {
        
    }
    
    void texture_loading_operation::serialize()
    {
        assert(m_resource);
        m_status = e_resource_loading_operation_status_in_progress;
        
        if(m_filename.find(".pvr") != std::string::npos)
        {
            
        }
        else if(m_filename.find(".png") != std::string::npos)
        {
            m_serializer = std::make_shared<texture_serializer_png>(m_filename,
                                                                    m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_serializer->serialize();
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void texture_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        
        texture_shared_ptr texture = std::static_pointer_cast<gb::texture>(m_resource);
        
        if(m_filename.find(".pvr") != std::string::npos)
        {
            
        }
        else if(m_filename.find(".png") != std::string::npos)
        {
            m_commiter = std::make_shared<texture_commiter_png>(m_serializer->get_guid(),
                                                                m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_commiter->commit();
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}