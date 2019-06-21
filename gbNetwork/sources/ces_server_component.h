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
#include "ces_net_log_component_extension.h"

namespace gb
{
    namespace net
    {
        class ces_server_component_pimpl;
        class ces_server_component : public ces_base_component, public ces_net_log_component_extension
        {
        public:
            
            typedef std::function<void(ui32)> connection_established_callback_t;
            typedef std::function<void(ui32)> connection_closed_callback_t;
            
        private:
     
            std::shared_ptr<ces_server_component_pimpl> m_pimpl;
            std::thread m_thread;
            mutable std::recursive_mutex m_connections_mutex;
            static i32 g_connection_udid;
            
        protected:
            
            std::map<i32, connection_shared_ptr> m_connections;
            connection_established_callback_t m_connection_established_callback;
            connection_closed_callback_t m_connection_closed_callback;
            
            void listen_connections();
            
        public:
            
            STTI_CLASS_GUID(ces_server_component, ces_base_component::g_guids_container)
            ces_server_component();
            ~ces_server_component();
            
            void update_connection_status();
            
            void start();
            void stop();
            
            void set_connection_established_callback(const connection_established_callback_t& callback);
            void set_connection_closed_callback(const connection_closed_callback_t& callback);
            
            void send_command(command_const_shared_ptr command, i32 udid = -1);
            
            std::map<i32, connection_shared_ptr> get_connections() const;
            connection_shared_ptr get_connection(i32 udid) const;
        };
    };
};
