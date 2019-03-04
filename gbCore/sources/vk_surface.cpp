#include "vk_surface.h"

#if USED_GRAPHICS_API == VULKAN_API

#include "window_impl.h"

namespace gb
{
	std::shared_ptr<vk_surface> vk_surface::m_instance = nullptr;

	vk_surface::~vk_surface()
	{

	}

	std::shared_ptr<vk_surface> vk_surface::get_instance()
	{
		if (!m_instance)
		{
			m_instance = std::make_shared<vk_surface>();
		}
		return m_instance;
	}

	void vk_surface::construct(VkInstance vk_instance, const std::shared_ptr<window_impl>& window)
	{
		VkWin32SurfaceCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		create_info.hwnd = window->get_hwnd();
		create_info.hinstance = GetModuleHandle(nullptr);

		auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(vk_instance, "vkCreateWin32SurfaceKHR");
		if (CreateWin32SurfaceKHR)
		{
			VkResult result = CreateWin32SurfaceKHR(vk_instance, &create_info, nullptr, &m_vk_surface);
			assert(result == VK_SUCCESS);
		}
		else
		{
			assert(false);
		}
	}

	VkSurfaceKHR vk_surface::get() const
	{
		return m_vk_surface;
	}
}

#endif
