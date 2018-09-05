#pragma once

#if defined(VULKAN_API)

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	class vk_swap_chain
	{
	public:

		struct vk_swap_chain_support_details
		{
			VkSurfaceCapabilitiesKHR m_capabilities;
			std::vector<VkSurfaceFormatKHR> m_formats;
			std::vector<VkPresentModeKHR> m_present_modes;
		};

		struct vk_swap_chain_buffer
		{
			VkImage m_image = VK_NULL_HANDLE;
			VkImageView m_view = VK_NULL_HANDLE;
		};

	private:

		static std::shared_ptr<vk_swap_chain> m_instance;

		VkSwapchainKHR m_swap_chain = VK_NULL_HANDLE;

		std::vector<VkImage> m_images;
		std::vector<vk_swap_chain_buffer> m_buffers;

		VkFormat m_image_format;
		VkExtent2D m_extent;

		VkRenderPass m_render_pass = VK_NULL_HANDLE;
		VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;

		PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
		PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
		PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
		PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
		PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
		PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
		PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
		PFN_vkQueuePresentKHR fpQueuePresentKHR;

	protected:

		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<ogl_window>& window);

	public:

		~vk_swap_chain();

		static std::shared_ptr<vk_swap_chain> get_instance();

		void construct(VkInstance vk_instance, const std::shared_ptr<ogl_window>& window);
		void connect(VkInstance vk_instance);

		VkSwapchainKHR get() const;

		vk_swap_chain_support_details query_swap_chain_support();

		const std::vector<VkImage>& get_images() const;
		const ui32 get_images_count() const;

		VkImageView get_image_view(ui32 index) const;

		VkFormat get_swap_chain_image_format() const;
		VkExtent2D get_swap_chain_extent() const;

		VkRenderPass get_render_pass() const;
		VkPipelineLayout get_pipeline_layout() const;

		VkResult acquire_next_image(VkSemaphore present_complete_semaphore, ui32 *image_index);
		VkResult queue_present(VkQueue queue, ui32 image_index, VkSemaphore wait_semaphore = VK_NULL_HANDLE);
	};
};

#endif