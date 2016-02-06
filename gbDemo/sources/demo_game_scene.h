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

class demo_game_scene : public gb::scene_graph
{
private:
    

protected:
    
    gb::ui::ui_fabricator_shared_ptr m_ui_fabricator;
    gb::ed::ed_fabricator_shared_ptr m_ed_fabricator;
    
    gb::ed::drag_controller_shared_ptr m_drag_controller;
    
    gb::ed::grid_shared_ptr m_grid;
    gb::ed::landscape_shared_ptr m_landscape;
    gb::ed::brush_shared_ptr m_brush;
    gb::camera_shared_ptr m_camera;
    gb::ed::stroke_shared_ptr m_stroke;
    std::vector<gb::game_object_shared_ptr> m_game_objects;
    
    void add_light_stroke(const gb::light_shared_ptr& light);
    
    gb::ui::table_view_cell_shared_ptr create_table_view_cell(i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view);
    f32 get_table_view_cell_height(i32 index);
    gb::ui::content_tab_list_cell_shared_ptr create_tab_list_cell(i32 index, const gb::ui::content_tab_list_data_shared_ptr& data);
    
    void on_controller_changed(i32 index, const gb::ces_entity_shared_ptr& entity);
    void on_lighting_switch(bool value, const gb::ces_entity_shared_ptr& entity);
    
public:
    
    demo_game_scene(const gb::game_transition_shared_ptr& transition);
    ~demo_game_scene();
    
    void create();
};

#endif
