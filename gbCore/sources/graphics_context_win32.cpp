#include "graphics_context.h"
#include "ogl_window.h"

#if defined(__WIN32__)

namespace gb
{
	class graphics_context_win32 : public graphics_context
	{
	private:

	protected:

		HDC m_hdc;

	public:

		graphics_context_win32(const HWND& hwnd, const HDC& hdc);
		~graphics_context_win32();

		void* get_context() const;

		void make_current();
		void draw() const;
	};

	std::shared_ptr<graphics_context> create_graphics_context_win32(const std::shared_ptr<ogl_window>& window)
	{
		assert(window != nullptr);
		return std::make_shared<graphics_context_win32>(window->get_hwnd(), window->get_hdc());
	};

	graphics_context_win32::graphics_context_win32(const HWND& hwnd, const HDC& hdc)
	{
		m_hdc = hdc;
		HGLRC hrc = nullptr;
		if (!(hrc = wglCreateContext(m_hdc)))
		{
			MessageBox(0, "wglCreateContext() failed.", "gb", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (!wglMakeCurrent(m_hdc, hrc))
		{
			MessageBox(0, "wglMakeCurrent() failed.", "gb", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		i32 binded_frame_buffer_handle = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binded_frame_buffer_handle);
		m_frame_buffer = binded_frame_buffer_handle;

		i32 binded_render_buffer_handle = 0;
		glGetIntegerv(GL_RENDERBUFFER_BINDING, &binded_render_buffer_handle);
		m_render_buffer = binded_render_buffer_handle;
	}

	graphics_context_win32::~graphics_context_win32()
	{

	}

	void* graphics_context_win32::get_context() const
	{
		return nullptr;
	}

	void graphics_context_win32::make_current()
	{

	}

	void graphics_context_win32::draw() const
	{
		SwapBuffers(m_hdc);
	}
}

#endif