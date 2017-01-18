//
//  ces_network_system.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"
#include "net_declarations.h"
#include "command_processor.h"

namespace gb
{
    namespace net
    {
        class ces_network_system : public ces_base_system
        {
        private:
            
            command_processor_shared_ptr m_command_processor;
            
        protected:
            
            void on_feed_start(f32 deltatime);
            void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
            void on_feed_end(f32 deltatime);
            
            void update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime);
            
        public:
            
            CTTI_CLASS_GUID(ces_network_system, ces_base_system::g_guids_container)
            ces_network_system();
            ~ces_network_system();
            
            std::list<command_processor::command_callback_t>::iterator register_command_callback(i32 command_id,
                                                                                                 const command_processor::command_callback_t& callback);
            void unregister_command_callback(i32 command_id,
                                             const std::list<command_processor::command_callback_t>::iterator& iterator);
        };
    };
};
