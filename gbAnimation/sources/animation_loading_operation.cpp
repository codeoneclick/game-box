//
//  animation_loading_operation.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "animation_loading_operation.h"
#include "animation.h"
#include "animation_serializer_ani.h"
#include "animation_commiter_ani.h"

namespace gb
{
    namespace anim
    {
        animation_loading_operation::animation_loading_operation(const std::string& filename, const resource_shared_ptr& resource) :
        gb::resource_loading_operation(filename, resource),
        m_filename(filename)
        {
            m_transfering_data = std::make_shared<animation_transfering_data>();
        }
        
        animation_loading_operation::~animation_loading_operation()
        {
            
        }
        
        void animation_loading_operation::serialize()
        {
            assert(m_resource);
            m_status = e_resource_loading_operation_status_in_progress;
            
            if(m_filename.find(".ani") != std::string::npos)
            {
                m_serializer = std::make_shared<animation_serializer_ani>(m_filename, m_resource);
            }
            else
            {
                assert(false);
            }
            
            m_serializer->serialize(m_transfering_data);
            m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
        }
        
        void animation_loading_operation::commit()
        {
            if(m_filename.find(".ani") != std::string::npos)
            {
                m_commiter = std::make_shared<animation_commiter_ani>(m_serializer->get_guid(), m_resource);
            }
            else
            {
                assert(false);
            }
            
            m_commiter->commit(m_transfering_data);
            m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
        }
    }
}
