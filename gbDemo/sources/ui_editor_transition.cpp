#include "ui_editor_transition.h"
#include "ui_editor_scene.h"
#include "scene_fabricator.h"

namespace game
{
	ui_editor_transition::ui_editor_transition(const std::string& guid, bool is_offscreen) :
		game_transition(guid, is_offscreen)
	{

	}

	ui_editor_transition::~ui_editor_transition(void)
	{

	}

	void ui_editor_transition::create_scene()
	{
		m_scene = gb::ces_entity::construct<game::ui_editor_scene>(shared_from_this());
		gb::scene_fabricator_shared_ptr fabricator = std::make_shared<gb::scene_fabricator>();
		ui_editor_transition::set_fabricator(fabricator);
	}

	void ui_editor_transition::destroy_scene()
	{
		m_scene = nullptr;
	}
}
