//
//  ces_network_system.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_network_system.h"
#include "ces_server_component.h"
#include "ces_client_component.h"
#include "ces_server_broadcast_component.h"
#include "ces_client_broadcast_component.h"
#include "connection.h"
#include "command_character_move.h"
#include "command_processor.h"

namespace gb
{
    namespace net
    {
        ces_network_system::ces_network_system()
        {
            m_command_processor = std::make_shared<command_processor>();
            m_command_processor->register_command_creator(1, std::bind(&command_character_move::create, std::placeholders::_1));
        }
        
        ces_network_system::~ces_network_system()
        {
            
        }
        
        void ces_network_system::on_feed_start(f32 deltatime)
        {
            
        }
        
        void ces_network_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            ces_network_system::update_recursively(entity, deltatime);
        }
        
        void ces_network_system::on_feed_end(f32 deltatime)
        {
            
        }
        
        void ces_network_system::update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            ces_client_broadcast_component_shared_ptr client_endpoint_component = entity->get_component<ces_client_broadcast_component>();
            if(client_endpoint_component)
            {
                if(client_endpoint_component->get_endpoint_find_callback())
                {
                    client_endpoint_component->get_endpoint_find_callback()(client_endpoint_component->get_endpoints(), entity);
                }
            }
            
            ces_client_component_shared_ptr client_component = entity->get_component<ces_client_component>();
            if(client_component)
            {
                static f32 angle = 0.f;
                angle += 0.1f;
                command_character_move_shared_ptr command = std::make_shared<command_character_move>(angle);
                connection_shared_ptr connection = client_component->get_connection();
                connection->send_command(command);
            }
            
            ces_server_component_shared_ptr server_component = entity->get_component<ces_server_component>();
            if(server_component)
            {
                std::queue<std::string> log_messages = server_component->get_log_messages();
                if(server_component->get_log_callback() && log_messages.size() != 0)
                {
                    while(!log_messages.empty())
                    {
                        server_component->get_log_callback()(log_messages.front(), entity);
                        log_messages.pop();
                    }
                }
            }
            
            ces_server_broadcast_component_shared_ptr server_broadcast_component = entity->get_component<ces_server_broadcast_component>();
            if(server_broadcast_component)
            {
                std::queue<std::string> log_messages = server_broadcast_component->get_log_messages();
                if(server_broadcast_component->get_log_callback() && log_messages.size() != 0)
                {
                    while(!log_messages.empty())
                    {
                        server_broadcast_component->get_log_callback()(log_messages.front(), entity);
                        log_messages.pop();
                    }
                }
            }
            
            std::list<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_network_system::update_recursively(child, deltatime);
            }
        }
    }
}
