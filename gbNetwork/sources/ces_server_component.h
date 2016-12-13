//
//  ces_server_component.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class ces_server_component_pimpl;
        class ces_server_component : public ces_base_component
        {
        private:
     
            std::shared_ptr<ces_server_component_pimpl> m_pimpl;
            std::thread m_thread;
            
        protected:
            
            std::list<connection_shared_ptr> m_connections;
            
            void listen_connections();
            void accept_connection(const std::error_code& ec);
            
        public:
            
            CTTI_CLASS_GUID(ces_server_component, ces_base_component::g_guids_container)
            ces_server_component();
            ~ces_server_component();
            
            void update_connection_status();
            
            void start();
            void stop();
        };
    };
};
