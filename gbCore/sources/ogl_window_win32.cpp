#if defined(__WIN32__) && !defined(__NO_RENDER__)

#include "ogl_window.h"
#include "input_context.h"

namespace gb
{
	ogl_window::ogl_window(void* hwnd)
	{
		assert(m_hwnd);
		m_size = *static_cast<glm::ivec2*>(hwnd);

		m_hwnd = 0;
		m_hdc = 0;

		WNDCLASS window_class;
		DWORD window_ex_style = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD window_style = WS_OVERLAPPEDWINDOW;
		RECT window_frame;
		window_frame.left = 0;
		window_frame.right = m_size.x;
		window_frame.top = 0;
		window_frame.bottom = m_size.y;

		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		window_class.lpfnWndProc = (WNDPROC)win32_callback;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = GetModuleHandle(NULL);
		window_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hbrBackground = NULL;
		window_class.lpszMenuName = NULL;
		window_class.lpszClassName = L"game-box";

		if (!RegisterClass(&window_class))
		{
			MessageBox(NULL, L"Failed To Register The Window Class.", L"game-box", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		AdjustWindowRectEx(&window_frame, window_style, false, window_ex_style);
		if (!(m_hwnd = CreateWindowEx(window_ex_style,
			L"game-box",
			L"game-box",
			window_style |
			WS_CLIPSIBLINGS |
			WS_CLIPCHILDREN,
			0, 0,
			window_frame.right - window_frame.left,
			window_frame.bottom - window_frame.top,
			NULL,
			NULL,
			GetModuleHandle(NULL),
			NULL)))
		{
			MessageBox(NULL, L"Window Creation Error.", L"game-box", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (!(m_hdc = GetDC(m_hwnd)))
		{
			MessageBox(NULL, L"Can't Create GL Device Context.", L"game-box", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		static PIXELFORMATDESCRIPTOR pixel_format_descriptor =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			16,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			16,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		GLuint pixel_format = 0;
		if (!(pixel_format = ChoosePixelFormat(m_hdc, &pixel_format_descriptor)))
		{
			MessageBox(NULL, L"Can't ChoosePixelFormat.", L"game-box", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (!SetPixelFormat(m_hdc, pixel_format, &pixel_format_descriptor))
		{
			MessageBox(NULL, L"Can't SetPixelFormat.", L"game-box", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		ShowWindow(m_hwnd, SW_SHOW);
		UpdateWindow(m_hwnd);
	}

	ogl_window::~ogl_window()
	{

	}

	HWND ogl_window::get_hwnd() const
	{
		return m_hwnd;
	}

	HDC ogl_window::get_hdc() const
	{
		return m_hdc;
	}

	ui32 ogl_window::get_width() const
	{
		return m_size.x;
	}

	ui32 ogl_window::get_height() const
	{
		return m_size.y;
	}
}

#endif
