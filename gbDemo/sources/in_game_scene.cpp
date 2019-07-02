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
#include "ces_ai_system.h"
#include "ces_sound_component.h"
#include "gameplay_configuration_accessor.h"
#include "gameplay_fabricator.h"
#include "ces_transformation_component.h"
#include "resource_accessor.h"
#include "scene_2d.h"
#include "scene_3d.h"
#include "scene_3d_loading_operation.h"
#include "scene_2d_loading_operation.h"
#include "ces_car_parts_component.h"
#include "gameplay_ui_fabricator.h"
#include "ces_ui_interaction_system.h"
#include "ces_car_simulator_system.h"
#include "ces_ai_car_replay_system.h"
#include "ces_interaction_system.h"
#include "ces_car_sound_system.h"
#include "ces_level_route_component.h"
#include "ss_render_technique_custom_uniforms.h"
#include "ces_car_camera_follow_component.h"
#include "ces_level_descriptor_component.h"
#include "ces_action_component.h"
#include "ces_car_visual_effects_system.h"
#include "button.h"
#include "textfield.h"
#include "glm_extensions.h"

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
        car_simulator_system->set_is_paused(true);
        get_transition()->add_system(car_simulator_system);
        
        const auto car_visual_effects_system = std::make_shared<ces_car_visual_effects_system>();
        get_transition()->add_system(car_visual_effects_system);
        
        auto car_ai_system = std::make_shared<ces_ai_system>();
        car_ai_system->set_is_paused(true);
        get_transition()->add_system(car_ai_system);
        
        const auto ai_car_replay_system = std::make_shared<ces_ai_car_replay_system>();
        ai_car_replay_system->set_is_paused(true);
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
        camera_3d->set_rotation(0.f);
        set_camera_3d(camera_3d);
        
        auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        //sound_component->add_sound("in_game_music_01.mp3", true);
        //sound_component->trigger_sound("in_game_music_01.mp3");
        ces_entity::add_component(sound_component);
       
        m_track = m_gameplay_fabricator->create_scene("track_output.tmx");
        add_child(m_track);
        
        const auto track_route_component = m_track->get_component<ces_level_route_component>();
        std::vector<glm::vec2> spawners = track_route_component->spawners;
        
        enable_box2d_world(glm::vec2(-256.f),
                           glm::vec2(256.f));
        
        m_car = m_gameplay_fabricator->create_player_car("character.human_01.xml");
        place_car_on_track(m_car, spawners.at(0));
        m_car->add_component(std::make_shared<ces_car_camera_follow_component>());
        add_child(m_car);
        
        const auto car_parts_component = m_car->get_component<ces_car_parts_component>();
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_label)->visible = false;
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label)->visible = false;
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label)->visible = false;
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label)->visible = false;
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_label)->visible = false;
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label)->visible = false;
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow)->visible = false;
        
        glm::vec3 main_car_rotation = m_car->rotation;
        camera_3d->set_rotation(main_car_rotation.y - 90.f);
        
        const auto opponent_car_01 = m_gameplay_fabricator->create_ai_car("character.human_01.xml");
        place_car_on_track(opponent_car_01, spawners.at(1));
        add_child(opponent_car_01);
        
        const auto opponent_car_02 = m_gameplay_fabricator->create_ai_car("character.human_01.xml");
        place_car_on_track(opponent_car_02, spawners.at(2));
        add_child(opponent_car_02);
        
        const auto opponent_car_03 = m_gameplay_fabricator->create_ai_car("character.human_01.xml");
        place_car_on_track(opponent_car_03, spawners.at(3));
        add_child(opponent_car_03);
        
        const auto render_technique_uniforms_component = get_component<gb::ces_render_technique_uniforms_component>();
        if (render_technique_uniforms_component)
        {
            render_technique_uniforms_component->construct_uniforms<ss_output_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.compose");
            
            render_technique_uniforms_component->construct_uniforms<ss_tv_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.tv");
            
            auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
            uniforms_wrapper->set(-1.f, "vignetting_edge_size");
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.tv");
            uniforms_wrapper->set(1.f, "enabled");
        }
        
        const auto main_menu_transition_button = m_ui_base_fabricator->create_button(glm::vec2(128.f, 24.f), std::bind(&in_game_scene::on_goto_main_menu_scene, this, std::placeholders::_1));
        main_menu_transition_button->position = glm::vec2(8.f, 8.f);
        main_menu_transition_button->set_text("Back");
        main_menu_transition_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        add_child(main_menu_transition_button);
        
        //const auto scores_label = m_gameplay_ui_fabricator->create_scores_label("");
        //add_child(scores_label);
        
        const auto countdown_label = m_gameplay_ui_fabricator->create_countdown_label("");
        add_child(countdown_label);
        
        const auto track_descriptor_component = m_track->get_component<ces_level_descriptor_component>();
        track_descriptor_component->start_timestamp = std::get_tick_count();
        
        auto action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&in_game_scene::on_update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        add_component(action_component);
    }
    
    void in_game_scene::on_goto_main_menu_scene(gb::ces_entity_const_shared_ptr entity)
    {
        
    }
    
    void in_game_scene::place_car_on_track(const gb::game_object_3d_shared_ptr &car, const glm::vec2 &spawner_position)
    {
        const auto track_route_component = m_track->get_component<ces_level_route_component>();
        std::vector<glm::vec2> route = track_route_component->route;
        i32 nearest_next_checkpoint_index = 0;
        f32 nearest_next_checkpoint_distance = glm::distance(glm::vec2(spawner_position.x, spawner_position.y), route.at(nearest_next_checkpoint_index));
        
        i32 index = 0;
        for (auto route_it : route)
        {
            f32 distance = glm::distance(glm::vec2(spawner_position.x, spawner_position.y), route_it);
            if (distance < nearest_next_checkpoint_distance)
            {
                nearest_next_checkpoint_distance = distance;
                nearest_next_checkpoint_index = index;
            }
            index++;
        }
        
        nearest_next_checkpoint_index = (nearest_next_checkpoint_index + 2) % route.size();
        auto goal_position = route.at(nearest_next_checkpoint_index);
        f32 goal_rotation = glm::wrap_degrees(glm::degrees(atan2(goal_position.x - spawner_position.x, goal_position.y - spawner_position.y)));
        
        if (goal_rotation >= 0.f && goal_rotation <= 45.f)
        {
            goal_rotation = 0.f;
        }
        else if (goal_rotation > 45.f && goal_rotation <= 135.f)
        {
            goal_rotation = 90.f;
        }
        else if (goal_rotation > 135.f && goal_rotation <= 225.f)
        {
            goal_rotation = 180.f;
        }
        else if (goal_rotation > 225.f && goal_rotation <= 315.f)
        {
            goal_rotation = 270.f;
        }
        else
        {
            goal_rotation = 0.f;
        }
        
        car->position = glm::vec3(spawner_position.x, 0.f, spawner_position.y);
        car->rotation = glm::vec3(0.f, goal_rotation, 0.f);
    }
    
    void in_game_scene::on_update(gb::ces_entity_const_shared_ptr entity, f32 dt)
    {
        const auto track_descriptor_component = m_track->get_component<ces_level_descriptor_component>();
        f32 track_start_timestamp = track_descriptor_component->start_timestamp;
        f32 delta = std::get_tick_count() - track_start_timestamp;
        delta /= 1000.f;
        f32 track_countdown_time = track_descriptor_component->countdown_time;
        f32 track_current_countdown_time = track_countdown_time - delta;
        track_descriptor_component->current_countdown_time = track_current_countdown_time;
        
        if (track_current_countdown_time <= 0.f)
        {
            get_transition()->get_system<ces_car_simulator_system>()->set_is_paused(false);
            get_transition()->get_system<ces_ai_system>()->set_is_paused(false);
            get_transition()->get_system<ces_ai_car_replay_system>()->set_is_paused(false);
            
            const auto car_parts_component = m_car->get_component<ces_car_parts_component>();
            car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_label)->visible = true;
            car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label)->visible = true;
            car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label)->visible = true;
            car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label)->visible = true;
            car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_label)->visible = true;
            car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label)->visible = true;
            car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow)->visible = true;
            
            const auto render_technique_uniforms_component = get_component<gb::ces_render_technique_uniforms_component>();
            if (render_technique_uniforms_component)
            {
                const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.tv");
                uniforms_wrapper->set(-1.f, "enabled");
            }
        }
        else
        {
            const auto render_technique_uniforms_component = get_component<gb::ces_render_technique_uniforms_component>();
            if (render_technique_uniforms_component)
            {
                const auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.tv");
                uniforms_wrapper->set(static_cast<f32>(std::get_tick_count()), "time");
            }
        }
    }
}
