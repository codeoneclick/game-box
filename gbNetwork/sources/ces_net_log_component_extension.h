//
//  ces_net_log_component_extension.h
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
        class ces_net_log_component_extension
        {
        public:
            
            typedef std::function<void(const std::string& message, ces_entity_const_shared_ptr entity)> log_callback_t;
            
        private:
            
            std::recursive_mutex m_mutex;
            std::queue<std::string> m_log_messages;
            
            log_callback_t m_log_callback;
            
            
        protected:
            
            void log(const std::string& message);
            
        public:
            
            ces_net_log_component_extension();
            ~ces_net_log_component_extension();
            
            void set_log_callback(const log_callback_t& callback);
            log_callback_t get_log_callback() const;
            
            std::queue<std::string> get_log_messages();
        };
    };
};

