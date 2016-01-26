//
//  demo_game_scene.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 8/27/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "demo_game_scene.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "label.h"
#include "light.h"
#include "camera.h"
#include "stroke.h"
#include "mesh_constructor.h"
#include "ces_material_component.h"
#include "ces_bound_touch_component.h"
#include "ui_fabricator.h"
#include "button.h"
#include "ed_fabricator.h"
#include "grid.h"
#include "camera_controller.h"
#include "game_objects_drag_controller.h"
#include "table_view.h"
#include "table_view_cell.h"
#include "content_tab_list.h"
#include "content_tab_list_cell.h"

demo_game_scene::demo_game_scene(const gb::game_transition_shared_ptr& transition) :
gb::scene_graph(transition)
{

}

demo_game_scene::~demo_game_scene()
{
    
}

void demo_game_scene::create()
{
    gb::scene_graph::create();
    
    m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(demo_game_scene::get_fabricator());
    m_ed_fabricator = std::make_shared<gb::ed::ed_fabricator>(demo_game_scene::get_fabricator());
    
    gb::camera_shared_ptr camera = std::make_shared<gb::camera>(demo_game_scene::get_transition()->get_screen_width(),
                                                                demo_game_scene::get_transition()->get_screen_height());
    demo_game_scene::set_camera(camera);
    
    gb::ed::grid_shared_ptr grid = m_ed_fabricator->create_grid("grid.xml", 256, 256, 32, 32);
    grid->set_color(glm::vec4(0.f, 1.f, 0.f, 1.f));
    demo_game_scene::add_child(grid);
    
    m_camera_controller = std::make_shared<cs::camera_controller>(camera);
    m_camera_controller->set_map(grid);
    
    gb::sprite_shared_ptr sprite_01 = demo_game_scene::get_fabricator()->create_sprite("sprite_01.xml");
    sprite_01->set_size(glm::vec2(128.f, 128.f));
    sprite_01->set_position(glm::vec2(350.f, 200.f));
    demo_game_scene::add_child(sprite_01);
    sprite_01->set_cast_shadow(true);
    
    gb::sprite_shared_ptr sprite_02 = demo_game_scene::get_fabricator()->create_sprite("sprite_02.xml");
    sprite_02->set_size(glm::vec2(64.f, 64.f));
    sprite_02->set_position(glm::vec2(50.f, 200.f));
    sprite_01->add_child(sprite_02);
    sprite_02->set_cast_shadow(true);
    
    gb::label_shared_ptr label_01 = demo_game_scene::get_fabricator()->create_label("label_01.xml");
    label_01->set_text("game box");
    demo_game_scene::add_child(label_01);
    
    gb::light_shared_ptr light_01 = demo_game_scene::get_fabricator()->create_light("light_01.xml");
    light_01->set_position(glm::vec2(250.f, 250.f));
    light_01->set_radius(512.f);
    light_01->set_color(glm::vec4(1.f, 1.f, 1.f, 1.f));
    demo_game_scene::add_child(light_01);
    demo_game_scene::add_light_stroke(light_01);
    
    gb::light_shared_ptr light_02 = demo_game_scene::get_fabricator()->create_light("light_01.xml");
    light_02->set_position(glm::vec2(250.f, 650.f));
    light_02->set_radius(512.f);
    light_02->set_color(glm::vec4(1.f, 1.f, 1.f, 1.f));
    demo_game_scene::add_child(light_02);
    demo_game_scene::add_light_stroke(light_02);
    
    gb::light_shared_ptr light_03 = demo_game_scene::get_fabricator()->create_light("light_01.xml");
    light_03->set_position(glm::vec2(650.f, 250.f));
    light_03->set_radius(512.f);
    light_03->set_color(glm::vec4(1.f, 1.f, 1.f, 1.f));
    demo_game_scene::add_child(light_03);
    demo_game_scene::add_light_stroke(light_03);
    
    gb::ui::content_tab_list_shared_ptr content_tab_list = m_ui_fabricator->create_content_tab_list(glm::vec2(300.f, 32.f));
    content_tab_list->set_on_create_cell_callback(std::bind(&demo_game_scene::create_tab_list_cell, this, std::placeholders::_1, std::placeholders::_2));
    content_tab_list->set_position(glm::vec2(demo_game_scene::get_transition()->get_screen_width() - 335.f, 10.f));
    demo_game_scene::add_child(content_tab_list);
    
    std::vector<std::shared_ptr<gb::ui::content_tab_list_data>> data_source_02;
    for(i32 i = 0; i < 4; ++i)
    {
        data_source_02.push_back(std::make_shared<gb::ui::content_tab_list_data>("tab"));
    }
    content_tab_list->set_data_source(data_source_02);
    
    gb::ed::stroke_shared_ptr stroke = m_ed_fabricator->create_stroke("stroke.xml");
    stroke->set_color(glm::vec4(0.f, 1.f, 0.f, 1.f));
    stroke->set_size(glm::vec2(64.f, 64.f));
    stroke->set_is_animated(true);
    
    m_game_objects_drag_controller = std::make_shared<cs::game_objects_drag_controller>(stroke);
    
    m_game_objects_drag_controller->add_game_object(sprite_01);
    m_game_objects_drag_controller->add_game_object(sprite_02);
    
    m_game_objects_drag_controller->add_game_object(light_01);
    m_game_objects_drag_controller->add_game_object(light_02);
    m_game_objects_drag_controller->add_game_object(light_03);
}

