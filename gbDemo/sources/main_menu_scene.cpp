//
//  main_menu_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "main_menu_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "button.h"
#include "textfield.h"
#include "joystick.h"
#include "heightmap.h"
#include "shape_3d.h"
#include "game_commands_container.h"
#include "ns_ui_commands.h"
#include "ces_sound_system.h"
#include "ces_heightmap_assembling_system.h"
#include "ces_heightmap_lod_system.h"
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
#include "ces_interaction_system.h"
#include "ces_track_route_component.h"
#include "ces_action_component.h"
#include "deferred_point_light_3d.h"
#include "advertisement_provider.h"

namespace game
{
    main_menu_scene::main_menu_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    main_menu_scene::~main_menu_scene()
    {
        
    }
    
    void main_menu_scene::create()
    {
        gb::scene_graph::create();

		glm::vec2 scene_2d_size = glm::vec2(667, 375);
        
        auto sound_system = std::make_shared<gb::al::ces_sound_system>();
        get_transition()->add_system(sound_system);

        m_gameplay_fabricator = std::make_shared<gameplay_fabricator>(get_fabricator());
        
        m_ui_base_fabricator = std::make_shared<gb::ui::ui_fabricator>(get_fabricator());
        m_gameplay_ui_fabricator = std::make_shared<gameplay_ui_fabricator>(get_fabricator(),
                                                                            m_ui_base_fabricator,
                                                                            m_scene_size);
        
        const auto camera_2d = std::make_shared<gb::camera_2d>(scene_2d_size.x,
                                                               scene_2d_size.y);
        set_camera_2d(camera_2d);
        
        auto camera_3d = std::make_shared<gb::camera_3d>(45.f, .1f, 1000.f,
                                                         glm::ivec4(0, 0, scene_2d_size.x, scene_2d_size.y), true);
        set_camera_3d(camera_3d);
        m_camera_3d = camera_3d;
        
        const auto fuel_label = m_ui_base_fabricator->create_textfield(glm::vec2(210.f, 32.f), "Fuel: 3");
        fuel_label->position = glm::vec2(8.f, 8.f);
        fuel_label->set_font_color(glm::u8vec4(255, 255, 0, 255));
        add_child(fuel_label);
        
        const auto plus_fuel_button = m_ui_base_fabricator->create_button(glm::vec2(32.f, 32.f), std::bind(&main_menu_scene::on_play_rewarded_video, this, std::placeholders::_1));
        plus_fuel_button->position = glm::vec2(186.f, 8.f);
        plus_fuel_button->set_text("+");
        plus_fuel_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        add_child(plus_fuel_button);
        
        const auto scores_label = m_ui_base_fabricator->create_textfield(glm::vec2(210.f, 32.f), "Scores: 7350");
        scores_label->position = glm::vec2(226.f, 8.f);
        scores_label->set_font_color(glm::u8vec4(255, 255, 0, 255));
        add_child(scores_label);
        
        const auto name_label = m_ui_base_fabricator->create_textfield(glm::vec2(210.f, 32.f), "Name: Racer");
        name_label->position = glm::vec2(444.f, 8.f);
        name_label->set_font_color(glm::u8vec4(255, 255, 0, 255));
        add_child(name_label);

        const auto in_game_transition_button = m_ui_base_fabricator->create_button(glm::vec2(210.f, 32.f), std::bind(&main_menu_scene::on_goto_in_game_scene, this, std::placeholders::_1));
        in_game_transition_button->position = glm::vec2(8.f, 64.f);
        in_game_transition_button->set_text("Play");
        in_game_transition_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        add_child(in_game_transition_button);
        
        const auto garage_transition_button = m_ui_base_fabricator->create_button(glm::vec2(210.f, 32.f), std::bind(&main_menu_scene::on_goto_in_game_scene, this, std::placeholders::_1));
        garage_transition_button->position = glm::vec2(8.f, 112.f);
        garage_transition_button->set_text("Garage");
        garage_transition_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        add_child(garage_transition_button);

        auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        sound_component->add_sound("music_01.mp3", true);
        sound_component->trigger_sound("music_01.mp3");
        ces_entity::add_component(sound_component);
        
        const auto scene = m_gameplay_fabricator->create_scene("main_menu_scene.tmx");
        main_menu_scene::add_child(scene);
        
        const auto track_route_component = scene->get_component<ces_track_route_component>();
        glm::vec2 start_point = track_route_component->start_point;
        
        main_menu_scene::enable_box2d_world(glm::vec2(-256.f),
                                            glm::vec2(256.f));
        
        m_car = m_gameplay_fabricator->create_car("character.human_01.xml");
        m_car->position = glm::vec3(start_point.x, 0.f, start_point.y);
        m_car->rotation = glm::vec3(0.f, 30.f, 0.f);
        main_menu_scene::add_child(m_car);
        
        const auto car_parts_component = m_car->get_component<ces_character_parts_component>();
        car_parts_component->get_part(ces_character_parts_component::parts::k_ui_name_label)->visible = false;
        car_parts_component->get_part(ces_character_parts_component::parts::k_ui_speed_label)->visible = false;
        car_parts_component->get_part(ces_character_parts_component::parts::k_ui_speed_value_label)->visible = false;
        car_parts_component->get_part(ces_character_parts_component::parts::k_ui_drift_label)->visible = false;
        car_parts_component->get_part(ces_character_parts_component::parts::k_ui_drift_value_label)->visible = false;
        
        camera_3d->set_look_at(glm::vec3(start_point.x, 0.f, start_point.y));
        camera_3d->set_rotation(-45.f);
        camera_3d->set_distance_to_look_at(glm::vec3(12.f, 24.f, 12.f));
        
        auto action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&main_menu_scene::on_update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        add_component(action_component);
    }
    
