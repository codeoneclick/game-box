//
//  render_pipeline.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_pipeline.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "graphics_context.h"
#include "render_technique_main.h"
#include "render_technique_ws.h"
#include "render_technique_ss.h"
#include "vk_device.h"
#include "vk_swap_chain.h"

namespace gb
{
    render_pipeline::render_pipeline(const graphics_context_shared_ptr& graphic_context, bool is_offscreen) :
    gb::render_techniques_importer(graphic_context, is_offscreen),
    gb::render_techniques_accessor()
    {

    }
    
    render_pipeline::~render_pipeline()
    {
        
    }

    void render_pipeline::on_draw_begin()
    {
		assert(m_graphics_context);

        m_graphics_context->make_current();

#if USED_GRAPHICS_API == VULKAN_API

		ui32 current_image_index = vk_device::get_instance()->get_current_image_index();
		VkRenderPass render_pass = vk_swap_chain::get_instance()->get_render_pass();

		VkCommandBuffer draw_cmd_buffer = vk_device::get_instance()->get_draw_cmd_buffer(current_image_index);
		VkFramebuffer frame_buffer = vk_device::get_instance()->get_frame_buffer(current_image_index);

		vkResetCommandBuffer(draw_cmd_buffer, 0);

		VkCommandBufferBeginInfo cmd_buffer_info = {};
		cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmd_buffer_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		cmd_buffer_info.pNext = nullptr;

		VkClearValue clear_values[2];
		clear_values[0].color = { { 1.f, 0.f, 0.f, 1.f } };
		clear_values[1].depthStencil = { 1.f, 0 };

		VkRenderPassBeginInfo vk_render_pass_begin_info = {};
		vk_render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vk_render_pass_begin_info.pNext = nullptr;
		vk_render_pass_begin_info.renderPass = render_pass;
		vk_render_pass_begin_info.renderArea.offset.x = 0;
		vk_render_pass_begin_info.renderArea.offset.y = 0;
		vk_render_pass_begin_info.renderArea.extent.width = get_screen_width();
		vk_render_pass_begin_info.renderArea.extent.height = get_screen_height();
		vk_render_pass_begin_info.clearValueCount = 2;
		vk_render_pass_begin_info.pClearValues = clear_values;
		vk_render_pass_begin_info.framebuffer = frame_buffer;
		
		VkResult result = vkBeginCommandBuffer(draw_cmd_buffer, &cmd_buffer_info);
		assert(result == VK_SUCCESS);

		vkCmdBeginRenderPass(draw_cmd_buffer, &vk_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = {};
		viewport.height = static_cast<f32>(get_screen_width());
		viewport.width = static_cast<f32>(get_screen_height());
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		vkCmdSetViewport(draw_cmd_buffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = get_screen_width();
		scissor.extent.height = get_screen_height();
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(draw_cmd_buffer, 0, 1, &scissor);

#endif

    }
    
    void render_pipeline::on_draw_end()
    {
        for(const auto& technique_it : m_ordered_ss_render_techniques)
        {
            if (technique_it->get_current_skipped_frames_count() == technique_it->get_skip_frames_count())
            {
                technique_it->bind();
                technique_it->draw();
                technique_it->unbind();
            }
            technique_it->inc_current_skipped_frames_count();
        }
        
       
        if(m_main_render_technique && !m_offscreen)
        {
            m_main_render_technique->bind();
            m_main_render_technique->draw();
            m_main_render_technique->unbind();
        }

#if USED_GRAPHICS_API == VULKAN_API

		ui32 current_image_index = vk_device::get_instance()->get_current_image_index();
		VkCommandBuffer draw_cmd_buffer = vk_device::get_instance()->get_draw_cmd_buffer(current_image_index);

		vkCmdEndRenderPass(draw_cmd_buffer);

		VkResult result = vkEndCommandBuffer(draw_cmd_buffer);
		assert(result == VK_SUCCESS);

#endif
        
        if(!m_offscreen)
        {
            m_graphics_context->draw();
        }
    }
    
    texture_shared_ptr render_pipeline::get_technique_texture(const std::string& technique_name)
    {
        std::string find_technique_name = technique_name;
        std::string::size_type location = technique_name.find(".depth");
        if (std::string::npos != location)
        {
            find_technique_name = std::string(technique_name, 0, location);
        }
        std::shared_ptr<texture> texture = m_ws_render_techniques.find(find_technique_name) != m_ws_render_techniques.end() ? std::string::npos == location ? m_ws_render_techniques.find(find_technique_name)->second->get_color_attachment_texture() : m_ws_render_techniques.find(find_technique_name)->second->get_depth_attachment_texture() : m_ss_render_techniques.find(find_technique_name) != m_ss_render_techniques.end() ? m_ss_render_techniques.find(find_technique_name)->second->get_color_attachment_texture() : nullptr;
        assert(texture != nullptr);
        return texture;
    }
    
    material_shared_ptr render_pipeline::get_technique_material(const std::string& technique_name)
    {
        material_shared_ptr material = m_ss_render_techniques.find(technique_name) != m_ss_render_techniques.end() ? m_ss_render_techniques.find(technique_name)->second->get_material(): nullptr;
        assert(material != nullptr);
        return material;
    }
    
    ui32 render_pipeline::get_screen_width()
    {
        assert(m_graphics_context != nullptr);
        return m_graphics_context->get_width();
    }
    
    ui32 render_pipeline::get_screen_height()
    {
        assert(m_graphics_context != nullptr);
        return m_graphics_context->get_height();
    }
    
    graphics_context_shared_ptr render_pipeline::get_graphics_context() const
    {
        return m_graphics_context;
    }
    
	const std::list<std::shared_ptr<render_technique_ws>>& render_pipeline::get_ws_techniques() const
    {
        return m_ordered_ws_render_techniques;
    }
    
    const const std::list<std::shared_ptr<render_technique_ss>>& render_pipeline::get_ss_techniques() const
    {
        return m_ordered_ss_render_techniques;
    }
    
    std::shared_ptr<render_technique_main> render_pipeline::get_main_technique() const
    {
        return m_main_render_technique;
    }
}

#endif
