#pragma once
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

	private:

		static std::shared_ptr<vk_swap_chain> m_instance;

		VkSwapchainKHR m_vk_swap_chain = VK_NULL_HANDLE;

		std::vector<VkImage> m_vk_swap_chain_images;
		VkFormat m_vk_swap_chain_image_format;
		VkExtent2D m_vk_swap_chain_extent;

	protected:

		VkSurfaceFormatKHR choose_vk_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR choose_vk_swap_present_mode(const std::vector<VkPresentModeKHR> present_modes);
		VkExtent2D choose_vk_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, const std::shared_ptr<ogl_window>& window);

	public:

		~vk_swap_chain();

		static std::shared_ptr<vk_swap_chain> get_instance();

		void construct(VkInstance vk_instance, const std::shared_ptr<ogl_window>& window);

		VkSwapchainKHR get() const;

		vk_swap_chain_support_details query_vk_swap_chain_support();

		const std::vector<VkImage>& get_vk_swap_chain_images() const;
		VkFormat get_vk_swap_chain_image_format() const;
		VkExtent2D get_vk_swap_chain_extent() const;
	};
};