
#if defined(__WIN32__) && !defined(__NO_RENDER__)

#include "input_context.h"

namespace gb
{
	class input_context_win32;
	static std::weak_ptr<input_context_win32> g_input_context;

	class input_context_win32 : public input_context
	{
	private:

		friend LRESULT CALLBACK win32_callback(HWND hwnd, UINT message, WPARAM param_w, LPARAM param_l);

	protected:

		bool m_is_tapped;

	public:

		input_context_win32(const std::shared_ptr<ogl_window>& window);
		~input_context_win32() = default;
	};

	input_context_win32::input_context_win32(const std::shared_ptr<ogl_window>& window) :
		m_is_tapped(false)
	{

	}

	LRESULT CALLBACK win32_callback(HWND hwnd, UINT message, WPARAM param_w, LPARAM param_l)
	{
		if (g_input_context.expired())
		{
			return DefWindowProc(hwnd, message, param_w, param_l);
		}

		auto input_context = g_input_context.lock();

		POINT mouse_position;
		GetCursorPos(&mouse_position);
		ScreenToClient(hwnd, &mouse_position);
		glm::ivec2 current_touch_point = glm::ivec2(mouse_position.x, mouse_position.y);

		switch (message)
		{
		case WM_ACTIVATE:
		{
			return 0;
		}

		case WM_SYSCOMMAND:
		{
			switch (param_w)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				return 0;
			}
			break;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_KEYDOWN:
		{
			return 0;
		}

		case WM_KEYUP:
		{
			return 0;
		}

		case WM_SIZE:
		{
			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			return 0;
		}
		case WM_RBUTTONUP:
		{
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			input_context->gr_pressed(current_touch_point, input_context->m_screen_size, gb::e_input_source_mouse_left, 0);
			input_context->set_previous_touch_point(current_touch_point);
			input_context->m_is_tapped = true;
			return 0;
		}
		case WM_LBUTTONUP:
		{
			input_context->gr_released(current_touch_point, input_context->m_screen_size, gb::e_input_source_mouse_left, 0);
			input_context->set_previous_touch_point(current_touch_point);
			input_context->m_is_tapped = false;
			return 0;
		}
		}

		if (input_context->m_is_tapped)
		{
			glm::ivec2 delta = current_touch_point - input_context->get_previous_touch_point();
			input_context->gr_dragged(current_touch_point, input_context->m_screen_size, delta, gb::e_input_source_mouse_left, 0);
			input_context->set_previous_touch_point(current_touch_point);
		}

		return DefWindowProc(hwnd, message, param_w, param_l);
	}

	std::shared_ptr<input_context> create_input_context_win32(const std::shared_ptr<ogl_window>& window)
	{
		auto input_context = std::make_shared<input_context_win32>(window);
		g_input_context = input_context;
		return input_context;
	};
}
#endif