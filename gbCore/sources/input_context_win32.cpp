
#if defined(__WINOS__) && !defined(__NO_RENDER__)

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

        bool m_is_left_tapped;
        bool m_is_right_tapped;

	public:

		input_context_win32(const std::shared_ptr<window_impl>& window);
		~input_context_win32() = default;
	};

	input_context_win32::input_context_win32(const std::shared_ptr<window_impl>& window) :
        m_is_left_tapped(false),
        m_is_right_tapped(false)
	{

	}

	LRESULT CALLBACK win32_callback(HWND hwnd, UINT message, WPARAM param_w, LPARAM param_l)
	{
		if (g_input_context.expired())
		{
			return DefWindowProc(hwnd, message, param_w, param_l);
		}

		auto input_context = g_input_context.lock();

        // Mouse messages carry client coordinates, including negative values
        // while captured outside the window. Do not poll a newer cursor position.
        glm::ivec2 current_touch_point(static_cast<short>(LOWORD(param_l)),
                                      static_cast<short>(HIWORD(param_l)));
        const auto cancel_drag = [&]() {
            const auto point = input_context->get_previous_touch_point();
            if (input_context->m_is_left_tapped)
                input_context->gr_released(point, input_context->get_touch_area_size(), e_input_source_mouse_left, 0);
            if (input_context->m_is_right_tapped)
                input_context->gr_released(point, input_context->get_touch_area_size(), e_input_source_mouse_right, 0);
            input_context->m_is_left_tapped = false;
            input_context->m_is_right_tapped = false;
        };

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
			input_context->key_down(param_w);
			return 0;
		}

		case WM_KEYUP:
		{
			input_context->key_up(param_w);
			return 0;
		}

		case WM_CHAR:
		{
			switch (param_w)
			{
			case 0x08:  // backspace 
			case 0x0A:  // linefeed 
			case 0x1B:  // escape 
			case 0x09:  // tab 
			case 0x0D:  // carriage return 
				return 0;
			default: 
				return 0;
			}
			return 0;
		}

		case WM_SIZE:
		{
			return 0;
		}

        case WM_RBUTTONDOWN:
        {
            input_context->m_is_right_tapped = true;
            input_context->gr_pressed(current_touch_point, input_context->get_touch_area_size(), e_input_source_mouse_right, 0);
            input_context->set_previous_touch_point(current_touch_point);
            SetCapture(hwnd);
            return 0;
		}
        case WM_RBUTTONUP:
        {
            input_context->m_is_right_tapped = false;
            input_context->gr_released(current_touch_point, input_context->get_touch_area_size(), e_input_source_mouse_right, 0);
            input_context->set_previous_touch_point(current_touch_point);
            if (!input_context->m_is_left_tapped && GetCapture() == hwnd) ReleaseCapture();
            return 0;
		}

		case WM_LBUTTONDOWN:
		{
			input_context->gr_pressed(current_touch_point, input_context->get_touch_area_size(), gb::e_input_source_mouse_left, 0);
			input_context->set_previous_touch_point(current_touch_point);
            input_context->m_is_left_tapped = true;
            SetCapture(hwnd);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			input_context->gr_released(current_touch_point, input_context->get_touch_area_size(), gb::e_input_source_mouse_left, 0);
			input_context->set_previous_touch_point(current_touch_point);
            input_context->m_is_left_tapped = false;
            if (!input_context->m_is_right_tapped && GetCapture() == hwnd) ReleaseCapture();
            return 0;
        }
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
        case WM_KILLFOCUS:
        {
            cancel_drag();
            if (message != WM_CAPTURECHANGED && GetCapture() == hwnd) ReleaseCapture();
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            // Unlike move/button messages, wheel coordinates are screen-relative.
            POINT point={current_touch_point.x,current_touch_point.y};
            ScreenToClient(hwnd,&point);
            const float delta=static_cast<short>(HIWORD(param_w))/static_cast<float>(WHEEL_DELTA);
            input_context->gr_scrolled(glm::ivec2(point.x,point.y),input_context->get_touch_area_size(),delta);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            glm::ivec2 delta = current_touch_point - input_context->get_previous_touch_point();
            if (input_context->m_is_left_tapped)
                input_context->gr_dragged(current_touch_point, input_context->get_touch_area_size(), delta, e_input_source_mouse_left, 0);
            if (input_context->m_is_right_tapped)
                input_context->gr_dragged(current_touch_point, input_context->get_touch_area_size(), delta, e_input_source_mouse_right, 0);
            input_context->set_previous_touch_point(current_touch_point);
            break;
        }
        }

		return DefWindowProc(hwnd, message, param_w, param_l);
	}

	std::shared_ptr<input_context> create_input_context_win32(const std::shared_ptr<window_impl>& window)
	{
		auto input_context = std::make_shared<input_context_win32>(window);
		g_input_context = input_context;
		return input_context;
	};
}
#endif
