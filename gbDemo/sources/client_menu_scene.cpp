//
//  client_menu_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_menu_scene.h"
#include "ns_ui_commands.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "console.h"
#include "table_view.h"
#include "table_view_cell.h"
#include "game_commands_container.h"
#include "ces_client_component.h"
#include "ces_client_broadcast_component.h"
#include "ces_network_system.h"

namespace ns
{
    
    class endpoint_table_view_cell_data : public gb::ui::table_view_cell_data
    {
    private:
        
    protected:
        
        std::string m_endpoint;
        
    public:
        
        endpoint_table_view_cell_data(const std::string& endpoint);
        
        std::string get_endpoint() const;
    };
    
    endpoint_table_view_cell_data::endpoint_table_view_cell_data(const std::string& endpoint) :
    m_endpoint(endpoint)
    {
        
    }
    
    std::string endpoint_table_view_cell_data::get_endpoint() const
    {
        return m_endpoint;
    }
    
    client_menu_scene::client_menu_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    client_menu_scene::~client_menu_scene()
    {
        
    }
    
    void client_menu_scene::create()
    {
        gb::scene_graph::create();
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(client_menu_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(client_menu_scene::get_transition()->get_screen_width(),
                                                client_menu_scene::get_transition()->get_screen_height());
        client_menu_scene::set_camera(m_camera);
        
        client_menu_scene::get_transition()->add_system(std::make_shared<gb::net::ces_network_system>());
        
        gb::net::ces_client_broadcast_component_shared_ptr client_broadcast_component = std::make_shared<gb::net::ces_client_broadcast_component>();
        client_broadcast_component->start();
        client_broadcast_component->set_endpoint_find_callback(std::bind(&client_menu_scene::on_endpoints_received, this,
                                                                         std::placeholders::_1, std::placeholders::_2));
        client_menu_scene::add_component(client_broadcast_component);
        
        gb::net::ces_client_component_shared_ptr client_component = std::make_shared<gb::net::ces_client_component>();
        client_menu_scene::add_component(client_component);
        
        m_servers_list = m_ui_fabricator->create_table_view(glm::vec2(200.f, 300.f));
        m_servers_list->set_on_get_cell_callback(std::bind(&client_menu_scene::create_table_view_cell, this, std::placeholders::_1,
                                                           std::placeholders::_2, std::placeholders::_3));
        m_servers_list->set_on_get_table_cell_height_callback(std::bind(&client_menu_scene::get_table_view_cell_height, this, std::placeholders::_1));
        m_servers_list->position = glm::vec2(0.f, 0.f);
        client_menu_scene::add_child(m_servers_list);
    }
    
    void client_menu_scene::on_endpoints_received(std::set<std::string> endpoints, gb::ces_entity_const_shared_ptr entity)
    {
        std::vector<gb::ui::table_view_cell_data_shared_ptr> data_source;
        for(const auto& endpoint : endpoints)
        {
            auto data = std::make_shared<endpoint_table_view_cell_data>(endpoint);
            data_source.push_back(data);
        }
        m_servers_list->set_data_source(data_source);
        m_servers_list->reload_data();
    }
    
    f32 client_menu_scene::get_table_view_cell_height(i32 index)
    {
        return 32.f;
    }
    
    gb::ui::table_view_cell_shared_ptr client_menu_scene::create_table_view_cell(i32 index, const gb::ui::table_view_cell_data_shared_ptr& data,
                                                                                 const gb::ces_entity_shared_ptr& table_view)
    {
        gb::ui::table_view_cell_shared_ptr cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("endpoint_cell", index);
        if(!cell)
        {
            cell = std::make_shared<gb::ui::table_view_cell>(client_menu_scene::get_fabricator(), index, "endpoint_cell");
            cell->create();
            
            const auto& endpoint_data = std::static_pointer_cast<endpoint_table_view_cell_data>(data);
            
            gb::ui::button_shared_ptr connect_button = m_ui_fabricator->create_button(glm::vec2(200.f, 32.f), nullptr);
            connect_button->position = glm::vec2(0.f, 0.f);
            connect_button->set_text(endpoint_data->get_endpoint());
            cell->add_child(connect_button);
        }
        cell->set_size(glm::vec2(200.f, 32.f));
        return cell;
    }
}