void demo_game_scene::add_light_stroke(const gb::light_shared_ptr& light)
{
    gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<gb::ces_bound_touch_component>();
    glm::vec4 bound = glm::vec4(-16.f, -16.f, 16.f, 16.f);
    bound_touch_compoent->set_frame(bound);
    light->add_component(bound_touch_compoent);
    
    gb::ed::stroke_shared_ptr light_stroke = m_ed_fabricator->create_stroke("stroke.xml");
    light_stroke->set_color(glm::vec4(light->get_color()));
    light_stroke->set_size(glm::vec2(32.f, 32.f));
    
    glm::vec2 center = glm::vec2((bound.x + bound.z) * .5f,
                                 (bound.y + bound.w) * .5f);
    light_stroke->set_position(center);
    
    light->add_child(light_stroke);
}

gb::ui::table_view_cell_shared_ptr demo_game_scene::create_table_view_cell(i32 index, const gb::ui::table_view_cell_data_shared_ptr& data,
                                                                              const gb::ces_entity_shared_ptr& table_view)
{
    gb::ui::table_view_cell_shared_ptr cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("sprite_cell", index);
    if(!cell)
    {
        cell = std::make_shared<gb::ui::table_view_cell>(demo_game_scene::get_fabricator(), index, "sprite_cell");
        cell->create();
    }
    cell->set_size(glm::vec2(280.f, 64.f));
    return cell;
}

gb::ui::content_tab_list_cell_shared_ptr demo_game_scene::create_tab_list_cell(i32 index, const gb::ui::content_tab_list_data_shared_ptr& data)
{
    gb::ui::content_tab_list_cell_shared_ptr cell = std::make_shared<gb::ui::content_tab_list_cell>(demo_game_scene::get_fabricator());
    cell->create();
    cell->set_size(glm::vec2(300.f, 700.f));
    if(index == 0)
    {
        gb::ui::table_view_shared_ptr table_view = m_ui_fabricator->create_table_view(glm::vec2(280.f, 700.f));
        table_view->set_on_get_cell_callback(std::bind(&demo_game_scene::create_table_view_cell, this, std::placeholders::_1,
                                                       std::placeholders::_2, std::placeholders::_3));
        table_view->set_on_get_table_cell_height_callback(std::bind(&demo_game_scene::get_table_view_cell_height, this, std::placeholders::_1));
        table_view->set_position(glm::vec2(0.f, 0.f));
        cell->add_child(table_view);
        
        std::vector<gb::ui::table_view_cell_data_shared_ptr> data_source_01;
        for(i32 i = 0; i < 64; ++i)
        {
            data_source_01.push_back(std::make_shared<gb::ui::table_view_cell_data>());
        }
        table_view->set_data_source(data_source_01);
        table_view->reload_data();
    }
    return cell;
}

f32 demo_game_scene::get_table_view_cell_height(i32 index)
{
    return index % 2 == 0 ? 128.f : 64.f;
}

