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
#include "mesh_constructor.h"
#include "ces_material_component.h"
#include "ui_fabricator.h"
#include "button.h"
#include "ed_fabricator.h"
#include "grid.h"
#include "camera_controller.h"

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
    light_01->set_color(glm::vec4(0.f, 0.f, 1.f, 1.f));
    demo_game_scene::add_child(light_01);
    
    gb::light_shared_ptr light_02 = demo_game_scene::get_fabricator()->create_light("light_01.xml");
    light_02->set_position(glm::vec2(250.f, 450.f));
    light_02->set_radius(512.f);
    light_02->set_color(glm::vec4(1.f, 0.f, 0.f, 1.f));
    demo_game_scene::add_child(light_02);
    
    gb::light_shared_ptr light_03 = demo_game_scene::get_fabricator()->create_light("light_01.xml");
    light_03->set_position(glm::vec2(650.f, 50.f));
    light_03->set_radius(512.f);
    light_03->set_color(glm::vec4(1.f, 1.f, 0.f, 1.f));
    demo_game_scene::add_child(light_03);
    
    gb::ui::button_shared_ptr button = m_ui_fabricator->create_button(glm::vec2(128.f, 32.f), nullptr);
    button->set_position(glm::vec2(50.f, 450.f));
    button->set_text("button");
    demo_game_scene::add_child(button);
    
    /*gb::sprite_shared_ptr sprite_03 = demo_game_scene::get_fabricator()->create_sprite("button_background.xml");
    sprite_03->set_size(glm::vec2(64.f, 64.f));
    sprite_03->set_position(glm::vec2(50.f, 450.f));
    demo_game_scene::add_child(sprite_03);*/
    
}

