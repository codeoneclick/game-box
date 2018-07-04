#include "vk_initializers.h"

namespace gb
{
	VkMemoryAllocateInfo vk_initializers::memory_allocate_info()
	{
		VkMemoryAllocateInfo mem_alloc_info {};
		mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		return mem_alloc_info;
	}

	VkMappedMemoryRange vk_initializers::mapped_memory_range()
	{
		VkMappedMemoryRange mapped_memory_range{};
		mapped_memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		return mapped_memory_range;
	}

	VkCommandBufferAllocateInfo vk_initializers::command_buffer_allocate_info(VkCommandPool command_pool, VkCommandBufferLevel level, ui32 buffers_count)
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info {};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.commandPool = command_pool;
		command_buffer_allocate_info.level = level;
		command_buffer_allocate_info.commandBufferCount = buffers_count;
		return command_buffer_allocate_info;
	}

	VkCommandPoolCreateInfo vk_initializers::command_pool_create_info()
	{
		VkCommandPoolCreateInfo command_pool_create_info {};
		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		return command_pool_create_info;
	}

	VkCommandBufferBeginInfo vk_initializers::command_buffer_begin_info()
	{
		VkCommandBufferBeginInfo command_buffer_begin_info {};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		return command_buffer_begin_info;
	}

	VkCommandBufferInheritanceInfo vk_initializers::command_buffer_inheritance_info()
	{
		VkCommandBufferInheritanceInfo command_buffer_inheritance_info {};
		command_buffer_inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		return command_buffer_inheritance_info;
	}

	VkRenderPassBeginInfo vk_initializers::render_pass_begin_info()
	{
		VkRenderPassBeginInfo render_pass_begin_info {};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		return render_pass_begin_info;
	}

	VkRenderPassCreateInfo vk_initializers::render_pass_create_info()
	{
		VkRenderPassCreateInfo render_pass_create_info {};
		render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		return render_pass_create_info;
	}

	VkImageMemoryBarrier vk_initializers::image_memory_barrier()
	{
		VkImageMemoryBarrier image_memory_barrier {};
		image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		return image_memory_barrier;
	}

	VkBufferMemoryBarrier vk_initializers::buffer_memory_barrier()
	{
		VkBufferMemoryBarrier buffer_memory_barrier{};
		buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		return buffer_memory_barrier;
	}

	VkMemoryBarrier vk_initializers::memory_barrier()
	{
		VkMemoryBarrier memory_barrier {};
		memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		return memory_barrier;
	}

	VkImageCreateInfo vk_initializers::image_create_info()
	{
		VkImageCreateInfo image_create_info {};
		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		return image_create_info;
	}

	VkSamplerCreateInfo vk_initializers::sampler_create_info()
	{
		VkSamplerCreateInfo sampler_create_info {};
		sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_create_info.maxAnisotropy = 1.f;
		return sampler_create_info;
	}

	VkImageViewCreateInfo vk_initializers::image_view_create_info()
	{
		VkImageViewCreateInfo image_view_create_info {};
		image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		return image_view_create_info;
	}

	VkFramebufferCreateInfo vk_initializers::frame_buffer_create_info()
	{
		VkFramebufferCreateInfo frame_buffer_create_info {};
		frame_buffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		return frame_buffer_create_info;
	}

	VkSemaphoreCreateInfo vk_initializers::semaphore_create_info()
	{
		VkSemaphoreCreateInfo semaphore_create_info {};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		return semaphore_create_info;
	}

	VkFenceCreateInfo vk_initializers::fence_create_info(VkFenceCreateFlags flags)
	{
		VkFenceCreateInfo fence_create_info {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.flags = flags;
		return fence_create_info;
	}

	VkEventCreateInfo vk_initializers::event_create_info()
	{
		VkEventCreateInfo event_create_info{};
		event_create_info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
		return event_create_info;
	}

	VkSubmitInfo vk_initializers::submit_info()
	{
		VkSubmitInfo submit_info {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		return submit_info;
	}

	VkViewport vk_initializers::viewport(f32 width, f32 height, f32 min_depth, f32 max_depth)
	{
		VkViewport viewport {};
		viewport.width = width;
		viewport.height = height;
		viewport.minDepth = min_depth;
		viewport.maxDepth = max_depth;
		return viewport;
	}

	VkRect2D vk_initializers::rect_2d(i32 width, i32 height, i32 offset_x, i32 offset_y)
	{
		VkRect2D rect_2d {};
		rect_2d.extent.width = width;
		rect_2d.extent.height = height;
		rect_2d.offset.x = offset_x;
		rect_2d.offset.y = offset_y;
		return rect_2d;
	}

	VkBufferCreateInfo vk_initializers::buffer_create_info()
	{
		VkBufferCreateInfo buffer_create_info{};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		return buffer_create_info;
	}

	VkBufferCreateInfo vk_initializers::buffer_create_info(VkBufferUsageFlags usage, VkDeviceSize size)
	{
		VkBufferCreateInfo buffer_create_info{};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.usage = usage;
		buffer_create_info.size = size;
		return buffer_create_info;
	}

	VkDescriptorPoolCreateInfo vk_initializers::descriptor_pool_create_info(ui32 pool_sizes_count, VkDescriptorPoolSize* pool_sizes, ui32 max_sets)
	{
		VkDescriptorPoolCreateInfo descriptor_pool_info {};
		descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_info.poolSizeCount = pool_sizes_count;
		descriptor_pool_info.pPoolSizes = pool_sizes;
		descriptor_pool_info.maxSets = max_sets;
		return descriptor_pool_info;
	}

	VkDescriptorPoolCreateInfo vk_initializers::descriptor_pool_create_info(const std::vector<VkDescriptorPoolSize>& pool_sizes, ui32 max_sets)
	{
		VkDescriptorPoolCreateInfo descriptor_pool_info {};
		descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		descriptor_pool_info.pPoolSizes = pool_sizes.data();
		descriptor_pool_info.maxSets = max_sets;
		return descriptor_pool_info;
	}

	VkDescriptorPoolSize vk_initializers::descriptor_pool_size(VkDescriptorType type, ui32 descriptors_count)
	{
		VkDescriptorPoolSize descriptor_pool_size {};
		descriptor_pool_size.type = type;
		descriptor_pool_size.descriptorCount = descriptors_count;
		return descriptor_pool_size;
	}

	VkDescriptorSetLayoutBinding vk_initializers::descriptor_set_layout_binding(VkDescriptorType type, VkShaderStageFlags stage_flags, ui32 binding, ui32 descriptors_count)
	{
		VkDescriptorSetLayoutBinding set_layout_binding {};
		set_layout_binding.descriptorType = type;
		set_layout_binding.stageFlags = stage_flags;
		set_layout_binding.binding = binding;
		set_layout_binding.descriptorCount = descriptors_count;
		return set_layout_binding;
	}

	VkDescriptorSetLayoutCreateInfo vk_initializers::descriptor_set_layout_create_info(const VkDescriptorSetLayoutBinding* bindings, ui32 bindings_count)
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info {};
		descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_create_info.pBindings = bindings;
		descriptor_set_layout_create_info.bindingCount = bindings_count;
		return descriptor_set_layout_create_info;
	}

	VkDescriptorSetLayoutCreateInfo vk_initializers::descriptor_set_layout_create_info(const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info {};
		descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_create_info.pBindings = bindings.data();
		descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		return descriptor_set_layout_create_info;
	}

	VkPipelineLayoutCreateInfo vk_initializers::pipeline_layout_create_info(const VkDescriptorSetLayout* set_layouts, ui32 set_layouts_count)
	{
		VkPipelineLayoutCreateInfo pipeline_layout_create_info {};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = set_layouts_count;
		pipeline_layout_create_info.pSetLayouts = set_layouts;
		return pipeline_layout_create_info;
	}

	VkPipelineLayoutCreateInfo vk_initializers::pipeline_layout_create_info(ui32 set_layouts_count)
	{
		VkPipelineLayoutCreateInfo pipeline_layout_create_info {};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = set_layouts_count;
		return pipeline_layout_create_info;
	}

	VkDescriptorSetAllocateInfo vk_initializers::descriptorSetAllocateInfo(VkDescriptorPool descriptor_pool, const VkDescriptorSetLayout* set_layouts, ui32 descriptor_sets_count)
	{
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info {};
		descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_allocate_info.descriptorPool = descriptor_pool;
		descriptor_set_allocate_info.pSetLayouts = set_layouts;
		descriptor_set_allocate_info.descriptorSetCount = descriptor_sets_count;
		return descriptor_set_allocate_info;
	}

	VkDescriptorImageInfo vk_initializers::descriptor_image_info(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout)
	{
		VkDescriptorImageInfo descriptor_imageInfo {};
		descriptor_imageInfo.sampler = sampler;
		descriptor_imageInfo.imageView = image_view;
		descriptor_imageInfo.imageLayout = image_layout;
		return descriptor_imageInfo;
	}

	VkWriteDescriptorSet vk_initializers::write_descriptor_set(VkDescriptorSet dst_set, VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo* buffer_info, ui32 descriptors_count)
	{
		VkWriteDescriptorSet write_descriptor_set {};
		write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.dstSet = dst_set;
		write_descriptor_set.descriptorType = type;
		write_descriptor_set.dstBinding = binding;
		write_descriptor_set.pBufferInfo = buffer_info;
		write_descriptor_set.descriptorCount = descriptors_count;
		return write_descriptor_set;
	}

	VkWriteDescriptorSet vk_initializers::write_descriptor_set(VkDescriptorSet dst_set, VkDescriptorType type, ui32 binding, VkDescriptorImageInfo *image_info, ui32 descriptors_count)
	{
		VkWriteDescriptorSet write_descriptor_set {};
		write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.dstSet = dst_set;
		write_descriptor_set.descriptorType = type;
		write_descriptor_set.dstBinding = binding;
		write_descriptor_set.pImageInfo = image_info;
		write_descriptor_set.descriptorCount = descriptors_count;
		return write_descriptor_set;
	}

	VkVertexInputBindingDescription vk_initializers::vertex_input_binding_description(ui32 binding, ui32 stride, VkVertexInputRate input_rate)
	{
		VkVertexInputBindingDescription vertex_input_bind_description {};
		vertex_input_bind_description.binding = binding;
		vertex_input_bind_description.stride = stride;
		vertex_input_bind_description.inputRate = input_rate;
		return vertex_input_bind_description;
	}

	VkVertexInputAttributeDescription vk_initializers::vertex_input_attribute_description(ui32 binding, ui32 location, VkFormat format, ui32 offset)
	{
		VkVertexInputAttributeDescription vertex_input_attribute_description {};
		vertex_input_attribute_description.location = location;
		vertex_input_attribute_description.binding = binding;
		vertex_input_attribute_description.format = format;
		vertex_input_attribute_description.offset = offset;
		return vertex_input_attribute_description;
	}

	VkPipelineVertexInputStateCreateInfo vk_initializers::pipeline_vertex_input_state_create_info()
	{
		VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info {};
		pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		return pipeline_vertex_input_state_create_info;
	}

	VkPipelineInputAssemblyStateCreateInfo vk_initializers::pipeline_input_assembly_state_create_info(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitive_restart_enable)
	{
		VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info {};
		pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipeline_input_assembly_state_create_info.topology = topology;
		pipeline_input_assembly_state_create_info.flags = flags;
		pipeline_input_assembly_state_create_info.primitiveRestartEnable = primitive_restart_enable;
		return pipeline_input_assembly_state_create_info;
	}

	inline VkPipelineRasterizationStateCreateInfo vk_initializers::pipeline_rasterization_state_create_info(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, VkPipelineRasterizationStateCreateFlags flags)
	{
		VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info {};
		pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipeline_rasterization_state_create_info.polygonMode = polygon_mode;
		pipeline_rasterization_state_create_info.cullMode = cull_mode;
		pipeline_rasterization_state_create_info.frontFace = front_face;
		pipeline_rasterization_state_create_info.flags = flags;
		pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
		pipeline_rasterization_state_create_info.lineWidth = 1.f;
		return pipeline_rasterization_state_create_info;
	}

	VkPipelineColorBlendAttachmentState vk_initializers::pipeline_color_blend_attachment_state(VkColorComponentFlags color_write_mask, VkBool32 blend_enable)
	{
		VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state {};
		pipeline_color_blend_attachment_state.colorWriteMask = color_write_mask;
		pipeline_color_blend_attachment_state.blendEnable = blend_enable;
		return pipeline_color_blend_attachment_state;
	}

	VkPipelineColorBlendStateCreateInfo vk_initializers::pipeline_color_blend_state_create_info(ui32 attachments_count, const VkPipelineColorBlendAttachmentState* attachments)
	{
		VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info {};
		pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipeline_color_blend_state_create_info.attachmentCount = attachments_count;
		pipeline_color_blend_state_create_info.pAttachments = attachments;
		return pipeline_color_blend_state_create_info;
	}

	VkPipelineDepthStencilStateCreateInfo vk_initializers::pipeline_depth_stencil_state_create_info(VkBool32 depth_test_enable, VkBool32 depth_write_enable, VkCompareOp depth_compare_op)
	{
		VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info {};
		pipeline_depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipeline_depth_stencil_state_create_info.depthTestEnable = depth_test_enable;
		pipeline_depth_stencil_state_create_info.depthWriteEnable = depth_write_enable;
		pipeline_depth_stencil_state_create_info.depthCompareOp = depth_compare_op;
		pipeline_depth_stencil_state_create_info.front = pipeline_depth_stencil_state_create_info.back;
		pipeline_depth_stencil_state_create_info.back.compareOp = VK_COMPARE_OP_ALWAYS;
		return pipeline_depth_stencil_state_create_info;
	}

	VkPipelineViewportStateCreateInfo vk_initializers::pipeline_viewport_state_create_info(ui32 viewports_count, ui32 scissors_count, VkPipelineViewportStateCreateFlags flags)
	{
		VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info {};
		pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipeline_viewport_state_create_info.viewportCount = viewports_count;
		pipeline_viewport_state_create_info.scissorCount = scissors_count;
		pipeline_viewport_state_create_info.flags = flags;
		return pipeline_viewport_state_create_info;
	}

	VkPipelineMultisampleStateCreateInfo vk_initializers::pipeline_multisample_state_create_info(VkSampleCountFlagBits rasterization_samples, VkPipelineMultisampleStateCreateFlags flags)
	{
		VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info {};
		pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipeline_multisample_state_create_info.rasterizationSamples = rasterization_samples;
		pipeline_multisample_state_create_info.flags = flags;
		return pipeline_multisample_state_create_info;
	}

	inline VkPipelineDynamicStateCreateInfo vk_initializers::pipeline_dynamic_state_create_info(const VkDynamicState* dynamic_states, ui32 dynamic_states_count, VkPipelineDynamicStateCreateFlags flags)
	{
		VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info {};
		pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipeline_dynamic_state_create_info.pDynamicStates = dynamic_states;
		pipeline_dynamic_state_create_info.dynamicStateCount = dynamic_states_count;
		pipeline_dynamic_state_create_info.flags = flags;
		return pipeline_dynamic_state_create_info;
	}

	VkPipelineDynamicStateCreateInfo vk_initializers::pipeline_dynamic_state_create_info(const std::vector<VkDynamicState>& dynamic_states, VkPipelineDynamicStateCreateFlags flags)
	{
		VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info {};
		pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipeline_dynamic_state_create_info.pDynamicStates = dynamic_states.data();
		pipeline_dynamic_state_create_info.dynamicStateCount = static_cast<ui32>(dynamic_states.size());
		pipeline_dynamic_state_create_info.flags = flags;
		return pipeline_dynamic_state_create_info;
	}

	VkPipelineTessellationStateCreateInfo vk_initializers::pipeline_tessellation_state_create_info(ui32 patch_control_points)
	{
		VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info {};
		pipeline_tessellation_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		pipeline_tessellation_state_create_info.patchControlPoints = patch_control_points;
		return pipeline_tessellation_state_create_info;
	}

	VkGraphicsPipelineCreateInfo vk_initializers::pipeline_create_info(VkPipelineLayout layout, VkRenderPass render_pass, VkPipelineCreateFlags flags)
	{
		VkGraphicsPipelineCreateInfo pipeline_create_info {};
		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_create_info.layout = layout;
		pipeline_create_info.renderPass = render_pass;
		pipeline_create_info.flags = flags;
		pipeline_create_info.basePipelineIndex = -1;
		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
		return pipeline_create_info;
	}

	VkComputePipelineCreateInfo vk_initializers::compute_pipeline_create_info(VkPipelineLayout layout, VkPipelineCreateFlags flags)
	{
		VkComputePipelineCreateInfo compute_pipeline_create_info {};
		compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		compute_pipeline_create_info.layout = layout;
		compute_pipeline_create_info.flags = flags;
		return compute_pipeline_create_info;
	}

	VkPushConstantRange vk_initializers::push_constant_range(VkShaderStageFlags stage_flags, ui32 size, ui32 offset)
	{
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = stage_flags;
		push_constant_range.offset = offset;
		push_constant_range.size = size;
		return push_constant_range;
	}

	VkBindSparseInfo vk_initializers::bind_sparse_info()
	{
		VkBindSparseInfo bind_sparse_info {};
		bind_sparse_info.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;
		return bind_sparse_info;
	}

	VkSpecializationMapEntry vk_initializers::specialization_map_entry(ui32 constant_id, ui32 offset, size_t size)
	{
		VkSpecializationMapEntry specialization_map_entry {};
		specialization_map_entry.constantID = constant_id;
		specialization_map_entry.offset = offset;
		specialization_map_entry.size = size;
		return specialization_map_entry;
	}

	VkSpecializationInfo vk_initializers::specialization_info(ui32 map_entries_count, const VkSpecializationMapEntry* map_entries, size_t data_size, const void* data)
	{
		VkSpecializationInfo specialization_info {};
		specialization_info.mapEntryCount = map_entries_count;
		specialization_info.pMapEntries = map_entries;
		specialization_info.dataSize = data_size;
		specialization_info.pData = data;
		return specialization_info;
	}
}