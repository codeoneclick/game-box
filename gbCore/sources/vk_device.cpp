#include "vk_device.h"
#include "vk_surface.h"
#include "vk_initializers.h"
#include "vk_buffer.h"

#define VK_FLAGS_NONE 0
#define DEFAULT_FENCE_TIMEOUT 100000000000

namespace gb
{
	std::shared_ptr<vk_device> vk_device::m_instance = nullptr;

	std::shared_ptr<vk_device> vk_device::get_instance()
	{
		if (!m_instance)
		{
			m_instance = std::make_shared<vk_device>();
		}
		return m_instance;
	}

	vk_device::~vk_device()
	{
		if (m_command_pool)
		{
			vkDestroyCommandPool(m_logical_device, m_command_pool, nullptr);
		}
	}

	void vk_device::construct(VkInstance vk_instance)
	{
		uint32_t devices_count = 0;
		vkEnumeratePhysicalDevices(vk_instance, &devices_count, nullptr);
		assert(devices_count != 0);

		std::vector<VkPhysicalDevice> devices(devices_count);
		vkEnumeratePhysicalDevices(vk_instance, &devices_count, devices.data());

		VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;

		for (const auto& device : devices) {
			vkGetPhysicalDeviceProperties(device, &device_properties);
			vkGetPhysicalDeviceFeatures(device, &device_features);

			if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_features.geometryShader) {
				m_physical_device = device;
				break;
			}
		}
		vkGetPhysicalDeviceMemoryProperties(m_physical_device, &m_memory_properties);

		vk_queue_family queue_family = get_queue_family();

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<int> unique_queue_families = { queue_family.m_graphics_family, queue_family.m_present_family };

		float queue_priority = 1.0f;
		for (int queueFamily : unique_queue_families) {
			VkDeviceQueueCreateInfo queue_create_info = {};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queueFamily;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = static_cast<ui32>(queue_create_infos.size());
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.pEnabledFeatures = &device_features;

		create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
		create_info.ppEnabledExtensionNames = m_device_extensions.data();

		VkResult result = vkCreateDevice(m_physical_device, &create_info, nullptr, &m_logical_device);
		assert(result == VK_SUCCESS);

		if (result == VK_SUCCESS)
		{
			m_command_pool = create_command_pool(queue_family.m_graphics_family);
		}

		vkGetDeviceQueue(m_logical_device, queue_family.m_graphics_family, 0, &m_graphics_queue);
		vkGetDeviceQueue(m_logical_device, queue_family.m_present_family, 0, &m_present_queue);
	}

	vk_device::vk_queue_family vk_device::get_queue_family()
	{
		vk_queue_family result;
		ui32 queue_familie_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_familie_count, nullptr);

