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
#include "scene_graph.h"
#include "camera.h"
#include "shadow_cast_light.h"
#include "omni_light.h"
#include "direction_light.h"
#include "particle_emitter.h"
#include "model3d_animated.h"
#include "instanced_models3d_static.h"
#include "skybox.h"
#include "ocean.h"
#include "ui_fabricator.h"
#include "ui_graph.h"
#include "level.h"
#include "character_controller.h"
#include "ai_character_controller.h"
#include "koth_game_commands.h"
#include "game_command.h"
#include "game_commands_container.h"
#include "ces_render_component.h"
#include "ces_fly_camera_navigation_component.h"

demo_game_scene::demo_game_scene(const gb::game_transition_shared_ptr& transition) :
gb::game_scene(transition)
{
    m_camera = scene_fabricator_inst->create_camera(45.f, .5f, 1024.f, glm::ivec4(0.f, 0.f,
                                                                                  game_scene::get_transition()->get_width(),
                                                                                  game_scene::get_transition()->get_height()));
    scene_graph_inst->set_main_camera(m_camera);
    
    m_grid = scene_fabricator_inst->create_grid(32, 32);
    m_grid->set_position(glm::vec3(0.f, 1.f, 0.f));
    m_grid->set_touches_receives_enabled(true);
    scene_graph_inst->add_game_object(m_grid);
    
    m_3d_tile_cursor = scene_fabricator_inst->create_3d_tile_cursor();
    m_3d_tile_cursor->set_position(glm::vec3(0.f, 1.f, 0.f));
    scene_graph_inst->add_game_object(m_3d_tile_cursor);
    
    koth::ces_fly_camera_navigation_component_shared_ptr fly_camera_navigation_component = std::make_shared<koth::ces_fly_camera_navigation_component>();
    m_camera->add_component(fly_camera_navigation_component);
    fly_camera_navigation_component->set_position(glm::vec3(-8.f, 8.f, 8.f));
    m_camera->set_position(glm::vec3(-8.f, 8.f, 8.f));
    
    scene_graph_inst->set_shadow_cast_light(90.f, .1f, 128.f);
    m_shadow_cast_light = scene_graph_inst->get_shadow_cast_light();
    
    m_models["human_01"] = scene_fabricator_inst->create_model3d_animated("gameobject.human_01.xml");
    m_models["human_02"] = scene_fabricator_inst->create_model3d_animated("gameobject.human_02.xml");
    m_models["orc_01"] = scene_fabricator_inst->create_model3d_animated("gameobject.orc_01.xml");
    m_models["orc_02"] = scene_fabricator_inst->create_model3d_animated("gameobject.orc_02.xml");
    
    gb::particle_emitter_shared_ptr particle_emitter_fire = scene_fabricator_inst->create_particle_emitter("gameobject.particle.emitter.fire.xml");
    gb::particle_emitter_shared_ptr particle_emitter_smoke = scene_fabricator_inst->create_particle_emitter("gameobject.particle.emitter.smoke.xml");
    
    gb::model3d_static_shared_ptr plane = scene_fabricator_inst->create_model3d_static("gameobject.plane.xml");
    plane->set_scale(glm::vec3(32.f, 1.f, 32.f));
    plane->set_position(glm::vec3(-8.f, 0.f, -8.f));
    
    scene_graph_inst->add_game_object(m_models["human_01"]);
    scene_graph_inst->add_game_object(m_models["human_02"]);
    scene_graph_inst->add_game_object(m_models["orc_01"]);
    scene_graph_inst->add_game_object(m_models["orc_02"]);
    
    /*scene_graph_inst->add_game_object(particle_emitter_fire);
    scene_graph_inst->add_game_object(particle_emitter_smoke);*/
    
    //scene_graph_inst->add_game_object(plane);
    
    //m_camera->set_position(glm::vec3(0.f));
    //m_camera->set_look_at(glm::vec3(2.f, 1.f, 2.f));
    
    
    particle_emitter_fire->set_position(glm::vec3(8.f, .5f, 10.f));
    particle_emitter_smoke->set_position(glm::vec3(8.f, .5f, 6.f));
    
    m_shadow_cast_light->set_position(glm::vec3(8.25f, 16.f, 8.25f));
    m_shadow_cast_light->set_look_at(glm::vec3(7.75f, 1.f, 7.75f));
    
    //m_models["human_02"]->set_rotation(glm::vec3(0.f, -90.f, 0.f));
    //m_models["orc_01"]->set_rotation(glm::vec3(0.f, -90.f, 0.f));
    //m_models["orc_02"]->set_rotation(glm::vec3(0.f, -90.f, 0.f));
    
    //m_models["human_02"]->set_position(glm::vec3(1.f));
    //m_models["orc_01"]->set_position(glm::vec3(4.f, .5f, 8.f));
    //m_models["orc_02"]->set_position(glm::vec3(2.f, .5f, 4.f));
    
    m_models["human_01"]->set_scale(glm::vec3(.5f));
    m_models["human_02"]->set_scale(glm::vec3(.5f));
    m_models["orc_01"]->set_scale(glm::vec3(.5f));
    m_models["orc_02"]->set_scale(glm::vec3(.5f));
    
    //m_models["human_02"]->set_touches_receives_enabled(true);
    //m_models["human_02"]->set_debug_draw_enabled(true);
    
    //m_models["orc_01"]->set_debug_draw_enabled(true);
    //m_models["orc_02"]->set_debug_draw_enabled(true);
    
    m_omni_lights["omni_light_01"] = scene_fabricator_inst->create_omni_light(4.f, glm::vec4(1.f, 0.f, 0.f, 1.f));
    m_omni_lights["omni_light_02"] = scene_fabricator_inst->create_omni_light(4.f, glm::vec4(0.f, 1.f, 0.f, 1.f));
    m_omni_lights["omni_light_03"] = scene_fabricator_inst->create_omni_light(4.f, glm::vec4(0.f, 0.f, 1.f, 1.f));
    m_omni_lights["omni_light_04"] = scene_fabricator_inst->create_omni_light(4.f, glm::vec4(1.f, 0.f, 1.f, 1.f));
    
    scene_graph_inst->add_omni_light(m_omni_lights["omni_light_01"]);
    scene_graph_inst->add_omni_light(m_omni_lights["omni_light_02"]);
    scene_graph_inst->add_omni_light(m_omni_lights["omni_light_03"]);
    scene_graph_inst->add_omni_light(m_omni_lights["omni_light_04"]);
    
    /*m_omni_lights["omni_light_01"] = game_scene::get_transition()->get_fabricator()->create_omni_light();
    game_scene::get_transition()->get_scene_graph()->add_omni_light(m_omni_lights["omni_light_01"]);
    m_omni_lights["omni_light_01"]->set_radius(4.f);
    m_omni_lights["omni_light_01"]->set_position(glm::vec3(16.f, 1.f, 16.f));
    
    m_omni_lights["omni_light_02"] = game_scene::get_transition()->get_fabricator()->create_omni_light();
    game_scene::get_transition()->get_scene_graph()->add_omni_light(m_omni_lights["omni_light_02"]);
    m_omni_lights["omni_light_02"]->set_radius(8.f);
    m_omni_lights["omni_light_02"]->set_position(glm::vec3(16.f, 3.f, 16.f));*/
    
    m_direction_light = scene_fabricator_inst->create_direction_light();
    //scene_graph_inst->add_direction_light(m_direction_light);
    //m_direction_light->set_intensity(1.f);
    
    //m_instanced_omni_lights = scene_fabricator_inst->create_instanced_omni_lights(8);
    //scene_graph_inst->add_instanced_omni_lights(m_instanced_omni_lights);
   
    //m_instanced_omni_lights->set_radius(7.f, 0);
    //m_instanced_omni_lights->set_color(glm::vec4(0.f, 1.f, 0.f, 1.f), 0);
    //m_instanced_omni_lights->set_radius(3.f, 1);
    
    //m_instanced_omni_lights->set_radius(7.f, 2);
    //m_instanced_omni_lights->set_color(glm::vec4(0.f, 1.f, 1.f, 1.f), 2);
    //m_instanced_omni_lights->set_radius(3.f, 3);
    
    //m_instanced_omni_lights->set_radius(7.f, 4);
    //m_instanced_omni_lights->set_color(glm::vec4(1.f, 0.f, 0.f, 1.f), 4);
    //m_instanced_omni_lights->set_radius(3.f, 5);
    
    //m_instanced_omni_lights->set_radius(7.f, 6);
    //m_instanced_omni_lights->set_color(glm::vec4(1.f, 1.f, 0.f, 1.f), 6);
    //m_instanced_omni_lights->set_radius(3.f, 7);
    
    //m_instanced_omni_lights->set_position(glm::vec3(192.f * .5f, 192.f * .33f, 192.f * .5f), 8);
    //m_instanced_omni_lights->set_radius(192.f, 8);
    
    m_skybox = scene_fabricator_inst->create_skybox("gameobject.skybox.xml");
    scene_graph_inst->add_game_object(m_skybox);
    
    m_ocean = scene_fabricator_inst->create_ocean("gameobject.ocean.xml");
    scene_graph_inst->add_game_object(m_ocean);

    m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>();
    game_scene::get_transition()->add_fabricator(m_ui_fabricator, ui_fabricator_id);
    
    m_ui_graph = std::make_shared<gb::ui::ui_graph>();
    game_scene::get_transition()->add_graph(m_ui_graph, ui_graph_id);
    
    m_level = std::make_shared<koth::level>(scene_fabricator_inst, scene_graph_inst);
    m_level->construct("");
    
    m_models["human_01"]->set_enable_box2d_physics(true, false);
    m_models["human_02"]->set_enable_box2d_physics(true, false);
    m_models["orc_01"]->set_enable_box2d_physics(true, false);
    m_models["orc_02"]->set_enable_box2d_physics(true, false);
    
    m_character_controller = std::make_shared<koth::character_controller>(m_models["human_01"],
                                                                          nullptr);
    m_character_controller->set_position(glm::vec3(66.f, 0.f, 66.f));
    
    m_ai_character_controllers["human_02"] = std::make_shared<koth::ai_character_controller>(m_models["human_02"], m_level);
    m_ai_character_controllers["human_02"]->set_position(glm::vec3(66.f, 0.f, 126.f));
    
    m_ai_character_controllers["orc_01"] = std::make_shared<koth::ai_character_controller>(m_models["orc_01"], m_level);
    m_ai_character_controllers["orc_01"]->set_position(glm::vec3(126.f, 0.f, 126.f));
    
    m_ai_character_controllers["orc_02"] = std::make_shared<koth::ai_character_controller>(m_models["orc_02"], m_level);
    m_ai_character_controllers["orc_02"]->set_position(glm::vec3(126.f, 0.f, 66.f));
    
    gb::game_command_i_shared_ptr command = std::make_shared<gb::game_command<koth::on_move_state_changed::t_command>>(std::bind(&demo_game_scene::on_move_state_changed,
                                                                                                                                 this,
                                                                                                                                 std::placeholders::_1));
    m_internal_commands->add_command(koth::on_move_state_changed::guid, command);
    
    command = std::make_shared<gb::game_command<koth::on_rotate_state_changed::t_command>>(std::bind(&demo_game_scene::on_rotate_state_changed,
                                                                                                     this,
                                                                                                     std::placeholders::_1));
    m_internal_commands->add_command(koth::on_rotate_state_changed::guid, command);
    
    command = std::make_shared<gb::game_command<koth::on_mouse_dragged::t_command>>(std::bind(&demo_game_scene::on_mouse_dragged,
                                                                                              this,
                                                                                              std::placeholders::_1));
    m_internal_commands->add_command(koth::on_mouse_dragged::guid, command);
}

