#pragma once

#if defined(VULKAN_API)

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	class vk_surface
	{
	private:

		static std::shared_ptr<vk_surface> m_instance;

		VkSurfaceKHR m_vk_surface = VK_NULL_HANDLE;

	protected:

	public:

		~vk_surface();

		static std::shared_ptr<vk_surface> get_instance();

		void construct(VkInstance vk_instance, const std::shared_ptr<ogl_window>& window);

		VkSurfaceKHR get() const;
	};
};

#endif