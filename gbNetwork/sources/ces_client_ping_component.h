//
//  ces_client_ping_component.h
//  gbNetwork
//
//  Created by serhii serhiiv on 4/11/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class ces_client_ping_component_pimpl;
        class ces_client_ping_component : public ces_base_component
        {
        public:
            
            typedef std::function<void(const std::error_code&)> ping_callback_t;
            
        private:
            
            std::shared_ptr<ces_client_ping_component_pimpl> m_pimpl;
            
        protected:
            
            ping_connection_shared_ptr m_ping_connection;
            
        public:
            
            STTI_CLASS_GUID(ces_client_ping_component, ces_base_component::g_guids_container)
            ces_client_ping_component();
            ~ces_client_ping_component();
            
            void check(const std::string& ip, i32 port, const ping_callback_t& callback);
        };
    };
};
