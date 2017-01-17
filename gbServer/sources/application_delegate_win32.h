#pragma once

#include "main_headers.h"
#include "game_conroller_win32.h"

namespace ns
{
	class application_delegate_win32
	{
	private:

	protected:

	std::shared_ptr<game_conroller_win32> m_game_controller;

	public:
		application_delegate_win32();
		~application_delegate_win32();
	};
};