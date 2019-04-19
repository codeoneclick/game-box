//
//  fcl_script.cpp
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "fcl_script.h"
#include "resource_status.h"

namespace gb
{
    fcl_script_transfering_data::fcl_script_transfering_data(i32 id) :
    m_id(id)
    {
        m_type = e_resource_transfering_data_type_fcl_script;
    }
    
    fcl_script_transfering_data::~fcl_script_transfering_data()
    {
    }
    
    i32 fcl_script_transfering_data::get_id() const
    {
        return m_id;
    }
    
    
    fcl_script::fcl_script(const std::string& guid) :
    resource(e_resource_type_fcl_script, guid),
    m_fcl_script_data(nullptr)
    {
        
    }
    
    fcl_script_shared_ptr fcl_script::construct(const std::string& guid,
                                                const fcl_script_transfering_data_shared_ptr& data)
    {
        fcl_script_shared_ptr fcl_script_resource = std::make_shared<fcl_script>(guid);
        fcl_script_resource->on_transfering_data_serialized(data);
        fcl_script_resource->on_transfering_data_commited(data);
        return fcl_script_resource;
    }
    
    fcl_script::~fcl_script()
    {
        
    }
    
    void fcl_script::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_fcl_script:
            {
                m_fcl_script_data = std::static_pointer_cast<fcl_script_transfering_data>(data);
                m_status |= e_resource_status_loaded;
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    void fcl_script::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_fcl_script:
            {
                m_status |= e_resource_status_commited;
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    i32 fcl_script::get_id() const
    {
        return resource::is_loaded() ? m_fcl_script_data->get_id() : -1;
    }
}