    void main_menu_scene::on_update(gb::ces_entity_const_shared_ptr entity, f32 dt)
    {
        auto current_camera_angle = m_camera_3d->get_rotation();
        current_camera_angle += .5f * dt;
        m_camera_3d->set_rotation(current_camera_angle);
        
        static f32 back_lights_blink_timestamp = 0.f;
        back_lights_blink_timestamp += dt;
        const auto car_parts_component = m_car->get_component<ces_character_parts_component>();
        const auto car_back_light_left = std::static_pointer_cast<gb::deferred_point_light_3d>(car_parts_component->get_part(ces_character_parts_component::parts::k_bl_light));
        const auto car_back_light_right = std::static_pointer_cast<gb::deferred_point_light_3d>(car_parts_component->get_part(ces_character_parts_component::parts::k_br_light));
        f32 current_ray_length = car_back_light_left->ray_length;
        if (back_lights_blink_timestamp > .33f)
        {
            if (current_ray_length > 0.f)
            {
                car_back_light_left->ray_length = 0.f;
                car_back_light_right->ray_length = 0.f;
            }
            else
            {
                car_back_light_left->ray_length = 1.f;
                car_back_light_right->ray_length = 1.f;
            }
            back_lights_blink_timestamp = 0.f;
        }
    }
    
    void main_menu_scene::on_goto_in_game_scene(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_in_game_scene::t_command>(on_goto_in_game_scene::guid);
        }
        else
        {
            assert(false);
        }
        //std::static_pointer_cast<gb::shape_3d>(m_car->get_component<ces_character_parts_component>()->get_body_part())->play_animation("idle");
        //m_character->play_animation("idle", true);
    }
    
    void main_menu_scene::on_play_rewarded_video(gb::ces_entity_const_shared_ptr entity)
    {
        advertisement_provider::shared_instance()->play_rewarded_video();
    }
    
    void main_menu_scene::on_goto_net_menu_scene(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_net_menu_scene::t_command>(on_goto_net_menu_scene::guid);
        }
        else
        {
            assert(false);
        }
    }

	void main_menu_scene::on_goto_ui_editor_scene(gb::ces_entity_const_shared_ptr entity) 
	{
        std::static_pointer_cast<gb::shape_3d>(m_car->get_component<ces_character_parts_component>()->get_body_part())->play_animation("run", true);
		/*if (m_external_commands)
		{
			m_external_commands->execute<on_goto_ui_editor_scene::t_command>(on_goto_ui_editor_scene::guid);
		}
		else
		{
			assert(false);
		}*/
	}
}
