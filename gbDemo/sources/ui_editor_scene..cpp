#include "ui_editor_scene.h"
#include "ui_editable_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera_2d.h"
#include "editable_textfield.h"
#include "editable_button.h"
#include "panel.h"
#include "ces_sound_system.h"
#include "ces_textedit_system.h"
#include "ces_sound_component.h"

namespace game
{
	ui_editor_scene::ui_editor_scene(const gb::game_transition_shared_ptr& transition) :
		gb::scene_graph(transition)
	{

	}

	ui_editor_scene::~ui_editor_scene()
	{

	}

	void ui_editor_scene::create()
	{
		gb::scene_graph::create();

		glm::vec2 scene_2d_size = glm::vec2(667, 375);

		auto sound_system = std::make_shared<gb::al::ces_sound_system>();
		ui_editor_scene::get_transition()->add_system(sound_system);

		auto textedit_system = std::make_shared<gb::ui::ces_textedit_system>();
		ui_editor_scene::get_transition()->get_input_context()->add_listener(textedit_system);
		ui_editor_scene::get_transition()->add_system(textedit_system);

		m_ui_editable_fabricator = std::make_shared<ui_editable_fabricator>(ui_editor_scene::get_fabricator());

		m_camera_2d = std::make_shared<gb::camera_2d>(scene_2d_size.x,
													  scene_2d_size.y);
		ui_editor_scene::set_camera_2d(m_camera_2d);

		auto ui_elements_panel = m_ui_editable_fabricator->create_panel(glm::vec2(192.f, 512.f), "Elements");
		ui_elements_panel->position = glm::vec2(8.f, 8.f);
		ui_editor_scene::add_child(ui_elements_panel);

		auto create_button_button = m_ui_editable_fabricator->create_button(glm::vec2(128.f, 32.f), nullptr);
		create_button_button->position = glm::vec2(16.f, 24.f);
		create_button_button->set_text("New Button");
		create_button_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
		create_button_button->set_on_drag_started_callback(std::bind(&ui_editor_scene::on_create_button_button_drag_started, this, std::placeholders::_1, std::placeholders::_2));
		create_button_button->set_on_dragging_callback(std::bind(&ui_editor_scene::on_create_button_button_dragging, this, std::placeholders::_1, std::placeholders::_2));
		create_button_button->set_on_drag_ended_callback(std::bind(&ui_editor_scene::on_create_button_button_drag_ended, this, std::placeholders::_1, std::placeholders::_2));
		ui_elements_panel->add_child(create_button_button);

		auto create_textfield_textfield = m_ui_editable_fabricator->create_textfield(glm::vec2(128.f, 32.f), "New Textfield");
		create_textfield_textfield->position = glm::vec2(16.f, 72.f);
		create_textfield_textfield->set_on_drag_started_callback(std::bind(&ui_editor_scene::on_create_textfield_textfield_drag_started, this, std::placeholders::_1, std::placeholders::_2));
		create_textfield_textfield->set_on_dragging_callback(std::bind(&ui_editor_scene::on_create_textfield_textfield_dragging, this, std::placeholders::_1, std::placeholders::_2));
		create_textfield_textfield->set_on_drag_ended_callback(std::bind(&ui_editor_scene::on_create_textfield_textfield_drag_ended, this, std::placeholders::_1, std::placeholders::_2));
		ui_elements_panel->add_child(create_textfield_textfield);
	}

	void ui_editor_scene::on_create_button_button_drag_started(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point)
	{
		auto new_button = m_ui_editable_fabricator->create_editable_button(glm::vec2(128.f, 32.f));
		new_button->position = glm::vec2(touch_point.x - 64.f, touch_point.y - 16.f);
		new_button->set_text("Button");
		ui_editor_scene::add_child(new_button);
		m_active_created_button = new_button;
	}

	void ui_editor_scene::on_create_button_button_dragging(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point)
	{
		if (m_active_created_button) 
		{
			m_active_created_button->position = glm::vec2(touch_point.x - 64.f, touch_point.y - 16.f);
		}
	}

	void ui_editor_scene::on_create_button_button_drag_ended(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point)
	{
		m_active_created_button = nullptr;
	}

	void ui_editor_scene::on_create_textfield_textfield_drag_started(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point)
	{
		auto new_textfield = m_ui_editable_fabricator->create_editable_textfield(glm::vec2(128.f, 32.f));
		new_textfield->position = glm::vec2(touch_point.x - 64.f, touch_point.y - 16.f);
		new_textfield->set_text("Textfield");
		new_textfield->set_editable(false);
		ui_editor_scene::add_child(new_textfield);
		m_active_created_textfield = new_textfield;
	}

	void ui_editor_scene::on_create_textfield_textfield_dragging(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point)
	{
		if (m_active_created_textfield)
		{
			m_active_created_textfield->position = glm::vec2(touch_point.x - 64.f, touch_point.y - 16.f);
		}
	}

	void ui_editor_scene::on_create_textfield_textfield_drag_ended(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point)
	{
		m_active_created_textfield = nullptr;
	}
}