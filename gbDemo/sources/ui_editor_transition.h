#pragma once

#include "game_transition.h"
#include "ns_declarations.h"

namespace game
{
	class ui_editor_transition : public gb::game_transition
	{
	private:

	protected:

		void create_scene();
		void destroy_scene();

	public:

		ui_editor_transition(const std::string& guid, const gb::window_impl_shared_ptr& window, bool is_offscreen);
		~ui_editor_transition();
	};
};
