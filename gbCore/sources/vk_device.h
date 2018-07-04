#pragma once

#include "main_headers.h"

namespace gb
{
	class vk_buffer;

	class vk_device
	{
	public:

		struct vk_queue_family
		{
			i32 m_graphics_family = -1;
			i32 m_present_family = -1;

			bool is_complete() const
			{
				return m_graphics_family >= 0 && m_present_family >= 0;
			};
		};

	private:

		static std::shared_ptr<vk_device> m_instance;

		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkDevice m_logical_device = VK_NULL_HANDLE;

		VkQueue m_graphics_queue = VK_NULL_HANDLE;
		VkQueue m_present_queue = VK_NULL_HANDLE;

		VkCommandPool m_command_pool = VK_NULL_HANDLE;

		VkPhysicalDeviceMemoryProperties m_memory_properties;

		std::vector<const char*> m_device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

	protected:

	public:

		~vk_device();

		static std::shared_ptr<vk_device> get_instance();

		void construct(VkInstance vk_instance);

		VkDevice get_logical_device();
		VkPhysicalDevice get_physical_device();

		VkPhysicalDeviceMemoryProperties get_memory_properties();
		ui32 get_memory_type(ui32 type_bits, VkMemoryPropertyFlags properties, VkBool32 *mem_type_found = nullptr);

		vk_queue_family get_queue_family();

		VkQueue get_graphics_queue() const;
		VkQueue get_present_queue() const;

		VkCommandPool create_command_pool(ui32 queue_family_index, VkCommandPoolCreateFlags create_flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		VkCommandBuffer create_command_buffer(VkCommandBufferLevel level, bool begin = false);

		VkResult create_buffer(VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *memory, void *data = nullptr);
		VkResult create_buffer(VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, const std::shared_ptr<vk_buffer>& buffer, VkDeviceSize size, void *data = nullptr);

		void set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout old_image_layout, VkImageLayout new_image_layout, VkImageSubresourceRange subresource_range, VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
		void set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask, VkImageLayout old_image_layout, VkImageLayout new_image_layout, VkPipelineStageFlags src_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dst_stage_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	
		void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free = true);
	};
};