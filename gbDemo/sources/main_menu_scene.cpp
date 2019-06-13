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
#include "progress_bar.h"
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
#include "ces_car_parts_component.h"
#include "gameplay_ui_fabricator.h"
#include "ces_ui_interaction_system.h"
#include "ces_ai_system.h"
#include "ces_car_simulator_system.h"
#include "ces_interaction_system.h"
#include "ces_level_route_component.h"
#include "ces_action_component.h"
#include "deferred_point_light_3d.h"
#include "advertisement_provider.h"
#include "ss_render_technique_custom_uniforms.h"
#include "ces_car_camera_follow_component.h"
#include "ces_car_visual_effects_system.h"
#include "glm_extensions.h"
#include "ces_level_descriptor_component.h"
#include "ces_car_sound_system.h"
#include "ces_state_automat_system.h"
#include "ces_scene_state_automat_component.h"
#include "ces_scene_visual_effects_system.h"
#include "ces_scene_visual_effects_component.h"
#include "ces_scene_fabricator_component.h"
#include "ces_garage_database_component.h"
#include "ces_car_progression_system.h"
#include "ces_level_tutorial_system.h"

namespace game
{
    main_menu_scene::main_menu_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        //m_opponents.fill(nullptr);
    }
    
    main_menu_scene::~main_menu_scene()
    {
        
    }
    
    void main_menu_scene::create()
    {
        gb::scene_graph::create();

		m_scene_size = glm::vec2(667, 375);
        
        auto sound_system = std::make_shared<gb::al::ces_sound_system>();
        get_transition()->add_system(sound_system);
        
        const auto car_sound_system = std::make_shared<ces_car_sound_system>();
        get_transition()->add_system(car_sound_system);
        
        const auto car_simulator_system = std::make_shared<ces_car_simulator_system>();
        get_transition()->add_system(car_simulator_system);
        
        const auto car_visual_effects_system = std::make_shared<ces_car_visual_effects_system>();
        get_transition()->add_system(car_visual_effects_system);
        
        auto car_ai_system = std::make_shared<ces_ai_system>();
        get_transition()->add_system(car_ai_system);
        
        auto car_progression_system = std::make_shared<ces_car_progression_system>();
        get_transition()->add_system(car_progression_system);
        
        auto interaction_system = std::make_shared<ces_interaction_system>();
        interaction_system->set_is_paused(true);
        get_transition()->add_system(interaction_system);
        
        auto ui_interaction_system = std::make_shared<ces_ui_interaction_system>();
        get_transition()->add_system(ui_interaction_system);
        
        const auto state_automat_system = std::make_shared<ces_state_automat_system>();
        get_transition()->add_system(state_automat_system);
        
        //const auto level_tutorial_system = std::make_shared<ces_level_tutorial_system>();
        //get_transition()->add_system(level_tutorial_system);
        
        const auto scene_visual_effects_system = std::make_shared<ces_scene_visual_effects_system>();
        get_transition()->add_system(scene_visual_effects_system);
        
        m_gameplay_fabricator = std::make_shared<gameplay_fabricator>(get_fabricator());
        
        m_ui_base_fabricator = std::make_shared<gb::ui::ui_fabricator>(get_fabricator());
        m_gameplay_ui_fabricator = std::make_shared<gameplay_ui_fabricator>(get_fabricator(),
                                                                            m_ui_base_fabricator,
                                                                            m_scene_size);
        
        add_component<ces_scene_state_automat_component>();
        get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_main_menu;
        get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_none;
        
        add_component<ces_scene_visual_effects_component>();
        get_component<ces_scene_visual_effects_component>()->is_noises_enabled = true;
        get_component<ces_scene_visual_effects_component>()->is_crossfade_enabled = false;
        
        add_component<ces_scene_fabricator_component>();
        get_component<ces_scene_fabricator_component>()->set_parameters(m_gameplay_fabricator,
                                                                        m_gameplay_ui_fabricator);
       
        const auto camera_2d = std::make_shared<gb::camera_2d>(m_scene_size.x,
                                                               m_scene_size.y);
        set_camera_2d(camera_2d);
        
        auto camera_3d = std::make_shared<gb::camera_3d>(45.f, .1f, 1000.f,
                                                         glm::ivec4(0, 0, m_scene_size.x, m_scene_size.y), true);
        set_camera_3d(camera_3d);
        m_camera_3d = camera_3d;
        
        m_gameplay_fabricator->configure_levels_set(shared_from_this(), "levels_set_configuration.xml");
        
        const auto level = m_gameplay_fabricator->create_scene("track_output.tmx");
        add_child(level);
        
        const auto garage_database_component = level->get_component<ces_garage_database_component>();
        if (!garage_database_component->is_garage_exist(1))
        {
            garage_database_component->add_garage(1);
        }
        
        if (!garage_database_component->is_car_exist(1, 1))
        {
            garage_database_component->add_car_to_garage(1, 1);
            garage_database_component->select_car(1, 1);
        }
        
        if (!garage_database_component->is_car_exist(1, 2))
        {
            garage_database_component->add_car_to_garage(1, 2);
        }
        
        if (!garage_database_component->is_car_exist(1, 3))
        {
            garage_database_component->add_car_to_garage(1, 3);
        }
        
        if (!garage_database_component->is_car_exist(1, 4))
        {
            garage_database_component->add_car_to_garage(1, 4);
        }
        
        if (!garage_database_component->is_car_exist(1, 5))
        {
            garage_database_component->add_car_to_garage(1, 5);
        }
        
        if (!garage_database_component->is_car_exist(1, 6))
        {
            garage_database_component->add_car_to_garage(1, 6);
        }
        
        if (!garage_database_component->is_car_exist(1, 7))
        {
            garage_database_component->add_car_to_garage(1, 7);
        }
        
        if (!garage_database_component->is_car_exist(1, 8))
        {
            garage_database_component->add_car_to_garage(1, 8);
        }
        
        const auto selected_car = garage_database_component->get_selected_car(1);
        std::stringstream selected_car_configuration_filename;
        selected_car_configuration_filename<<"car_0";
        selected_car_configuration_filename<<selected_car->get_id();
        const auto car = m_gameplay_fabricator->create_ai_car(selected_car_configuration_filename.str());
        m_gameplay_fabricator->reskin_car(car, selected_car_configuration_filename.str(), selected_car->get_skin_id());
        m_gameplay_fabricator->place_car_on_level(level, car, 0);
        car->add_component(std::make_shared<ces_car_camera_follow_component>());
        add_child(car);
        
        const auto car_parts_component = car->get_component<ces_car_parts_component>();
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_name_label)->visible = false;
        
        const auto tickets_label = m_gameplay_ui_fabricator->create_tickets_label("");
        add_child(tickets_label);
        
        //const auto name_label = m_ui_base_fabricator->create_textfield(glm::vec2(210.f, 24.f), "Name: Racer");
        //name_label->position = glm::vec2(444.f, 8.f);
        //name_label->set_font_color(glm::u8vec4(255, 255, 0, 255));
        //name_label->set_editable(true);
        //add_child(name_label);
        
        const auto label_1 = m_gameplay_ui_fabricator->create_tutorial_steer_left_label("");
        label_1->visible = false;
        add_child(label_1);
        const auto label_2 = m_gameplay_ui_fabricator->create_tutorial_steer_right_label("");
        label_2->visible = false;
        add_child(label_2);
        
        const auto in_game_transition_button = m_gameplay_ui_fabricator->create_open_levels_list_dialog_button("");
        add_child(in_game_transition_button);
        
        const auto garage_transition_button = m_gameplay_ui_fabricator->create_open_garage_button("");
        add_child(garage_transition_button);
        
        const auto levels_list_dialog = m_gameplay_ui_fabricator->create_levels_list_dialog("");
        add_child(levels_list_dialog);
        
        const auto goto_racing_button = m_gameplay_ui_fabricator->create_goto_racing_button("");
        add_child(goto_racing_button);
        
        const auto stars_progress_label = m_gameplay_ui_fabricator->create_stars_progress_label("");
        add_child(stars_progress_label);
        
        const auto stars_progress_bar = m_gameplay_ui_fabricator->create_stars_progress_bar("");
        add_child(stars_progress_bar);
        std::static_pointer_cast<gb::ui::progress_bar>(stars_progress_bar)->set_progress(.25f);
        
        const auto stars_progress_button = m_gameplay_ui_fabricator->create_stars_progress_button("");
        add_child(stars_progress_button);

        auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        sound_component->add_sound("in_game_music_01.mp3", true);
        sound_component->trigger_sound("in_game_music_01.mp3");
        ces_entity::add_component(sound_component);
        
        enable_box2d_world(glm::vec2(-4096.f),
                           glm::vec2(4096.f));
        
        auto action_component = std::make_shared<gb::ces_action_component>();
        action_component->set_update_callback(std::bind(&main_menu_scene::on_update, this,
                                                        std::placeholders::_1, std::placeholders::_2));
        add_component(action_component);
        
        const auto render_technique_uniforms_component = get_component<gb::ces_render_technique_uniforms_component>();
        if (render_technique_uniforms_component)
        {
            render_technique_uniforms_component->construct_uniforms<ss_output_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.compose");
            
            render_technique_uniforms_component->construct_uniforms<ss_tv_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.tv");
            
            render_technique_uniforms_component->construct_uniforms<ss_cross_fade_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.crossfade");
            
            auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
            uniforms_wrapper->set(-1.f, "vignetting_edge_size");
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.tv");
            uniforms_wrapper->set(1.f, "enabled");
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.crossfade");
            uniforms_wrapper->set(-1.f, "enabled");
            uniforms_wrapper->set(.0f, "progress");
        }
        
        advertisement_provider::shared_instance()->show_banner();
    }
    
    /*void main_menu_scene::de_init()
    {
        if (m_level)
        {
            m_level->remove_from_parent();
            m_level = nullptr;
        }
        
        if (m_car)
        {
            m_car->remove_from_parent();
            m_car = nullptr;
        }
        
        for (i32 i = 0; i < m_opponents.size(); ++i)
        {
            if (m_opponents[i])
            {
                m_opponents[i]->remove_from_parent();
                m_opponents[i] = nullptr;
            }
        }
    }
    
    void main_menu_scene::init_scene_as_main_menu(const std::string& filename)
    {
        de_init();
        
        
    }
    
    void main_menu_scene::init_scene_as_in_game(const std::string& filename)
    {
        de_init();
        
        m_level = m_gameplay_fabricator->create_scene(filename);
        main_menu_scene::add_child(m_level);
        
        const auto level_route_component = m_level->get_component<ces_level_route_component>();
        std::vector<glm::vec2> spawners = level_route_component->spawners;
        
        m_car = m_gameplay_fabricator->create_player_car("character.human_01.xml");
        place_car_on_level(m_car, spawners.at(0));
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
        m_camera_3d->set_rotation(main_car_rotation.y - 90.f);
        
        const auto opponent_car_01 = m_gameplay_fabricator->create_opponent_car("character.human_01.xml");
        place_car_on_level(opponent_car_01, spawners.at(1));
        add_child(opponent_car_01);
        
        const auto opponent_car_02 = m_gameplay_fabricator->create_opponent_car("character.human_01.xml");
        place_car_on_level(opponent_car_02, spawners.at(2));
        add_child(opponent_car_02);
        
        const auto opponent_car_03 = m_gameplay_fabricator->create_opponent_car("character.human_01.xml");
        place_car_on_level(opponent_car_03, spawners.at(3));
        add_child(opponent_car_03);
        
        get_transition()->get_system<ces_car_simulator_system>()->set_is_paused(true);
        get_transition()->get_system<ces_interaction_system>()->set_is_paused(true);
        get_transition()->get_system<ces_ai_system>()->set_is_paused(true);
        
        const auto level_descriptor_component = m_level->get_component<ces_level_descriptor_component>();
        level_descriptor_component->start_timestamp = std::get_tick_count();
        level_descriptor_component->is_started = true;
    }*/
    
    void main_menu_scene::on_update(gb::ces_entity_const_shared_ptr entity, f32 dt)
    {
    /*    bool is_crossfade_enabled = false;
        f32 crossfade_progress = 0.f;
        if (m_is_scene_loading)
        {
            is_crossfade_enabled = true;
            if (m_scene_loading_progress <= 1.f)
            {
                m_scene_loading_progress += m_scene_loading_interval;
                crossfade_progress = m_scene_loading_progress;
            }
            else
            {
                m_is_scene_loading = false;
                m_is_scene_loaded = false;
                m_scene_loading_progress = 1.f;
                init_scene_as_in_game("track_output.tmx");
            }
        }
        else if (!m_is_scene_loaded)
        {
            is_crossfade_enabled = true;
            if (m_scene_loading_progress >= 0.f)
            {
                m_scene_loading_progress -= m_scene_loading_interval;
                crossfade_progress = m_scene_loading_progress;
            }
            else
            {
                m_is_scene_loaded = true;
                m_scene_loading_progress = 0.f;
                is_crossfade_enabled = false;
            }
        }
        
        const auto render_technique_uniforms_component = get_component<gb::ces_render_technique_uniforms_component>();
        if (render_technique_uniforms_component)
        {
            auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.tv");
            uniforms_wrapper->set(static_cast<f32>(std::get_tick_count()), "time");
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.crossfade");
            uniforms_wrapper->set(is_crossfade_enabled ? 1.f : -1.f, "enabled");
            uniforms_wrapper->set(crossfade_progress, "progress");
        }
        
        const auto level_descriptor_component = m_level->get_component<ces_level_descriptor_component>();
        if (level_descriptor_component->is_started)
        {
            f32 start_timestamp = level_descriptor_component->start_timestamp;
            f32 delta = std::get_tick_count() - start_timestamp;
            delta /= 1000.f;
            f32 countdown_time = level_descriptor_component->countdown_time;
            f32 current_countdown_time = countdown_time - delta;
            level_descriptor_component->current_countdown_time = current_countdown_time;
            
            if (current_countdown_time <= 0.f)
            {
                get_transition()->get_system<ces_car_simulator_system>()->set_is_paused(false);
                get_transition()->get_system<ces_interaction_system>()->set_is_paused(false);
                get_transition()->get_system<ces_ai_system>()->set_is_paused(false);
                
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
        }*/
    }
    
    void main_menu_scene::on_goto_in_game_scene(gb::ces_entity_const_shared_ptr entity)
    {
        get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_in_game;
        get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
        
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
        //std::static_pointer_cast<gb::shape_3d>(m_car->get_component<ces_car_parts_component>()->get_body_part())->play_animation("run", true);
		/*if (m_external_commands)
		{
			m_external_commands->execute<on_goto_ui_editor_scene::t_command>(on_goto_ui_editor_scene::guid);
		}
		else
		{
			assert(false);
		}*/
	}
    
    /*void main_menu_scene::place_car_on_level(const gb::game_object_3d_shared_ptr &car, const glm::vec2 &spawner_position)
    {
        const auto level_route_component = m_level->get_component<ces_level_route_component>();
        std::vector<glm::vec2> route = level_route_component->route;
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
    }*/
}
