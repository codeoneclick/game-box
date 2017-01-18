//
//  ces_client_component.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "net_declarations.h"
#include "ces_net_log_component_extension.h"

namespace gb
{
    namespace net
    {
        class ces_client_component_pimpl;
        class ces_client_component : public ces_base_component, public ces_net_log_component_extension
        {
        private:
            
            std::shared_ptr<ces_client_component_pimpl> m_pimpl;
            
        protected:

            connection_shared_ptr m_connection;
            
        public:
            
            CTTI_CLASS_GUID(ces_client_component, ces_base_component::g_guids_container)
            ces_client_component();
            ~ces_client_component();
            
            void connect(const std::string& ip, i32 port);
            void disconnect();
            
            connection_shared_ptr get_connection() const;
            
            void send_command(command_const_shared_ptr command);
        };
    };
};

