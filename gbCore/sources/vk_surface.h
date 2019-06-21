#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == VULKAN_API

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

		void construct(VkInstance vk_instance, const std::shared_ptr<window_impl>& window);

		VkSurfaceKHR get() const;
	};
};

#endif
