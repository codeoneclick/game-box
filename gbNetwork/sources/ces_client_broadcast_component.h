//
//  ces_client_broadcast_component.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "net_declarations.h"
#include "ces_net_endpoint_component_extension.h"

namespace gb
{
    namespace net
    {
        class ces_client_broadcast_component_pimpl;
        class ces_client_broadcast_component : public ces_base_component, public ces_net_endpoint_component_extension
        {
        private:
            
            std::shared_ptr<ces_client_broadcast_component_pimpl> m_pimpl;
            std::thread m_thread;
            bool m_is_working;
            
        protected:
            
            void update();
            
        public:
            
            CTTI_CLASS_GUID(ces_client_broadcast_component, ces_base_component::g_guids_container)
            ces_client_broadcast_component();
            ~ces_client_broadcast_component();
            
            void start();
            void stop();
        };
    };
};
