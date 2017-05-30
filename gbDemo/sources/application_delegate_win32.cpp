#include "application_delegate_win32.h"

namespace game
{
	application_delegate_win32::application_delegate_win32()
	{
		m_game_controller = std::make_shared<game_conroller_win32>();
	}

	application_delegate_win32::~application_delegate_win32()
	{

	}
}