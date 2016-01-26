//
//  demo_game_scene.h
//  gbDemo
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef demo_game_scene_h
#define demo_game_scene_h

#include "scene_graph.h"
#include "ui_declarations.h"
#include "ed_declarations.h"
#include "cs_declarations.h"
#include "content_list.h"

class sprite_list_data : public gb::ui::content_list_data
{
private:
    
protected:
    
    std::string m_filename;
    
public:
    
    sprite_list_data(const std::string& filename);
    ~sprite_list_data();
    
    std::string get_filename() const;
};

class demo_game_scene : public gb::scene_graph
{
private:
    

protected:
    
    gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
    gb::ed::ed_fabricator_shared_ptr m_ed_fabricator;
    
    cs::camera_controller_shared_ptr m_camera_controller;
    cs::game_objects_drag_controller_shared_ptr m_game_objects_drag_controller;
    
    void add_light_stroke(const gb::light_shared_ptr& light);
    
    gb::ui::content_list_cell_shared_ptr create_sprite_list_cell(i32 index, const gb::ui::content_list_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view);
    f32 get_table_view_cell_height(i32 index);
    gb::ui::content_tab_list_cell_shared_ptr create_tab_list_cell(i32 index, const gb::ui::content_tab_list_data_shared_ptr& data);
    
public:
    
    demo_game_scene(const gb::game_transition_shared_ptr& transition);
    ~demo_game_scene();
    
    void create();
};

#endif
