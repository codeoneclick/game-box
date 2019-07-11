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
#include "db_helper.h"
#include "ces_user_database_component.h"
#include "ces_levels_database_component.h"
#include "store_provider.h"

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

		m_scene_size = glm::vec2(get_transition()->get_screen_width(),
                                 get_transition()->get_screen_height());
        
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
        
        const auto level_tutorial_system = std::make_shared<ces_level_tutorial_system>();
        get_transition()->add_system(level_tutorial_system);
        
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
        
        auto camera_3d = std::make_shared<gb::camera_3d>(60.f, .1f, 256.f,
                                                         glm::ivec4(0, 0, m_scene_size.x, m_scene_size.y), true);
        set_camera_3d(camera_3d);
        m_camera_3d = camera_3d;
        
        db_helper::fill_initial_values(shared_from_this(), m_gameplay_fabricator->get_database_coordinator(),
                                       m_gameplay_fabricator->get_levels_set_configuration("levels_set_configuration.xml"),
                                       m_gameplay_fabricator->get_cars_progression_configuration("cars_progression.xml"));
        
        const auto user_database_component = get_component<ces_user_database_component>();
        const auto garage_database_component = get_component<ces_garage_database_component>();
        const auto levels_database_component = get_component<ces_levels_database_component>();
        
        i32 next_level_id = levels_database_component->get_next_level_id();
        const auto level_data = levels_database_component->get_level(next_level_id);
        const auto level = m_gameplay_fabricator->create_scene(level_data->get_scene_filename());
        // const auto level = m_gameplay_fabricator->create_scene("level_9.tmx");
        add_child(level);
        
        const auto selected_car = garage_database_component->get_selected_car(1);
        std::stringstream selected_car_configuration_filename;
        selected_car_configuration_filename<<"car_0";
        selected_car_configuration_filename<<selected_car->get_id();
        const auto car = m_gameplay_fabricator->create_ai_car(selected_car_configuration_filename.str());
        m_gameplay_fabricator->reskin_car(car, selected_car_configuration_filename.str(), selected_car->get_skin_id());
        m_gameplay_fabricator->place_car_on_level(level, car, 0);
        car->add_component(std::make_shared<ces_car_camera_follow_component>());
        add_child(car);
        
        glm::vec3 car_rotation = car->rotation;
        camera_3d->set_rotation(car_rotation.y - 90.f);
        
        const auto car_parts_component = car->get_component<ces_car_parts_component>();
        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_name_label)->visible = false;
        
#if defined(__IOS__)
        
        const auto tickets_label = m_gameplay_ui_fabricator->create_tickets_label("");
        add_child(tickets_label);
        
        std::string tickets_text = "TICKETS: ";
        tickets_text.append(std::to_string(user_database_component->get_tickets(1)));
        std::static_pointer_cast<gb::ui::textfield>(tickets_label)->set_text(tickets_text);
        
#endif
        
        const auto in_game_transition_button = m_gameplay_ui_fabricator->create_open_levels_list_dialog_button("");
        add_child(in_game_transition_button);
        
        const auto garage_transition_button = m_gameplay_ui_fabricator->create_open_garage_button("");
        add_child(garage_transition_button);
        
#if defined(__IOS__)
        
        const auto shop_button = m_gameplay_ui_fabricator->create_open_shop_button("");
        add_child(shop_button);
        
#else
        
        const auto quit_game_button = m_gameplay_ui_fabricator->create_exit_button("");
        add_child(quit_game_button);
        
#endif
        
#if defined(__FACEBOOK_LOGIN__)
        
        const auto facebook_button = m_gameplay_ui_fabricator->create_facebook_button("");
        add_child(facebook_button);
        
