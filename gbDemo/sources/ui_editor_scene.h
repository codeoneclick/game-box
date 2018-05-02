#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
	class ui_editor_scene : public gb::scene_graph
	{
	private:


	protected:

		ui_editable_fabricator_shared_ptr m_ui_editable_fabricator;
		gb::camera_2d_shared_ptr m_camera_2d;

	public:

		ui_editor_scene(const gb::game_transition_shared_ptr& transition);
		~ui_editor_scene();

		void create() override;
	};
};