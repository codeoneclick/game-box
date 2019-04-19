//
//  fcl_script.h
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"

namespace gb
{
    class fcl_script_transfering_data final : public resource_transfering_data
    {
    private:
        
    protected:
        
        i32 m_id = -1;
        
    public:
        
        fcl_script_transfering_data(i32 id);
        ~fcl_script_transfering_data();
        
        i32 get_id() const;
    };
    
    class fcl_script : public resource
    {
    private:
        
    protected:
        
        fcl_script_transfering_data_shared_ptr m_fcl_script_data = nullptr;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
    public:
        
        fcl_script(const std::string& guid);
        
        static fcl_script_shared_ptr construct(const std::string& guid,
                                               const fcl_script_transfering_data_shared_ptr& data);
        ~fcl_script();
        
        i32 get_id() const;
    };
};


