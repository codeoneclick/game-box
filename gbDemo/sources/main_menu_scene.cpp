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
#include "scene_3d.h"
#include "scene_3d_loading_operation.h"

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

		auto heightmap_assembling_system = std::make_shared<gb::ces_heightmap_assembling_system>();
		heightmap_assembling_system->set_resource_accessor(main_menu_scene::get_transition()->get_resource_accessor());
		main_menu_scene::get_transition()->add_system(heightmap_assembling_system);

		auto heightmap_lod_system = std::make_shared<gb::ces_heightmap_lod_system>();
		heightmap_lod_system->set_resource_accessor(main_menu_scene::get_transition()->get_resource_accessor());
		main_menu_scene::get_transition()->add_system(heightmap_lod_system);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(main_menu_scene::get_fabricator());
        
        m_camera_2d = std::make_shared<gb::camera_2d>(scene_2d_size.x,
			scene_2d_size.y);
        main_menu_scene::set_camera_2d(m_camera_2d);
        
        auto camera_3d = std::make_shared<gb::camera_3d>(90.f, .1f, 1024.f,
                                                         glm::ivec4(0,
                                                                    0,
                                                                    scene_2d_size.x,
                                                                    scene_2d_size.y), true);
        camera_3d->set_distance_to_look_at(glm::vec3(128.f, 32.f, 128.f));
        camera_3d->set_rotation(45.f);
        camera_3d->set_look_at(glm::vec3(256.f, 16.f, 256.f));
        main_menu_scene::set_camera_3d(camera_3d);

        gb::ui::button_shared_ptr local_session_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_local_session,
                                                                                                                          this, std::placeholders::_1));
        local_session_button->position = glm::vec2(scene_2d_size.x * .5f - 128.f, 128.f);
        local_session_button->set_text("-Start Game-");
        local_session_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(local_session_button);

		gb::ui::button_shared_ptr ui_editor_scene_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_ui_editor_scene,
			this, std::placeholders::_1));
		ui_editor_scene_button->position = glm::vec2(32.f, 32.f);
		ui_editor_scene_button->set_text("-UI Editor-");
		ui_editor_scene_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
		main_menu_scene::add_child(ui_editor_scene_button);
        
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

		m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(main_menu_scene::get_fabricator());
		m_gameplay_fabricator = std::make_shared<gameplay_fabricator>(main_menu_scene::get_fabricator(),
			m_anim_fabricator);

		//const auto gameplay_configuration_accessor = m_gameplay_fabricator->get_gameplay_configuration_accessor();
        
        /*auto heightmap = main_menu_scene::get_fabricator()->create_heightmap("heightmap.village.xml");
        main_menu_scene::add_child(heightmap);*/

		// auto character_configuration = std::static_pointer_cast<gb::character_configuration>(gameplay_configuration_accessor->get_character_configuration("orc.front.3d.xml"));
		auto hero = main_menu_scene::get_fabricator()->create_shape_3d("orc.main.3d.xml");
		hero->position = glm::vec3(256.f, -16.f, 256.f);
		//hero->scale = glm::vec3(.01f);
		hero->play_animation("idle", true);
		main_menu_scene::add_child(hero);

		gb::binding_shared_model::get_instance()->remove_all_submodels();
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
		element->remove_submodel(regular_element);
        
        main_menu_scene::get_transition()->get_resource_accessor()->get_resource<gb::scene_3d,
        gb::scene_3d_loading_operation>("main_scene.GB3D_SCENE");
    }
    
    void main_menu_scene::on_goto_local_session(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_local_session::t_command>(on_goto_local_session::guid);
        }
        else
        {
            assert(false);
        }
    }
    
    void main_menu_scene::on_goto_net_session(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_net_session::t_command>(on_goto_net_session::guid);
        }
        else
        {
            assert(false);
        }
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
		if (m_external_commands)
		{
			m_external_commands->execute<on_goto_ui_editor_scene::t_command>(on_goto_ui_editor_scene::guid);
		}
		else
		{
			assert(false);
		}
	}
}