#endif
        
        const auto levels_list_dialog = m_gameplay_ui_fabricator->create_levels_list_dialog("");
        add_child(levels_list_dialog);
        
        const auto shop_dialog = m_gameplay_ui_fabricator->create_shop_dialog("");
        add_child(shop_dialog);
        
        const auto goto_racing_button = m_gameplay_ui_fabricator->create_goto_racing_button("");
        add_child(goto_racing_button);
        
        const auto stars_progress_label = m_gameplay_ui_fabricator->create_stars_progress_label("");
        add_child(stars_progress_label);
        
        const auto current_rank = user_database_component->get_rank(1);
        const auto stars_for_rank = user_database_component->get_stars_for_rank(current_rank + 1);
        const auto current_stars_count_for_rank = user_database_component->get_collected_stars(1, current_rank);
        
        const auto stars_progress_info_label = m_gameplay_ui_fabricator->create_stars_progress_info_label("");
        add_child(stars_progress_info_label);
        std::stringstream stars_progress_str_stream;
        stars_progress_str_stream<<current_stars_count_for_rank<<"/"<<stars_for_rank;
        std::static_pointer_cast<gb::ui::textfield>(stars_progress_info_label)->set_text(stars_progress_str_stream.str());
        
        const auto stars_progress_bar = m_gameplay_ui_fabricator->create_stars_progress_bar("");
        add_child(stars_progress_bar);
        std::static_pointer_cast<gb::ui::progress_bar>(stars_progress_bar)->set_progress(static_cast<f32>(current_stars_count_for_rank) / static_cast<f32>(stars_for_rank));
        
        const auto stars_progress_button = m_gameplay_ui_fabricator->create_stars_progress_button("");
        add_child(stars_progress_button);
        
        const auto rank_info_label = m_gameplay_ui_fabricator->create_rank_info_label("");
        add_child(rank_info_label);
        std::stringstream rank_str_stream;
        rank_str_stream<<"RANK: "<<current_rank;
        std::static_pointer_cast<gb::ui::textfield>(rank_info_label)->set_text(rank_str_stream.str());
        
        const auto screen_overlay = m_gameplay_ui_fabricator->create_screen_overlay("");
        add_child(screen_overlay);
        
        const auto full_tickets_dialog = m_gameplay_ui_fabricator->create_full_tickets_dialog("");
        add_child(full_tickets_dialog);
        
        const auto not_enough_tickets_dialog = m_gameplay_ui_fabricator->create_not_enough_tickets_dialog("");
        add_child(not_enough_tickets_dialog);
        
#if defined(__OSX__)
        
        const auto quit_game_dialog = m_gameplay_ui_fabricator->create_exit_game_dialog("");
        add_child(quit_game_dialog);

#endif
        
        auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        sound_component->add_sound("music_01.mp3", true);
        sound_component->add_sound("music_03.mp3", true);
        sound_component->add_sound("music_05.mp3", true);
        sound_component->trigger_sound("music_01.mp3", false);
        add_component(sound_component);
        
        enable_box2d_world(glm::vec2(-4096.f),
                           glm::vec2(4096.f));
        
        const auto render_technique_uniforms_component = get_component<gb::ces_render_technique_uniforms_component>();
        if (render_technique_uniforms_component)
        {
            render_technique_uniforms_component->construct_uniforms<ss_output_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.compose");
            
            render_technique_uniforms_component->construct_uniforms<ss_tv_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.tv");
            
            render_technique_uniforms_component->construct_uniforms<ss_cross_fade_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.crossfade");
            
            render_technique_uniforms_component->construct_uniforms<ssao_shader_uniforms>(gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.ao");
            
            auto uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.compose");
            uniforms_wrapper->set(-1.f, "vignetting_edge_size");
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.tv");
            uniforms_wrapper->set(1.f, "enabled");
            
            uniforms_wrapper = render_technique_uniforms_component->get_uniforms("ss.crossfade");
            uniforms_wrapper->set(-1.f, "enabled");
            uniforms_wrapper->set(.0f, "progress");
        }
        
        if (!user_database_component->get_is_purchased_no_ads(1))
        {
            advertisement_provider::shared_instance()->show_banner();
        }
        
        store_provider::shared_instance()->request_products();
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
	}
}
