#include "ui_editor_scene.h"
#include "ui_editable_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera_2d.h"
#include "editable_button.h"
#include "ces_sound_system.h"
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

		m_ui_editable_fabricator = std::make_shared<ui_editable_fabricator>(ui_editor_scene::get_fabricator());

		m_camera_2d = std::make_shared<gb::camera_2d>(scene_2d_size.x,
													  scene_2d_size.y);
		ui_editor_scene::set_camera_2d(m_camera_2d);

		gb::ui::button_shared_ptr local_session_button = m_ui_editable_fabricator->create_editable_button(glm::vec2(256.f, 32.f));
		local_session_button->position = glm::vec2(32.f, 32.f);
		local_session_button->set_text("Button");
		local_session_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
		ui_editor_scene::add_child(local_session_button);
	}
}