//
//  client_menu_scene.hpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace ns
{
    class client_menu_scene : public gb::scene_graph
    {
    private:
        
        
    protected:
        
        gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
        gb::camera_shared_ptr m_camera;
        gb::ui::table_view_shared_ptr m_servers_list;
        
        void on_endpoints_received(std::set<std::string> endpoints, gb::ces_entity_const_shared_ptr entity);
        
        void connect(gb::ces_entity_const_shared_ptr entity);
        
        gb::ui::table_view_cell_shared_ptr create_table_view_cell(i32 index, const gb::ui::table_view_cell_data_shared_ptr& data,
                                                                  const gb::ces_entity_shared_ptr& table_view);
        f32 get_table_view_cell_height(i32 index);
        
    public:
        
        client_menu_scene(const gb::game_transition_shared_ptr& transition);
        ~client_menu_scene();
        
        void create();
    };
};

