//
//  ces_net_endpoint_component_extension.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/19/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    namespace net
    {
        class ces_net_endpoint_component_extension
        {
        public:
            
            typedef std::function<void(std::set<std::string> endpoints, ces_entity_const_shared_ptr entity)> endpoint_find_callback_t;
            
        private:
            
            std::recursive_mutex m_mutex;
            std::set<std::string> m_endpoints;
            
            endpoint_find_callback_t m_endpoint_find_callback;
            
            
        protected:
            
            void add_endpoint(const std::string& endpoint);
            
        public:
            
            ces_net_endpoint_component_extension();
            ~ces_net_endpoint_component_extension();
            
            void set_endpoint_find_callback(const endpoint_find_callback_t& callback);
            endpoint_find_callback_t get_endpoint_find_callback() const;
            
            std::set<std::string> get_endpoints();
        };
    };
};