demo_game_scene::~demo_game_scene()
{
    
}

void demo_game_scene::update(f32 deltatime)
{
    //m_models["human_02"]->set_animation("RUN");
    //m_models["orc_01"]->set_animation("IDLE");
    //m_models["orc_02"]->set_animation("IDLE");
    
    /*static f32 angle = 0.f;
    angle += 0.05f;
    glm::vec2 light_xz_position = glm::vec2(0.f);
    light_xz_position.x = 8.f + cosf(angle) * -8.f;
    light_xz_position.y = 8.f + sinf(angle) * -8.f;
    m_direction_light->set_direction(glm::normalize(m_camera->get_position() - glm::vec3(8.f, 0.f, 8.f)));*/
    m_skybox->set_rotation(glm::vec3(0.f, 90.f, 0.f));
    
    //m_terrain->set_position(glm::vec3(-64.f, 0.f, -64.f));
    //m_models["orc_01"]->set_position(glm::vec3(light_xz_position.x, 1.f, light_xz_position.y));
    
    //m_instanced_omni_lights->set_position(glm::vec3(light_xz_position.x, 3.f, light_xz_position.y), 0);
    //m_instanced_omni_lights->set_position(glm::vec3(4.f, 3.f, light_xz_position.y), 1);
    /*m_omni_lights["omni_light_01"]->set_position(glm::vec3(light_xz_position.x, 1.f, light_xz_position.y));
    m_omni_lights["omni_light_02"]->set_position(glm::vec3(4.f, 1.f, light_xz_position.y));*/
    //m_camera->set_rotation(angle * .1f);
    
    //m_level->set_box_state(static_cast<i32>(m_models["human_02"]->get_position().x),
    //                       static_cast<i32>(m_models["human_02"]->get_position().z));
    m_level->cleanup();
    m_level->set_box_under_strain(m_models["human_01"]->get_position().x,
                                  m_models["human_01"]->get_position().z);
    m_level->set_box_under_strain(m_models["human_02"]->get_position().x,
                                  m_models["human_02"]->get_position().z);
    m_level->set_box_under_strain(m_models["orc_01"]->get_position().x,
                                  m_models["orc_01"]->get_position().z);
    m_level->set_box_under_strain(m_models["orc_02"]->get_position().x,
                                  m_models["orc_02"]->get_position().z);
    m_level->update(deltatime);
    m_character_controller->update(deltatime);
    //m_camera_controller->update(deltatime);
    
    m_ai_character_controllers["human_02"]->update(deltatime);
    m_ai_character_controllers["orc_01"]->update(deltatime);
    m_ai_character_controllers["orc_02"]->update(deltatime);
    
    glm::vec3 position = m_models["human_01"]->get_position();
    //position.y = m_terrain->get_height(position);
    m_models["human_01"]->set_position(position);
    
    m_ai_character_controllers["human_02"]->set_goal_position(m_models["human_01"]->get_position());
    m_ai_character_controllers["orc_01"]->set_goal_position(m_models["human_01"]->get_position());
    m_ai_character_controllers["orc_02"]->set_goal_position(m_models["human_01"]->get_position());
    
    glm::vec3 light_position = m_models["human_01"]->get_position();
    m_omni_lights["omni_light_01"]->set_position(light_position + glm::vec3(0.f, 0.f, -4.f));
    m_omni_lights["omni_light_02"]->set_position(light_position + glm::vec3(4.f, 0.f, 0.f));
    m_omni_lights["omni_light_03"]->set_position(light_position + glm::vec3(-4.f, 0.f, 0.f));
    m_omni_lights["omni_light_04"]->set_position(light_position + glm::vec3(0.f, 0.f, 4.f));
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 0);
    
    light_position = m_models["human_01"]->get_position() + m_models["human_01"]->get_forward() * 2.5f;
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 1);
    
    light_position = m_models["human_02"]->get_position();
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 2);
    
    light_position = m_models["human_02"]->get_position() + m_models["human_02"]->get_forward() * 2.5f;
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 3);
    
    
    
    
    light_position = m_models["orc_01"]->get_position();
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 4);
    
    light_position = m_models["orc_01"]->get_position() + m_models["orc_01"]->get_forward() * 2.5f;
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 5);
    
    
    
    
    light_position = m_models["orc_02"]->get_position();
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 6);
    
    light_position = m_models["orc_02"]->get_position() + m_models["orc_02"]->get_forward() * 2.5f;
    //light_position.y = m_terrain->get_height(light_position) + 1.66f;
    //m_instanced_omni_lights->set_position(light_position, 7);

    
    
    
    gb::ces_render_component_shared_ptr render_component =
    std::static_pointer_cast<gb::ces_render_component>(m_models["human_01"]->get_component(gb::e_ces_component_type_render));
    render_component->set_custom_shader_uniform(.033f, "u_outline_width");
    render_component->set_custom_shader_uniform(glm::vec3(0.f, 1.f, 0.f), "u_outline_color");
    
    render_component =
    std::static_pointer_cast<gb::ces_render_component>(m_models["human_02"]->get_component(gb::e_ces_component_type_render));
    render_component->set_custom_shader_uniform(.033f, "u_outline_width");
    render_component->set_custom_shader_uniform(glm::vec3(0.f, 1.f, 1.f), "u_outline_color");
    
    render_component =
    std::static_pointer_cast<gb::ces_render_component>(m_models["orc_01"]->get_component(gb::e_ces_component_type_render));
    render_component->set_custom_shader_uniform(.033f, "u_outline_width");
    render_component->set_custom_shader_uniform(glm::vec3(1.f, 0.f, 0.f), "u_outline_color");
    
    render_component =
    std::static_pointer_cast<gb::ces_render_component>(m_models["orc_02"]->get_component(gb::e_ces_component_type_render));
    render_component->set_custom_shader_uniform(.033f, "u_outline_width");
    render_component->set_custom_shader_uniform(glm::vec3(1.f, 1.f, 0.f), "u_outline_color");
    
    static f32 angle = 0.f;
    angle += .1f;
    glm::vec3 direction_light_position;
    direction_light_position.x = 192.f * .5f + cosf(angle) * -192.f * .5f;
    direction_light_position.z = 192.f * .5f + sinf(angle) * -192.f * .5f;
    direction_light_position.y = 192.f * .5f;
    glm::vec3 direction_light_direction = glm::normalize(direction_light_position - glm::vec3(192.f * .5f, 0.f, 192.f * .5f));
    m_direction_light->set_direction(direction_light_direction);
    
    /*light_position = m_models["human_02"]->get_position() + m_models["human_02"]->get_right() * 1.5f;
    light_position.y = 1.5f;
    m_instanced_omni_lights->set_position(light_position, 2);
    
    light_position = m_models["human_02"]->get_position() + m_models["human_02"]->get_right() * -1.5f;
    light_position.y = 1.5f;
    m_instanced_omni_lights->set_position(light_position, 3);*/
}

void demo_game_scene::on_touch(const glm::vec3 &point, const glm::vec3& delta, const gb::ces_entity_shared_ptr &listener,
                               gb::e_input_element input_element, gb::e_input_state input_state)
{
    if(input_state == gb::e_input_state_pressed)
    {
        std::cout<<"on_touch"<<std::endl;
        m_3d_tile_cursor->set_position(glm::vec3(point.x, 1.f, point.z));
    }
}

void demo_game_scene::on_key_down(i32 key)
{
    std::cout<<"[on_key_down] : "<<key<<std::endl;
}

void demo_game_scene::on_key_up(i32 key)
{
    std::cout<<"[on_key_up] : "<<key<<std::endl;
}

void demo_game_scene::on_move_state_changed(i32 state)
{
    //m_camera_controller->set_move_state(state);
    //m_character_controller->set_move_state(state);
}

void demo_game_scene::on_rotate_state_changed(i32 state)
{
    //m_camera_controller->set_rotate_state(state);
    //m_character_controller->set_rotate_state(state);
}

void demo_game_scene::on_mouse_dragged(const glm::ivec2& delta)
{

}