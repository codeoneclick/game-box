#include "ui_editable_fabricator.h"
#include "editable_button.h"

namespace game
{
	ui_editable_fabricator::ui_editable_fabricator(const gb::scene_fabricator_shared_ptr& fabricator) : gb::ui::ui_fabricator(fabricator)
	{

	}

	ui_editable_fabricator::~ui_editable_fabricator()
	{

	}

	editable_button_shared_ptr ui_editable_fabricator::create_editable_button(const glm::vec2& size) {
		auto button = gb::ces_entity::construct<editable_button>(m_fabricator);
		button->create();
		button->size = size;
		button->set_on_pressed_callback(nullptr);
		return button;
	}
}