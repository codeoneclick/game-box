//
//  ces_net_log_component_extension.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/19/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_net_log_component_extension.h"

namespace gb
{
    namespace net
    {
        ces_net_log_component_extension::ces_net_log_component_extension() :
        m_log_callback(nullptr)
        {
            
        }
        
        ces_net_log_component_extension::~ces_net_log_component_extension()
        {
            
        }
        
        void ces_net_log_component_extension::log(const std::string& message)
        {
            auto time = std::time(nullptr);
            std::ostringstream stream;
            stream<<std::put_time(std::localtime(&time), "[%d-%m-%Y %H:%M:%S] ");
            std::string message_with_time = stream.str();
            message_with_time.append(message);
            
            std::lock_guard<std::recursive_mutex> guard(m_mutex);
            m_log_messages.push(message_with_time);
        }

        void ces_net_log_component_extension::set_log_callback(const log_callback_t& callback)
        {
            m_log_callback = callback;
        }
        
        ces_net_log_component_extension::log_callback_t ces_net_log_component_extension::get_log_callback() const
        {
            return m_log_callback;
        }

        std::queue<std::string> ces_net_log_component_extension::get_log_messages()
        {
            std::lock_guard<std::recursive_mutex> guard(m_mutex);
            std::queue<std::string> log_messages = m_log_messages;
            while (!m_log_messages.empty())
            {
                m_log_messages.pop();
            }
            return std::move(log_messages);
        }
    }
}
