#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
	class ui_editor_scene : public gb::scene_graph
	{
	private:

		void on_create_button_button_drag_started(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point);
		void on_create_button_button_dragging(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point);
		void on_create_button_button_drag_ended(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point);

		void on_create_textfield_textfield_drag_started(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point);
		void on_create_textfield_textfield_dragging(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point);
		void on_create_textfield_textfield_drag_ended(gb::ces_entity_const_shared_ptr entity, const glm::vec2& touch_point);

		gb::ui::button_shared_ptr m_active_created_button = nullptr;
		gb::ui::textfield_shared_ptr m_active_created_textfield = nullptr;

	protected:

		ui_editable_fabricator_shared_ptr m_ui_editable_fabricator;
		gb::camera_2d_shared_ptr m_camera_2d;

	public:

		ui_editor_scene(const gb::game_transition_shared_ptr& transition);
		~ui_editor_scene();

		void create() override;
	};
};