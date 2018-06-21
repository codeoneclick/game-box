#pragma once
#include "textfield.h"

namespace game
{
	class editable_textfield : public gb::ui::textfield
	{
	private:

		glm::vec2 m_touch_offset = glm::vec2(0.f);
		glm::vec2 m_previous_resize_point = glm::vec2(-1.f);
		bool m_is_resizing = false;
		std::string k_textfield_resize_element_name = "ui_textfield_resize_element";
		glm::vec2 k_button_min_size = glm::vec2(16.f);

	protected:

		void on_touched(const gb::ces_entity_shared_ptr&,
						const glm::vec2& touch_point,
						gb::e_input_source input_source,
						gb::e_input_state input_state);

		void on_dragging(const gb::ces_entity_shared_ptr&,
						 const glm::vec2& touch_point,
						 gb::e_input_source input_source,
						 gb::e_input_state input_state);

	public:

		editable_textfield(const gb::scene_fabricator_shared_ptr& fabricator);
		~editable_textfield();

		void create() override;
	};
};