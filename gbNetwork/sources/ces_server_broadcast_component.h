//
//  ces_server_broadcast_component.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class ces_server_broadcast_component_pimpl;
        class ces_server_broadcast_component : public ces_base_component
        {
        private:
            
            std::shared_ptr<ces_server_broadcast_component_pimpl> m_pimpl;
            std::thread m_thread;
            
        protected:
            
            void broadcast();
            
        public:
            
            CTTI_CLASS_GUID(ces_server_broadcast_component, ces_base_component::g_guids_container)
            ces_server_broadcast_component();
            ~ces_server_broadcast_component();
            
            void start();
            void stop();
        };
    };
};