		std::vector<VkQueueFamilyProperties> queue_families(queue_familie_count);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queue_familie_count, queue_families.data());

		i32 index = 0;
		for (const auto& queue_family : queue_families)
		{
			if (queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				result.m_graphics_family = index;
			}

			auto surface = vk_surface::get_instance()->get();
			if (surface)
			{
				VkBool32 present_support = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, index, surface, &present_support);

				if (queue_family.queueCount > 0 && present_support)
				{
					result.m_present_family = index;
				}
			}
			else
			{
				assert(false);
			}

			if (result.is_complete())
			{
				break;
			}
			index++;
		}
		return result;
	}

	VkDevice vk_device::get_logical_device()
	{
		return m_logical_device;
	}

	VkPhysicalDevice vk_device::get_physical_device()
	{
		return m_physical_device;
	}

	VkPhysicalDeviceMemoryProperties vk_device::get_memory_properties()
	{
		return m_memory_properties;
	}

	VkCommandPool vk_device::create_command_pool(ui32 queue_family_index, VkCommandPoolCreateFlags create_flags)
	{
		VkCommandPoolCreateInfo cmd_pool_info = {};
		cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmd_pool_info.queueFamilyIndex = queue_family_index;
		cmd_pool_info.flags = create_flags;
		VkCommandPool cmd_pool;
		VkResult result = vkCreateCommandPool(m_logical_device, &cmd_pool_info, nullptr, &cmd_pool);
		assert(result == VK_SUCCESS);
		return cmd_pool;
	}

	VkCommandBuffer vk_device::create_command_buffer(VkCommandBufferLevel level, bool begin)
	{
		VkCommandBufferAllocateInfo cmd_buffer_allocate_info = vk_initializers::command_buffer_allocate_info(m_command_pool, level, 1);

		VkCommandBuffer cmd_buffer;
		VkResult result = vkAllocateCommandBuffers(m_logical_device, &cmd_buffer_allocate_info, &cmd_buffer);
		assert(result == VK_SUCCESS);

		if (begin)
		{
			VkCommandBufferBeginInfo cmd_buffer_info = vk_initializers::command_buffer_begin_info();
			result = vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_info);
			assert(result == VK_SUCCESS);
		}

		return cmd_buffer;
	}

	ui32 vk_device::get_memory_type(ui32 type_bits, VkMemoryPropertyFlags properties, VkBool32* mem_type_found)
	{
		for (uint32_t i = 0; i < m_memory_properties.memoryTypeCount; i++)
		{
			if ((type_bits & 1) == 1)
			{
				if ((m_memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					if (mem_type_found)
					{
						*mem_type_found = true;
					}
					return i;
				}
			}
			type_bits >>= 1;
		}

		if (mem_type_found)
		{
			*mem_type_found = false;
			return 0;
		}
		else
		{
			assert(false && "Could not find a matching memory type");
		}
	}

	void vk_device::set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout old_image_layout, VkImageLayout new_image_layout, VkImageSubresourceRange subresource_range, VkPipelineStageFlags src_stage_mask, VkPipelineStageFlags dst_stage_mask)
	{
		VkImageMemoryBarrier image_memory_barrier = vk_initializers::image_memory_barrier();
		image_memory_barrier.oldLayout = old_image_layout;
		image_memory_barrier.newLayout = new_image_layout;
		image_memory_barrier.image = image;
		image_memory_barrier.subresourceRange = subresource_range;

		switch (old_image_layout)
		{
		case VK_IMAGE_LAYOUT_UNDEFINED:
			image_memory_barrier.srcAccessMask = 0;
			break;

		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:
		
			break;
		}

		switch (new_image_layout)
		{
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			break;

		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
			image_memory_barrier.dstAccessMask = image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			break;

		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			if (image_memory_barrier.srcAccessMask == 0)
			{
				image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
			}
			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			break;
		default:

			break;
		}

		vkCmdPipelineBarrier(cmd_buffer, src_stage_mask, dst_stage_mask, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
	}

	void vk_device::set_image_layout(VkCommandBuffer cmd_buffer, VkImage image, VkImageAspectFlags aspect_mask, VkImageLayout old_image_layout, VkImageLayout new_image_layout, VkPipelineStageFlags src_stage_mask, VkPipelineStageFlags dst_stage_mask)
	{
		VkImageSubresourceRange subresource_range = {};
		subresource_range.aspectMask = aspect_mask;
		subresource_range.baseMipLevel = 0;
		subresource_range.levelCount = 1;
		subresource_range.layerCount = 1;
		set_image_layout(cmd_buffer, image, old_image_layout, new_image_layout, subresource_range, src_stage_mask, dst_stage_mask);
	}

	void vk_device::flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free)
	{
		if (command_buffer == VK_NULL_HANDLE)
		{
			return;
		}

		VkResult result = vkEndCommandBuffer(command_buffer);
		assert(result == VK_SUCCESS);

		VkSubmitInfo submit_info = vk_initializers::submit_info();
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		VkFenceCreateInfo fence_info = vk_initializers::fence_create_info(VK_FLAGS_NONE);
		VkFence fence;
		result = vkCreateFence(m_logical_device, &fence_info, nullptr, &fence);
		assert(result == VK_SUCCESS);

		result = vkQueueSubmit(queue, 1, &submit_info, fence);
		assert(result == VK_SUCCESS);

		result = vkWaitForFences(m_logical_device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
		assert(result == VK_SUCCESS);

		vkDestroyFence(m_logical_device, fence, nullptr);

		if (free)
		{
			vkFreeCommandBuffers(m_logical_device, m_command_pool, 1, &command_buffer);
		}
	}

	VkResult vk_device::create_buffer(VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, VkDeviceSize size, VkBuffer *buffer, VkDeviceMemory *memory, void *data)
	{
		VkBufferCreateInfo buffer_create_info = vk_initializers::buffer_create_info(usage_flags, size);
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkResult result = vkCreateBuffer(m_logical_device, &buffer_create_info, nullptr, buffer);
		assert(result == VK_SUCCESS);

		VkMemoryRequirements mem_requirements;
		VkMemoryAllocateInfo mem_alloc = vk_initializers::memory_allocate_info();
		vkGetBufferMemoryRequirements(m_logical_device, *buffer, &mem_requirements);
		mem_alloc.allocationSize = mem_requirements.size;
		mem_alloc.memoryTypeIndex = get_memory_type(mem_requirements.memoryTypeBits, memory_property_flags);
		result = vkAllocateMemory(m_logical_device, &mem_alloc, nullptr, memory);
		assert(result == VK_SUCCESS);

		if (data != nullptr)
		{
			void *mapped;
			result = vkMapMemory(m_logical_device, *memory, 0, size, 0, &mapped);
			assert(result == VK_SUCCESS);
			memcpy(mapped, data, size);
			if ((memory_property_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
			{
				VkMappedMemoryRange mapped_range = vk_initializers::mapped_memory_range();
				mapped_range.memory = *memory;
				mapped_range.offset = 0;
				mapped_range.size = size;
				vkFlushMappedMemoryRanges(m_logical_device, 1, &mapped_range);
			}
			vkUnmapMemory(m_logical_device, *memory);
		}

		result = vkBindBufferMemory(m_logical_device, *buffer, *memory, 0);
		assert(result == VK_SUCCESS);

		return result;
	}

	VkResult vk_device::create_buffer(VkBufferUsageFlags usage_flags, VkMemoryPropertyFlags memory_property_flags, const std::shared_ptr<vk_buffer>& buffer, VkDeviceSize size, void *data)
	{
		VkBufferCreateInfo buffer_create_info = vk_initializers::buffer_create_info(usage_flags, size);
		auto handler = buffer->get_handler();
		VkResult result = vkCreateBuffer(m_logical_device, &buffer_create_info, nullptr, &handler);
		assert(result == VK_SUCCESS);
		buffer->set_handler(handler);

		VkMemoryRequirements mem_requirements;
		VkMemoryAllocateInfo mem_alloc = vk_initializers::memory_allocate_info();
		vkGetBufferMemoryRequirements(m_logical_device, handler, &mem_requirements);
		mem_alloc.allocationSize = mem_requirements.size;
		mem_alloc.memoryTypeIndex = get_memory_type(mem_requirements.memoryTypeBits, memory_property_flags);
		auto memory = buffer->get_memory();
		result = vkAllocateMemory(m_logical_device, &mem_alloc, nullptr, &memory);
		assert(result == VK_SUCCESS);
		buffer->set_memory(memory);

		buffer->set_aligment(mem_requirements.alignment);
		buffer->set_size(mem_alloc.allocationSize);
		buffer->set_usage_flags(usage_flags);
		buffer->set_memory_property_flags(memory_property_flags);

		if (data != nullptr)
		{
			result = buffer->map();
			assert(result == VK_SUCCESS);
			auto mapped_data = buffer->get_mapped_data();
			memcpy(mapped_data, data, size);
			buffer->unmap();
		}
		buffer->set_descriptor();

		return buffer->bind();
	}

	VkQueue vk_device::get_graphics_queue() const
	{
		return m_graphics_queue;
	}

	VkQueue vk_device::get_present_queue() const
	{
		return m_present_queue;
	}
}