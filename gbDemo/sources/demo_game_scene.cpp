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
#include "text_label.h"
#include "light_source.h"
#include "camera.h"
#include "stroke.h"
#include "landscape.h"
#include "brush.h"
#include "mesh_constructor.h"
#include "ces_material_component.h"
#include "ces_bound_touch_component.h"
#include "ui_fabricator.h"
#include "button.h"
#include "grouped_buttons.h"
#include "switcher.h"
#include "ed_fabricator.h"
#include "grid.h"
#include "drag_camera_controller.h"
#include "drag_game_objects_controller.h"
#include "drag_brush_controller.h"
#include "table_view.h"
#include "table_view_cell.h"
#include "content_tab_list.h"
#include "content_tab_list_cell.h"
#include "ces_render_system.h"
#include "render_pipeline.h"

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
    
    m_camera = std::make_shared<gb::camera>(demo_game_scene::get_transition()->get_screen_width(),
                                            demo_game_scene::get_transition()->get_screen_height());
    demo_game_scene::set_camera(m_camera);
    
    std::vector<std::string> brushes_filenames;
    brushes_filenames.push_back("img_01.png");
    brushes_filenames.push_back("img_02.png");
    brushes_filenames.push_back("img_03.png");
    
    m_landscape = m_ed_fabricator->create_landscape("landscape.xml", glm::vec2(4096.f),
                                                    std::vector<std::string>(),
                                                    brushes_filenames);
    demo_game_scene::add_child(m_landscape);
    
    m_grid = m_ed_fabricator->create_grid("grid.xml", 128, 128, 32, 32);
    m_grid->set_color(glm::vec4(0.f, 1.f, 0.f, 1.f));
    demo_game_scene::add_child(m_grid);
    
    gb::sprite_shared_ptr sprite_01 = demo_game_scene::get_fabricator()->create_sprite("sprite_01.xml");
    sprite_01->size = glm::vec2(400.f, 400.f);
    sprite_01->position = glm::vec2(350.f, 200.f);
    demo_game_scene::add_child(sprite_01);
    sprite_01->cast_shadow = true;
    
    gb::sprite_shared_ptr sprite_02 = demo_game_scene::get_fabricator()->create_sprite("sprite_02.xml");
    sprite_02->size = glm::vec2(64.f, 64.f);
    sprite_02->position = glm::vec2(50.f, 200.f);
    sprite_01->add_child(sprite_02);
    sprite_02->cast_shadow = true;
    
    gb::sprite_shared_ptr wall_01 = demo_game_scene::get_fabricator()->create_sprite("wall_01.xml");
    wall_01->size = glm::vec2(200.f, 200.f);
    wall_01->position = glm::vec2(10.f, 10.f);
    demo_game_scene::add_child(wall_01);
    wall_01->cast_shadow = true;
    
    gb::text_label_shared_ptr label_01 = demo_game_scene::get_fabricator()->create_text_label("label_01.xml");
    label_01->text = "game box";
    demo_game_scene::add_child(label_01);
    
    gb::light_source_shared_ptr light_01 = demo_game_scene::get_fabricator()->create_light_source("light_01.xml");
    light_01->position = glm::vec2(250.f, 250.f);
    light_01->radius = 512.f;
    light_01->color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    demo_game_scene::add_child(light_01);
    demo_game_scene::add_light_stroke(light_01);
    
    gb::light_source_shared_ptr light_02 = demo_game_scene::get_fabricator()->create_light_source("light_01.xml");
    light_02->position = glm::vec2(250.f, 650.f);
    light_02->radius = 512.f;
    light_02->color = glm::vec4(1.f, 1.f, 1.f, 1.f);
    demo_game_scene::add_child(light_02);
    demo_game_scene::add_light_stroke(light_02);
    
    gb::ui::content_tab_list_shared_ptr content_tab_list = m_ui_fabricator->create_content_tab_list(glm::vec2(300.f, 32.f));
    content_tab_list->set_on_create_cell_callback(std::bind(&demo_game_scene::create_tab_list_cell, this, std::placeholders::_1, std::placeholders::_2));
    content_tab_list->position = glm::vec2(demo_game_scene::get_transition()->get_screen_width() - 335.f, 10.f);
    demo_game_scene::add_child(content_tab_list);
    
    std::vector<std::shared_ptr<gb::ui::content_tab_list_data>> data_source_02;
    for(i32 i = 0; i < 4; ++i)
    {
        data_source_02.push_back(std::make_shared<gb::ui::content_tab_list_data>("tab"));
    }
    content_tab_list->set_data_source(data_source_02);
    
    m_stroke = m_ed_fabricator->create_stroke("stroke.xml");
    m_stroke->set_color(glm::vec4(0.f, 1.f, 0.f, 1.f));
    m_stroke->set_size(glm::vec2(64.f, 64.f));
    m_stroke->set_is_animated(true);
    
    m_brush = m_ed_fabricator->create_brush("brush.xml");
    m_brush->set_radius(32.f);
    m_brush->set_position(glm::vec2(0.f));
    
    gb::ui::grouped_buttons_shared_ptr grouped_buttons = m_ui_fabricator->create_grouped_buttons(glm::vec2(196.f, 32.f),
                                                                                                 std::bind(&demo_game_scene::on_controller_changed, this, std::placeholders::_1, std::placeholders::_2));
    grouped_buttons->position = glm::vec2(10.f, 25.f);
    std::vector<std::string> labels;
    labels.push_back("camera");
    labels.push_back("objects");
    labels.push_back("brushes");
    grouped_buttons->set_data_source(labels);
    demo_game_scene::add_child(grouped_buttons);
    
    gb::ui::switcher_shared_ptr switcher = m_ui_fabricator->create_switcher(glm::vec2(64.f, 32.f));
    switcher->position = glm::vec2(10.f, 64.f);
    switcher->set_on_switch_callback(std::bind(&demo_game_scene::on_lighting_switch, this,
                                               std::placeholders::_1, std::placeholders::_2));
    demo_game_scene::add_child(switcher);
    
    m_game_objects.push_back(sprite_01);
    m_game_objects.push_back(sprite_02);
    m_game_objects.push_back(light_01);
    m_game_objects.push_back(light_02);
    
    std::shared_ptr<gb::ces_render_system> render_system = std::static_pointer_cast<gb::ces_render_system>(demo_game_scene::get_transition()->get_system(gb::e_ces_system_type_render));
    gb::material_shared_ptr deffered_lighting_material = render_system->get_render_pipeline()->get_technique_material("ss.deferred.lighting");
    deffered_lighting_material->set_custom_shader_uniform(0, "u_lighting");
}

