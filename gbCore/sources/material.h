//
//  material.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "texture.h"
#include "shader.h"
#include "material_configuration.h"
#include "resource_accessor.h"
#include "declarations.h"

namespace gb
{
    class material_cached_parameters
    {
    private:
        
    protected:
        
    public:
        
        material_cached_parameters();
        ~material_cached_parameters();
        
        bool m_is_culling;
        ui32 m_culling_mode;
        
        bool m_is_blending;
        ui32 m_blending_function_source;
        ui32 m_blending_function_destination;
        ui32 m_blending_equation;
        
        bool m_is_stencil_test;
        ui32 m_stencil_function;
        i32 m_stencil_function_parameter_1;
        i32 m_stencil_function_parameter_2;
        i32 m_stencil_mask_parameter;
        
        bool m_is_depth_test;
        bool m_is_depth_mask;
        
        bool m_is_color_mask_r;
        bool m_is_color_mask_g;
        bool m_is_color_mask_b;
        bool m_is_color_mask_a;
    
        bool m_is_debugging;
        bool m_is_batching;
        
        ui32 m_z_order;
        
        shader_shared_ptr m_shader;
        std::array<texture_shared_ptr, e_shader_sampler_max> m_textures;
    };
    
    class material
    {
    private:
        
        std::string m_technique_name;
        i32 m_technique_pass;
        
    protected:
        
        std::map<std::string, std::shared_ptr<shader_uniform>> m_custom_shader_uniforms;
        std::shared_ptr<material_cached_parameters> m_parameters;
        
        std::string m_guid;
        
        static std::shared_ptr<material_cached_parameters> m_cached_parameters;
        
        void bind_custom_shader_uniforms();
        
        void update_guid();

#if USED_GRAPHICS_API == VULKAN_API

		VkPipeline m_pipeline = VK_NULL_HANDLE;
		VkGraphicsPipelineCreateInfo m_graphics_pipeline;
		VkPipelineViewportStateCreateInfo m_viewport_state;
		VkPipelineMultisampleStateCreateInfo m_multisampling_state;
		VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state;
		VkPipelineRasterizationStateCreateInfo m_rasterization_state;
		VkPipelineColorBlendAttachmentState m_color_blend_attachment;
		VkPipelineColorBlendStateCreateInfo m_color_blend_state;
		VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state;
		std::vector<VkWriteDescriptorSet> m_descriptor_writes;

		bool m_is_pipeline_constructed = false;
		void construct_pipeline(const VkPipelineVertexInputStateCreateInfo& vertex_input_state);

#elif USED_GRAPHICS_API == METAL_API
        
        mtl_render_encoder_shared_ptr m_render_encoder = nullptr;
        mtl_render_pipeline_state_shared_ptr m_render_pipeline_state = nullptr;
        mtl_depth_stencil_state_shared_ptr m_depth_stencil_state = nullptr;
        mtl_buffer_shared_ptr m_mvp_uniforms_buffer = nullptr;
        mtl_buffer_shared_ptr m_custom_uniforms_buffer = nullptr;
        
#endif
        
    public:
        
        material();
        ~material();
        
        static material_shared_ptr construct(const std::shared_ptr<material_configuration>& configuration);
        
        static std::shared_ptr<material_cached_parameters> get_cached_parameters();
        
        static void set_shader(const material_shared_ptr& material,
                               const std::shared_ptr<material_configuration>& configuration,
                               const resource_accessor_shared_ptr& resource_accessor, bool force = false);
        
        static void set_textures(const material_shared_ptr& material,
                                 const std::shared_ptr<material_configuration>& configuration,
                                 const resource_accessor_shared_ptr& resource_accessor, bool force = false);
        
        bool is_culling() const;
        ui32 get_culling_mode() const;
        
        bool is_blending() const;
        ui32 get_blending_function_source() const;
        ui32 get_blending_function_destination() const;
        ui32 get_blending_equation() const;
        
