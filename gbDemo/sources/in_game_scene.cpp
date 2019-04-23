//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii.s on 4/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "in_game_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "button.h"
#include "joystick.h"
#include "shape_3d.h"
#include "game_commands_container.h"
#include "ns_ui_commands.h"
#include "ces_sound_system.h"
#include "ces_sound_component.h"
#include "character_configuration.h"
#include "gameplay_configuration_accessor.h"
#include "anim_fabricator.h"
#include "gameplay_fabricator.h"
#include "ces_transformation_component.h"
#include "binding_board_model.h"
#include "binding_shared_model.h"
#include "binding_cell_model.h"
#include "binding_element_model.h"
#include "binding_regular_element_model.h"
#include "resource_accessor.h"
#include "scene_2d.h"
#include "scene_3d.h"
#include "scene_3d_loading_operation.h"
#include "scene_2d_loading_operation.h"
#include "ces_character_parts_component.h"
#include "gameplay_ui_fabricator.h"
#include "ces_ui_interaction_system.h"
#include "ces_car_simulator_system.h"
#include "ces_ai_car_replay_system.h"
#include "ces_interaction_system.h"
#include "ces_car_sound_system.h"
#include "ces_track_route_component.h"
#include "ss_output_render_technique_uniforms.h"
#include "button.h"
#include "textfield.h"

namespace game
{
    in_game_scene::in_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    void in_game_scene::create()
    {
        gb::scene_graph::create();
        
        m_scene_size = glm::vec2(667, 375);
        
        auto sound_system = std::make_shared<gb::al::ces_sound_system>();
        get_transition()->add_system(sound_system);
        
        auto ui_interaction_system = std::make_shared<ces_ui_interaction_system>();
        get_transition()->add_system(ui_interaction_system);
        
        auto interaction_system = std::make_shared<ces_interaction_system>();
        get_transition()->add_system(interaction_system);
        
        const auto car_simulator_system = std::make_shared<ces_car_simulator_system>();
        get_transition()->add_system(car_simulator_system);
        
        const auto ai_car_replay_system = std::make_shared<ces_ai_car_replay_system>();
        get_transition()->add_system(ai_car_replay_system);
        
        const auto car_sound_system = std::make_shared<ces_car_sound_system>();
        get_transition()->add_system(car_sound_system);
        
        m_gameplay_fabricator = std::make_shared<gameplay_fabricator>(get_fabricator());
        
        m_ui_base_fabricator = std::make_shared<gb::ui::ui_fabricator>(get_fabricator());
        m_gameplay_ui_fabricator = std::make_shared<gameplay_ui_fabricator>(get_fabricator(),
                                                                            m_ui_base_fabricator,
                                                                            m_scene_size);
        
        const auto camera_2d = std::make_shared<gb::camera_2d>(m_scene_size.x,
                                                               m_scene_size.y);
        
        set_camera_2d(camera_2d);
        
        const auto camera_3d = std::make_shared<gb::camera_3d>(45.f, .1f, 1000.f,
                                                               glm::ivec4(0, 0, m_scene_size.x, m_scene_size.y), true);
        camera_3d->set_rotation(-180.f);
        set_camera_3d(camera_3d);
        
        auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        //sound_component->add_sound("in_game_music_01.mp3", true);
        //sound_component->trigger_sound("in_game_music_01.mp3");
        ces_entity::add_component(sound_component);
       
        const auto scene = m_gameplay_fabricator->create_scene("track_01.tmx");
        add_child(scene);
        
        const auto track_route_component = scene->get_component<ces_track_route_component>();
        glm::vec2 start_point = track_route_component->start_point;
        
        enable_box2d_world(glm::vec2(-256.f),
                           glm::vec2(256.f));
        
        m_car = m_gameplay_fabricator->create_player_car("character.human_01.xml");
        m_car->position = glm::vec3(start_point.x, 0.f, start_point.y);
        m_car->rotation = glm::vec3(0.f, -90.f, 0.f);
        add_child(m_car);
        
        const auto opponent_car_01 = m_gameplay_fabricator->create_opponent_car("character.human_01.xml");
        opponent_car_01->position = glm::vec3(start_point.x, 0.f, start_point.y);
        opponent_car_01->rotation = glm::vec3(0.f, -90.f, 0.f);
        add_child(opponent_car_01);
        
        const auto render_technique_uniforms_component = get_component<gb::ces_render_technique_uniforms_component>();
        if (render_technique_uniforms_component)
        {
            render_technique_uniforms_component->construct_uniforms<ss_output_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.output");
            const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.output");
            uniforms_wrapper->set(-1.f, "vignetting_edge_size");
        }
        
        const auto main_menu_transition_button = m_ui_base_fabricator->create_button(glm::vec2(128.f, 32.f), std::bind(&in_game_scene::on_goto_main_menu_scene, this, std::placeholders::_1));
        main_menu_transition_button->position = glm::vec2(8.f, 8.f);
        main_menu_transition_button->set_text("Back");
        main_menu_transition_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        add_child(main_menu_transition_button);
        
        const auto scores_label = m_ui_base_fabricator->create_textfield(glm::vec2(200.f, 32.f), "Scores: 7350");
        scores_label->position = glm::vec2(460.f, 8.f);
        scores_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        add_child(scores_label);
        
        const auto drift_time_label = m_gameplay_ui_fabricator->create_drift_time_label("");
        add_child(drift_time_label);
    }
    
    void in_game_scene::on_goto_main_menu_scene(gb::ces_entity_const_shared_ptr entity)
    {
        
    }
}
