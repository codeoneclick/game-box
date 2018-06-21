#pragma once

#include "ui_fabricator.h"
#include "ns_declarations.h"

namespace game 
{
	class ui_editable_fabricator : public gb::ui::ui_fabricator 
	{
	private:

	protected:

	public:

		ui_editable_fabricator(const gb::scene_fabricator_shared_ptr& fabricator);
		~ui_editable_fabricator();

		editable_button_shared_ptr create_editable_button(const glm::vec2& size);
		editable_textfield_shared_ptr create_editable_textfield(const glm::vec2& size);
	};
};