        bool is_stencil_test() const;
        ui32 get_stencil_function() const;
        i32 get_stencil_function_parameter_1() const;
        i32 get_stencil_function_parameter_2() const;
        i32 get_stencil_mask_parameter() const;
        
        bool is_depth_test() const;
        bool is_depth_mask() const;
        
        bool is_color_mask_r() const;
        bool is_color_mask_g() const;
        bool is_color_mask_b() const;
        bool is_color_mask_a() const;
        
        bool is_debugging() const;
        bool get_is_batching() const;
        
        ui32 get_z_order() const;
        
        const std::string& get_guid() const;
        
        shader_shared_ptr get_shader() const;
        texture_shared_ptr get_texture(e_shader_sampler sampler) const;
        e_shader_sampler get_sampler_index(const texture_shared_ptr& texture) const;
        
        void set_culling(bool value);
        void set_culling_mode(ui32 value);
        
        void set_blending(bool value);
        void set_blending_function_source(ui32 value);
        void set_blending_function_destination(ui32 value);
        void set_blending_equation(ui32 value);
        
        void set_stencil_test(bool value);
        void set_stencil_function(ui32 value);
        void set_stencil_function_parameter_1(i32 value);
        void set_stencil_function_parameter_2(i32 value);
        void set_stencil_mask_parameter(i32 value);
        
        void set_depth_test(bool value);
        void set_depth_mask(bool value);
        
        void set_color_mask_r(bool value);
        void set_color_mask_g(bool value);
        void set_color_mask_b(bool value);
        void set_color_mask_a(bool value);
        
        void set_debugging(bool value);
        void set_is_batching(bool value);
        
        void set_z_order(ui32 z_order);
        
        void set_shader(const shader_shared_ptr& shader);
        void set_texture(const texture_shared_ptr& texture, e_shader_sampler sampler);
        
        void set_custom_shader_uniform(const glm::mat4& matrix, const std::string& uniform);
        void set_custom_shader_uniform(glm::mat4* matrices, i32 size, const std::string& uniform);
        void set_custom_shader_uniform(const glm::mat3& matrix, const std::string& uniform);
        void set_custom_shader_uniform(glm::mat3* matrices, i32 size, const std::string& uniform);
        void set_custom_shader_uniform(const glm::vec4& vector, const std::string& uniform);
        void set_custom_shader_uniform(glm::vec4* vectors, i32 size, const std::string& uniform);
        void set_custom_shader_uniform(const glm::vec3& vector, const std::string& uniform);
        void set_custom_shader_uniform(glm::vec3* vectors, i32 size, const std::string& uniform);
        void set_custom_shader_uniform(const glm::vec2& vector, const std::string& uniform);
        void set_custom_shader_uniform(glm::vec2* vectors, i32 size, const std::string& uniform);
        void set_custom_shader_uniform(f32 value, const std::string& uniform);
        void set_custom_shader_uniform(f32* values, i32 size, const std::string& uniform);
        void set_custom_shader_uniform(i32 value, const std::string& uniform);
        void set_custom_shader_uniform(i32* values, i32 size, const std::string& uniform);
        
        const std::map<std::string, std::shared_ptr<shader_uniform>>& get_custom_uniforms() const;
        
#if USED_GRAPHICS_API == VULKAN_API

		void bind(const VkPipelineVertexInputStateCreateInfo& vertex_input_state);

#elif USED_GRAPHICS_API == METAL_API
        
        void bind(const mtl_vertex_descriptor_shared_ptr& vertex_descriptor);
        
#else

        void bind();

#endif

        void unbind();
        
#if USED_GRAPHICS_API == METAL_API
        
        mtl_render_encoder_shared_ptr get_render_encoder() const;
        mtl_buffer_shared_ptr get_mvp_uniforms_buffer() const;
        mtl_buffer_shared_ptr get_custom_uniform_buffer(ui32 size);
        
#endif
        
    };
};

#endif
