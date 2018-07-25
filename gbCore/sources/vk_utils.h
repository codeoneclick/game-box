#include "main_headers.h"
#include "declarations.h"

namespace gb
{
	class vk_buffer;
	class vk_utils
	{
	public:

		static VkResult create_buffer(VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *memory, void *data = nullptr);
		static VkResult create_buffer(VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, const std::shared_ptr<vk_buffer>& buffer, VkDeviceSize size, void *data = nullptr);
	
		static VkCommandBuffer create_command_buffer(VkCommandBufferLevel level, bool begin = false);
		static void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free = true);

		static void copy_buffers(const std::shared_ptr<vk_buffer>& buffer_src, const std::shared_ptr<vk_buffer>& buffer_dst);
	};
};
