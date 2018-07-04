#include "vk_swap_chain.h"
#include "ogl_window.h"
#include "vk_device.h"
#include "vk_surface.h"

namespace gb
{
	std::shared_ptr<vk_swap_chain> vk_swap_chain::m_instance = nullptr;

	vk_swap_chain::~vk_swap_chain()
	{

	}

	std::shared_ptr<vk_swap_chain> vk_swap_chain::get_instance()
	{
		if (!m_instance)
		{
			m_instance = std::make_shared<vk_swap_chain>();
		}
		return m_instance;
	}

	void vk_swap_chain::construct(VkInstance vk_instance, const std::shared_ptr<ogl_window>& window)
	{
		vk_swap_chain_support_details swap_chain_support = query_vk_swap_chain_support();

		VkSurfaceFormatKHR surface_format = choose_vk_swap_surface_format(swap_chain_support.m_formats);
		VkPresentModeKHR present_mode = choose_vk_swap_present_mode(swap_chain_support.m_present_modes);
		VkExtent2D extent = choose_vk_swap_extent(swap_chain_support.m_capabilities, window);

		uint32_t images_count = swap_chain_support.m_capabilities.minImageCount + 1;
		if (swap_chain_support.m_capabilities.maxImageCount > 0 && images_count > swap_chain_support.m_capabilities.maxImageCount)
		{
			images_count = swap_chain_support.m_capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = vk_surface::get_instance()->get();

		create_info.minImageCount = images_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = extent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		vk_device::vk_queue_family queue_family = vk_device::get_instance()->get_queue_family();

		if (queue_family.m_graphics_family != queue_family.m_present_family)
		{
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			ui32 queue_families[] = { (ui32)queue_family.m_graphics_family, (ui32)queue_family.m_present_family };
			create_info.pQueueFamilyIndices = queue_families;
		}
		else
		{
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 1;
			ui32 queue_families[] = { (ui32)queue_family.m_graphics_family };
			create_info.pQueueFamilyIndices = queue_families;
		}

		create_info.preTransform = swap_chain_support.m_capabilities.currentTransform;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;

		create_info.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(vk_device::get_instance()->get_logical_device(), &create_info, nullptr, &m_vk_swap_chain);
		assert(result == VK_SUCCESS);

		vkGetSwapchainImagesKHR(vk_device::get_instance()->get_logical_device(), m_vk_swap_chain, &images_count, nullptr);
		m_vk_swap_chain_images.resize(images_count);
		vkGetSwapchainImagesKHR(vk_device::get_instance()->get_logical_device(), m_vk_swap_chain, &images_count, m_vk_swap_chain_images.data());

		m_vk_swap_chain_image_format = surface_format.format;
		m_vk_swap_chain_extent = extent;
	}

	VkSwapchainKHR vk_swap_chain::get() const
	{
		return m_vk_swap_chain;
	}

	vk_swap_chain::vk_swap_chain_support_details vk_swap_chain::query_vk_swap_chain_support()
	{
		vk_swap_chain_support_details details;
		auto surface = vk_surface::get_instance()->get();

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_device::get_instance()->get_physical_device(), surface, &details.m_capabilities);

		ui32 formats_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device::get_instance()->get_physical_device(), surface, &formats_count, nullptr);

		if (formats_count != 0) 
		{
			details.m_formats.resize(formats_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device::get_instance()->get_physical_device(), surface, &formats_count, details.m_formats.data());
		}

		ui32 present_modes_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device::get_instance()->get_physical_device(), surface, &present_modes_count, nullptr);

		if (present_modes_count != 0)
		{
			details.m_present_modes.resize(present_modes_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device::get_instance()->get_physical_device(), surface, &present_modes_count, details.m_present_modes.data());
		}

		return details;
	}

	VkSurfaceFormatKHR vk_swap_chain::choose_vk_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& format : formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return formats[0];
	}

	VkPresentModeKHR vk_swap_chain::choose_vk_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes)
	{
		VkPresentModeKHR mode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& present_mode : present_modes) {
			if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return present_mode;
			}
			else if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				mode = present_mode;
			}
		}

		return mode;
	}

	VkExtent2D vk_swap_chain::choose_vk_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<ogl_window>& window)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actual_extent = { window->get_width(), window->get_height() };

			actual_extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
			actual_extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));

			return actual_extent;
		}
	}

	const std::vector<VkImage>& vk_swap_chain::get_vk_swap_chain_images() const
	{
		return m_vk_swap_chain_images;
	}

	VkFormat vk_swap_chain::get_vk_swap_chain_image_format() const
	{
		return m_vk_swap_chain_image_format;
	}

	VkExtent2D vk_swap_chain::get_vk_swap_chain_extent() const
	{
		return m_vk_swap_chain_extent;
	}
}