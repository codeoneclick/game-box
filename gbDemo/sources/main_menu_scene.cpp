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
        main_menu_scene::get_transition()->add_system(sound_system);
        
        auto ui_interaction_system = std::make_shared<ces_ui_interaction_system>();
        main_menu_scene::get_transition()->add_system(ui_interaction_system);
        
        auto interaction_system = std::make_shared<ces_interaction_system>();
        main_menu_scene::get_transition()->add_system(interaction_system);
        
        const auto car_simulator_system = std::make_shared<ces_car_simulator_system>();
        main_menu_scene::get_transition()->add_system(car_simulator_system);

		/*auto heightmap_assembling_system = std::make_shared<gb::ces_heightmap_assembling_system>();
		heightmap_assembling_system->set_resource_accessor(main_menu_scene::get_transition()->get_resource_accessor());
		main_menu_scene::get_transition()->add_system(heightmap_assembling_system);

		auto heightmap_lod_system = std::make_shared<gb::ces_heightmap_lod_system>();
		heightmap_lod_system->set_resource_accessor(main_menu_scene::get_transition()->get_resource_accessor());
		main_menu_scene::get_transition()->add_system(heightmap_lod_system);*/
        
        m_gameplay_fabricator = std::make_shared<gameplay_fabricator>(main_menu_scene::get_fabricator());
        
        m_ui_base_fabricator = std::make_shared<gb::ui::ui_fabricator>(main_menu_scene::get_fabricator());
        m_gameplay_ui_fabricator = std::make_shared<gameplay_ui_fabricator>(main_menu_scene::get_fabricator(),
                                                                            m_ui_base_fabricator,
                                                                            m_scene_size);
        
        m_camera_2d = std::make_shared<gb::camera_2d>(scene_2d_size.x,
			scene_2d_size.y);
        main_menu_scene::set_camera_2d(m_camera_2d);
        
        auto camera_3d = std::make_shared<gb::camera_3d>(60.f, .001f, 4096.f,
                                                         glm::ivec4(0,
                                                                    0,
                                                                    scene_2d_size.x,
                                                                    scene_2d_size.y), true);
        camera_3d->set_distance_to_look_at(glm::vec3(1.f, 18.f, 1.f));
        camera_3d->set_rotation(60.f);
        camera_3d->set_look_at(glm::vec3(0.f, 0.f, 0.f));
        main_menu_scene::set_camera_3d(camera_3d);

        /*gb::ui::button_shared_ptr local_session_button = m_ui_base_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_local_session, this, std::placeholders::_1));
        local_session_button->position = glm::vec2(32.f, 96.f);
        local_session_button->set_text("Idle");
        local_session_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(local_session_button);

		gb::ui::button_shared_ptr ui_editor_scene_button = m_ui_base_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_ui_editor_scene,
			this, std::placeholders::_1));
		ui_editor_scene_button->position = glm::vec2(32.f, 32.f);
		ui_editor_scene_button->set_text("Run");
		ui_editor_scene_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
		main_menu_scene::add_child(ui_editor_scene_button);
        
        gb::ui::button_shared_ptr attack_animation_button = m_ui_base_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_net_session,
                                                                                                                            this, std::placeholders::_1));
        attack_animation_button->position = glm::vec2(32.f, 160.f);
        attack_animation_button->set_text("Attack");
        attack_animation_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(attack_animation_button);
        
        const auto move_joystick = m_gameplay_ui_fabricator->create_move_joystick("");
        std::static_pointer_cast<gb::ui::joystick>(move_joystick)->set_threshold(glm::vec2(32.f));
        main_menu_scene::add_child(move_joystick);*/
        
        /*gb::ui::button_shared_ptr net_session_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_net_session,
                                                                                                                        this, std::placeholders::_1));
        net_session_button->position = glm::vec2(scene_2d_size.x * .5f - 128.f, 164.f);
        net_session_button->set_text("net session");
        net_session_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(net_session_button);
        
        
        gb::ui::button_shared_ptr goto_net_menu_scene_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_net_menu_scene,
                                                                                                                        this, std::placeholders::_1));
        goto_net_menu_scene_button->position = glm::vec2(scene_2d_size.x * .5f - 128.f, 200.f);
        goto_net_menu_scene_button->set_text("find server");
        goto_net_menu_scene_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(goto_net_menu_scene_button);*/
        
        auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        sound_component->add_sound("music_01.mp3", true);
        sound_component->trigger_sound("music_01.mp3");
        ces_entity::add_component(sound_component);

        /*m_character = m_gameplay_fabricator->create_main_character("character.human_01.xml");
        m_character->rotation = glm::vec3(0.f, -45.f, 0.f);
        m_character->position = glm::vec3(0.f, 0.f, 0.f);
        m_character->scale = glm::vec3(.01f);
        std::static_pointer_cast<gb::shape_3d>(m_character->get_component<ces_character_parts_component>()->get_body_part())->play_animation("idle");
        main_menu_scene::add_child(m_character);*/
		//const auto gameplay_configuration_accessor = m_gameplay_fabricator->get_gameplay_configuration_accessor();
        
        /*auto heightmap = main_menu_scene::get_fabricator()->create_heightmap("heightmap.village.xml");
        main_menu_scene::add_child(heightmap);*/

		// auto character_configuration = std::static_pointer_cast<gb::character_configuration>(gameplay_configuration_accessor->get_character_configuration("orc.front.3d.xml"));
        //m_character = main_menu_scene::get_fabricator()->create_shape_3d("orc.main.3d.xml");
        //m_character->position = glm::vec3(0.f, 0.f, 0.f);
        //m_character->rotation = glm::vec3(0.f, -45.f, 0.f);
        //hero->scale = glm::vec3(.01f);
        //m_character->play_animation("run", true);
        //main_menu_scene::add_child(m_character);
        
        /*auto plane = main_menu_scene::get_fabricator()->create_shape_3d("ground_dirt.xml");
        //plane->position = glm::vec3(256.f, -16.f, 256.f);
        plane->scale = glm::vec3(100.f, 100.f, 100.f);
        main_menu_scene::add_child(plane);*/
        
        //auto box = main_menu_scene::get_fabricator()->create_shape_3d("box_3d.xml");
        //box->position = glm::vec3(256.f, -16.f, 256.f);
        //main_menu_scene::add_child(box);

		/*gb::binding_shared_model::get_instance()->remove_all_submodels();
		gb::binding_shared_model::get_instance()->unregister_all_storages_and_pools();

		gb::binding_shared_model::get_instance()->add_submodel(binding_board_model::construct<binding_board_model>());

		const auto board = gb::binding_shared_model::get_instance()->get_submodel<binding_board_model>();
		board->set_cols(4);
		board->set_rows(4);

		const auto cell = binding_cell_model::construct<binding_cell_model>();
		board->add_submodel(cell);

		const auto element = binding_element_model::construct<binding_element_model>();
		cell->add_submodel(element);

		auto regular_element = binding_regular_element_model::construct<binding_regular_element_model>();
		element->add_submodel(regular_element);
		regular_element = element->get_current_sub_element_as<binding_regular_element_model>();
		element->remove_submodel(regular_element);*/
        
        const auto scene_2d = main_menu_scene::get_transition()->get_resource_accessor()->get_resource<gb::scene_2d,
        gb::scene_2d_loading_operation>("simple_scene_2d.tmx", true);
        
        const auto scene = m_gameplay_fabricator->create_scene("plane_3d.xml");
        main_menu_scene::add_child(scene);
        
        /*const auto road_corner = main_menu_scene::get_fabricator()->create_shape_3d("road_corner.xml");
        road_corner->position = glm::vec3(1.f, 0.f, 0.f);
        main_menu_scene::add_child(road_corner);
       
        const auto road_straight = main_menu_scene::get_fabricator()->create_shape_3d("road_straight.xml");
        main_menu_scene::add_child(road_straight);*/
        
    
        //const auto car = main_menu_scene::get_fabricator()->create_shape_3d("car.xml");
        //main_menu_scene::add_child(car);
        
        main_menu_scene::enable_box2d_world(glm::vec2(-256.f),
                                            glm::vec2(256.f));
        
        m_character = m_gameplay_fabricator->create_car("character.human_01.xml");
        m_character->position = glm::vec3(0.f, 0.f, 0.f);
        //m_character->rotation = glm::vec3(0.f, -45.f, 0.f);
        //m_character->scale = glm::vec3(16.f);
        main_menu_scene::add_child(m_character);
        
        /*const auto map = main_menu_scene::get_transition()->get_resource_accessor()->get_resource<gb::scene_3d,
         gb::scene_3d_loading_operation>("map_01.GB3D_SCENE", true);
        for (i32 i = 0; i < map->get_num_objects(); ++i)
        {
            const auto map_object = map->get_scene_object(i);
            auto map_object_name = map_object->get_name();
            map_object_name.resize(map_object_name.length() - 4);
            map_object_name.append(".xml");
            
            const auto position = map_object->get_position();
            const auto rotation = map_object->get_rotation();
            const auto scale = map_object->get_scale();
            
            auto map_object_instance = main_menu_scene::get_fabricator()->create_shape_3d(map_object_name);
            map_object_instance->position = position;
            map_object_instance->rotation = rotation;
            map_object_instance->scale = scale;
            main_menu_scene::add_child(map_object_instance);
        }*/
    }
    
    void main_menu_scene::on_goto_local_session(gb::ces_entity_const_shared_ptr entity)
    {
        /*if(m_external_commands)
        {
            m_external_commands->execute<on_goto_local_session::t_command>(on_goto_local_session::guid);
        }
        else
        {
            assert(false);
        }*/
        std::static_pointer_cast<gb::shape_3d>(m_character->get_component<ces_character_parts_component>()->get_body_part())->play_animation("idle");
        //m_character->play_animation("idle", true);
    }
    
    void main_menu_scene::on_goto_net_session(gb::ces_entity_const_shared_ptr entity)
    {
        /*if(m_external_commands)
        {
            m_external_commands->execute<on_goto_net_session::t_command>(on_goto_net_session::guid);
        }
        else
        {
            assert(false);
        }*/
        std::static_pointer_cast<gb::shape_3d>(m_character->get_component<ces_character_parts_component>()->get_body_part())->play_animation("run", true, { std::make_pair("attack", true) });
        //m_character->play_animation();
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
        std::static_pointer_cast<gb::shape_3d>(m_character->get_component<ces_character_parts_component>()->get_body_part())->play_animation("run", true);
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
