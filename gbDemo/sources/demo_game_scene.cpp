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
#include "mesh_constructor.h"
#include "ces_material_component.h"

demo_game_scene::demo_game_scene(const gb::game_transition_shared_ptr& transition) :
gb::scene_graph(transition)
{

}

demo_game_scene::~demo_game_scene()
{
    
}

void demo_game_scene::create()
{
    gb::sprite_shared_ptr sprite_01 = demo_game_scene::get_fabricator()->create_sprite("sprite_01.xml");
    sprite_01->set_size(glm::vec2(128.f, 128.f));
    sprite_01->set_position(glm::vec2(10.f, 10.f));
    demo_game_scene::add_child(sprite_01);
    
    gb::sprite_shared_ptr sprite_02 = demo_game_scene::get_fabricator()->create_sprite("sprite_02.xml");
    sprite_02->set_size(glm::vec2(64.f, 64.f));
    sprite_02->set_position(glm::vec2(150.f, 150.f));
    sprite_01->add_child(sprite_02);
    
    gb::label_shared_ptr label_01 = demo_game_scene::get_fabricator()->create_label("label_01.xml");
    label_01->set_text("game box");
    demo_game_scene::add_child(label_01);
    
    gb::light_shared_ptr light_01 = demo_game_scene::get_fabricator()->create_light("light_01.xml");
    light_01->set_position(glm::vec2(200.f, 200.f));
    gb::ces_material_component_shared_ptr material_component = std::static_pointer_cast<gb::ces_material_component>(light_01->get_component(gb::e_ces_component_type_material));
    material_component->set_custom_shader_uniform(64.f, "u_radius");
    demo_game_scene::add_child(light_01);
}

