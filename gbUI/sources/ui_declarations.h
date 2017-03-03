//
//  ui_declarations.h
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ui_declarations_h
#define ui_declarations_h

#include "declarations.h"

namespace gb
{
    namespace ui
    {
        class control;
        typedef std::shared_ptr<control> control_shared_ptr;
        
        class dialog;
        typedef std::shared_ptr<dialog> dialog_shared_ptr;
        
        class ui_fabricator;
        typedef std::shared_ptr<ui_fabricator> ui_fabricator_shared_ptr;
        
        class button;
        typedef std::shared_ptr<button> button_shared_ptr;
        
        class table_view;
        typedef std::shared_ptr<table_view> table_view_shared_ptr;
        
        class table_view_cell;
        typedef std::shared_ptr<table_view_cell> table_view_cell_shared_ptr;
        
        class table_view_cell_data;
        typedef std::shared_ptr<table_view_cell_data> table_view_cell_data_shared_ptr;
        
        class content_tab_list;
        typedef std::shared_ptr<content_tab_list> content_tab_list_shared_ptr;
        
        class content_tab_list_data;
        typedef std::shared_ptr<content_tab_list_data> content_tab_list_data_shared_ptr;
        
        class content_tab_list_cell;
        typedef std::shared_ptr<content_tab_list_cell> content_tab_list_cell_shared_ptr;
        
        class grouped_buttons;
        typedef std::shared_ptr<grouped_buttons> grouped_buttons_shared_ptr;
        
        class switcher;
        typedef std::shared_ptr<switcher> switcher_shared_ptr;
        
        class tree_view;
        typedef std::shared_ptr<tree_view> tree_view_shared_ptr;
        
        class tree_view_cell;
        typedef std::shared_ptr<tree_view_cell> tree_view_cell_shared_ptr;
        
        class tree_view_cell_data;
        typedef std::shared_ptr<tree_view_cell_data> tree_view_cell_data_shared_ptr;
        
        class tree_view_cell_scene_graph_data;
        typedef std::shared_ptr<tree_view_cell_scene_graph_data> tree_view_cell_scene_graph_data_shared_ptr;
        
        forward_decl(console)
        forward_decl(joystick)
        forward_decl(fullscreen_joystick)
    };
};

#endif
