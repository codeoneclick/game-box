#pragma once

#include "button.h"

namespace game
{
	class editable_button : public gb::ui::button 
	{
	private:

	protected:

	public:

		editable_button(const gb::scene_fabricator_shared_ptr& fabricator);
		~editable_button();
	};
};