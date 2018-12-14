#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == VULKAN_API

namespace gb
{
	class vk_initializers
	{
	public:

		static VkMemoryAllocateInfo memory_allocate_info();

		static VkMappedMemoryRange mapped_memory_range();

		static VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool command_pool, VkCommandBufferLevel level, ui32 buffers_count);

		static VkCommandPoolCreateInfo command_pool_create_info();

		static VkCommandBufferBeginInfo command_buffer_begin_info();

		static VkCommandBufferInheritanceInfo command_buffer_inheritance_info();

		static VkRenderPassBeginInfo render_pass_begin_info();

		static VkRenderPassCreateInfo render_pass_create_info();

		static VkImageMemoryBarrier image_memory_barrier();

		static VkBufferMemoryBarrier buffer_memory_barrier();

		static VkMemoryBarrier memory_barrier();

		static VkImageCreateInfo image_create_info();

		static VkSamplerCreateInfo sampler_create_info();

		static VkImageViewCreateInfo image_view_create_info();

		static VkFramebufferCreateInfo frame_buffer_create_info();

		static VkSemaphoreCreateInfo semaphore_create_info();

		static VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);

		static VkEventCreateInfo event_create_info();

		static VkSubmitInfo submit_info();

		static VkViewport viewport(f32 width, f32 height, f32 min_depth, f32 max_depth);

		static VkRect2D rect_2d(i32 width, i32 height, i32 offset_x, i32 offset_y);

		static VkBufferCreateInfo buffer_create_info();

		static VkBufferCreateInfo buffer_create_info(VkBufferUsageFlags usage, VkDeviceSize size);

		static VkDescriptorPoolCreateInfo descriptor_pool_create_info(ui32 pool_size_count, VkDescriptorPoolSize* pool_sizes, ui32 max_sets);

		static VkDescriptorPoolCreateInfo descriptor_pool_create_info(const std::vector<VkDescriptorPoolSize>& pool_sizes, ui32 max_sets);

		static VkDescriptorPoolSize descriptor_pool_size(VkDescriptorType type, ui32 descriptors_count);

		static VkDescriptorSetLayoutBinding descriptor_set_layout_binding(VkDescriptorType type, VkShaderStageFlags stage_flags, ui32 binding, ui32 descriptor_count = 1);

		static VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(const VkDescriptorSetLayoutBinding* bindings, ui32 binding_count);

		static VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		static VkPipelineLayoutCreateInfo pipeline_layout_create_info(const VkDescriptorSetLayout* set_layouts, ui32 set_layouts_count = 1);

		static VkPipelineLayoutCreateInfo pipeline_layout_create_info(ui32 set_layout_count = 1);

		static VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(VkDescriptorPool descriptor_pool, const VkDescriptorSetLayout* set_layouts, ui32 descriptor_sets_count);

		static VkDescriptorImageInfo descriptor_image_info(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout);

		static VkWriteDescriptorSet write_descriptor_set(VkDescriptorSet dst_set, VkDescriptorType type, ui32 binding, VkDescriptorBufferInfo* buffer_info, ui32 descriptors_count = 1);

		static VkWriteDescriptorSet write_descriptor_set(VkDescriptorSet dst_set, VkDescriptorType type, ui32 binding, VkDescriptorImageInfo *image_info, ui32 descriptors_count = 1);

		static VkVertexInputBindingDescription vertex_input_binding_description(ui32 binding, ui32 stride, VkVertexInputRate input_rate);

		static VkVertexInputAttributeDescription vertex_input_attribute_description(ui32 binding, ui32 location, VkFormat format, ui32 offset);

		static VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info();

		static VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitive_restart_enable);

		static VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, VkPipelineRasterizationStateCreateFlags flags = 0);

		static VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state(VkColorComponentFlags color_write_mask, VkBool32 blend_enable);

		static VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info(ui32 attachments_count, const VkPipelineColorBlendAttachmentState * attachments);

		static VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info(VkBool32 depth_test_enable, VkBool32 depth_write_enable, VkCompareOp depth_compare_op);

		static VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info(ui32 viewports_count, ui32 scissors_count, VkPipelineViewportStateCreateFlags flags = 0);

		static VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info(VkSampleCountFlagBits rasterization_samples, VkPipelineMultisampleStateCreateFlags flags = 0);

		static VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info(const VkDynamicState * dynamic_states, ui32 dynamic_states_count, VkPipelineDynamicStateCreateFlags flags = 0);

		static VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info(const std::vector<VkDynamicState>& dynamic_states, VkPipelineDynamicStateCreateFlags flags = 0);

		static VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info(ui32 patch_control_points);

		static VkGraphicsPipelineCreateInfo pipeline_create_info(VkPipelineLayout layout, VkRenderPass render_pass, VkPipelineCreateFlags flags = 0);

		static VkComputePipelineCreateInfo compute_pipeline_create_info(VkPipelineLayout layout, VkPipelineCreateFlags flags = 0);

		static VkPushConstantRange push_constant_range(VkShaderStageFlags stage_flags, ui32 size, ui32 offset);

		static VkBindSparseInfo bind_sparse_info();

		static VkSpecializationMapEntry specialization_map_entry(ui32 constant_id, ui32 offset, size_t size);

		static VkSpecializationInfo specialization_info(ui32 map_entry_count, const VkSpecializationMapEntry* map_entries, size_t data_size, const void* data);
	};
};

#endif
