//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "in_game_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "joystick.h"
#include "game_commands_container.h"
#include "ces_box2d_body_component.h"
#include "ns_ui_commands.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source.h"
#include "character_controller.h"
#include "ces_ani_animation_system.h"
#include "ces_character_controllers_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"

namespace ns
{
    in_game_scene::in_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    in_game_scene::~in_game_scene()
    {
        
    }
    
    void in_game_scene::create()
    {
        gb::scene_graph::create();
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(in_game_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(in_game_scene::get_transition()->get_screen_width(),
                                                in_game_scene::get_transition()->get_screen_height());
        in_game_scene::set_camera(m_camera);
        
        gb::ui::joystick_shared_ptr joystick = m_ui_fabricator->create_joystick(glm::vec2(128.f, 128.f));
        joystick->position = glm::vec2(32.f, in_game_scene::get_transition()->get_screen_height() - (128.f + 32.f));
        joystick->tag = "joystick";
        in_game_scene::add_child(joystick);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
        in_game_scene::get_transition()->add_system(animation_system);
        
        auto anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(in_game_scene::get_fabricator());
        
        auto level = anim_fabricator->create_animated_sprite("ns_level_01.xml", "level");
        in_game_scene::add_child(level);
        level->position = glm::vec2(0.f, 0.f);
        level->goto_and_stop(0);
        level->is_shadow_caster = true;
        
        gb::vbo::vertex_attribute vertices[4];
        vertices[0].m_position = glm::vec3(0.f,    0.f,    0.f);
        vertices[1].m_position = glm::vec3(2048.f, 0.f,    0.f);
        vertices[2].m_position = glm::vec3(2048.f, 2048.f, 0.f);
        vertices[3].m_position = glm::vec3(0.f,    2048.f, 0.f);
        
        gb::ces_convex_hull_component_shared_ptr convex_hull_component = std::make_shared<gb::ces_convex_hull_component>();
        convex_hull_component->create(vertices, 4);
        level->add_component(convex_hull_component);
        
        gb::ces_shadow_component_shared_ptr shadow_component = std::make_shared<gb::ces_shadow_component>();
        level->add_component(shadow_component);
        
        gb::light_source_shared_ptr light_source = in_game_scene::get_fabricator()->create_light_source("light_01.xml");
        light_source->radius = 512.f;
        light_source->color = glm::vec4(1.f, 1.f, 1.f, 1.f);
        light_source->tag = "light_source";
        
        gb::sprite_shared_ptr sprite_01 = in_game_scene::get_fabricator()->create_sprite("sprite_01.xml");
        sprite_01->size = glm::vec2(128.f, 128.f);
        sprite_01->position = glm::vec2(256.f, 256.f);
        sprite_01->pivot = glm::vec2(.5f, .5f);
        in_game_scene::add_child(sprite_01);
        sprite_01->is_shadow_caster = true;
        sprite_01->tag = "sprite_01";
        sprite_01->is_luminous = true;
        
        gb::game_object_shared_ptr character_container = std::make_shared<gb::game_object>();
        character_container->position = glm::vec2(64.f, 64.f);
        in_game_scene::add_child(character_container);
        
        auto feet = anim_fabricator->create_animated_sprite("ns_character_01.xml", "feet_animation");
        feet->tag = "feet";
        feet->goto_and_play("idle");
        feet->is_luminous = true;
        feet->rotation = -90.f;
        character_container->add_child(feet);
        
        auto body = anim_fabricator->create_animated_sprite("ns_character_01.xml", "character_animation_shotgun");
        body->tag = "body";
        body->goto_and_play("idle");
        body->is_luminous = true;
        body->rotation = -90.f;
        character_container->add_child(body);
        character_container->add_child(light_source);
        
        in_game_scene::get_transition()->add_system(std::make_shared<ces_character_controllers_system>());
        m_character_controller = std::make_shared<character_controller>(m_camera, character_container);
        m_character_controller->set_joystick(joystick);
        in_game_scene::add_child(m_character_controller);
        
        in_game_scene::enable_box2d_world(glm::vec2(0.f, 0.f), glm::vec2(2048.f, 2048.f));
        in_game_scene::apply_box2d_physics(character_container, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(10.f);
        });
        in_game_scene::apply_box2d_physics(sprite_01, nullptr);
    }
}