void demo_game_scene::add_light_stroke(const gb::light_source_shared_ptr& light)
{
    gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<gb::ces_bound_touch_component>();
    glm::vec4 bound = glm::vec4(-16.f, -16.f, 16.f, 16.f);
    bound_touch_compoent->set_frame(bound);
    light->add_component(bound_touch_compoent);
    
    gb::ed::stroke_shared_ptr light_stroke = m_ed_fabricator->create_stroke("stroke.xml");
    //light_stroke->set_color(glm::vec4(light->color));
    light_stroke->set_size(glm::vec2(32.f, 32.f));
    
    glm::vec2 center = glm::vec2((bound.x + bound.z) * .5f,
                                 (bound.y + bound.w) * .5f);
    light_stroke->position = center;
    
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
    cell->set_size(glm::vec2(280.f, 128.f));
    return cell;
}

gb::ui::content_tab_list_cell_shared_ptr demo_game_scene::create_tab_list_cell(i32 index,
                                                                               const gb::ui::content_tab_list_data_shared_ptr& data)
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
        table_view->position = glm::vec2(0.f, 0.f);
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
    return 128.f;
}

void demo_game_scene::on_controller_changed(i32 index, const gb::ces_entity_shared_ptr& entity)
{
    if(index == 0)
    {
        gb::ed::drag_camera_controller_shared_ptr drag_camera_controller = std::make_shared<gb::ed::drag_camera_controller>(m_camera);
        m_drag_controller = drag_camera_controller;
        
        drag_camera_controller->set_grid(m_grid);
    }
    else if(index == 1)
    {
        gb::ed::drag_game_objects_controller_shared_ptr drag_game_objects_controller = std::make_shared<gb::ed::drag_game_objects_controller>(m_stroke);
        m_drag_controller = drag_game_objects_controller;
        
        for (const auto& game_object : m_game_objects)
        {
            drag_game_objects_controller->add_game_object(game_object);
        }
    }
    else if(index == 2)
    {
        gb::ed::drag_brush_controller_shared_ptr drag_brush_controller = std::make_shared<gb::ed::drag_brush_controller>(m_landscape, m_brush);
        m_drag_controller = drag_brush_controller;
        
        drag_brush_controller->set_grid(m_grid);
    }
}

void demo_game_scene::on_lighting_switch(bool value, const gb::ces_entity_shared_ptr& entity)
{
    std::shared_ptr<gb::ces_render_system> render_system = std::static_pointer_cast<gb::ces_render_system>(demo_game_scene::get_transition()->get_system(gb::e_ces_system_type_render));
    gb::material_shared_ptr deffered_lighting_material = render_system->get_render_pipeline()->get_technique_material("ss.deferred.lighting");
    deffered_lighting_material->set_custom_shader_uniform(value ? 1 : 0, "u_lighting");
}
