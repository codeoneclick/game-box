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

		ui_editor_transition(const std::string& guid, bool is_offscreen);
		~ui_editor_transition();
	};
};
