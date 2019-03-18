 //
//  vbo.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"
#include "shader.h"
#include "vk_buffer.h"

namespace gb
{
    class vbo final : public resource_transfering_data
    {
    public:
        
        struct vertex_attribute
        {
            i8 m_data[32];
        };
        
        struct vertex_attribute_P
        {
            glm::vec3 m_position; // 12
            i8 m_unused[20]; // 20 = 32
        };
        
        struct vertex_attribute_PTC
        {
            glm::vec3 m_position; // 12
            glm::vec2 m_texcoord; // 8 = 20
            glm::u8vec4 m_color; // 4 = 24
            i8 m_unused[8]; // 8 = 32
        };
        
        struct vertex_attribute_PT4B
        {
            glm::vec3 m_position; // 12
            glm::uint32 m_texcoord; // 4 = 16
            glm::u8vec4 m_bone_ids; // 4 = 20
            glm::u8vec4 m_bone_weights; // 4 = 24
            i8 m_unused[8]; // 8 = 32
        };
        
        struct vertex_attribute_PT8B
        {
            glm::vec3 m_position; // 12
            glm::uint32 m_texcoord; // 4 = 16
            glm::u8vec4 m_bone_ids_group_01; // 4 = 20
            glm::u8vec4 m_bone_ids_group_02; // 4 = 24
            glm::u8vec4 m_bone_weights_group_01; // 4 = 28
            glm::u8vec4 m_bone_weights_group_02; // 4 = 32
        };
        
        struct vertex_attribute_PTNTC
        {
            glm::vec3 m_position;
            glm::vec2 m_texcoord;
            glm::uint32 m_normal;
            glm::uint32 m_tangent;
            glm::u8vec4 m_color;
        };
        
        class vertex_declaration
        {
        private:
            
            friend class vbo;
            
        protected:
            
            ui32 m_size = 0;
            vertex_attribute* m_data = nullptr;
            bool m_is_external_data;

			static std::unordered_map<std::string, ui32> m_attributes_locations;

#if USED_GRAPHICS_API == VULKAN_API

			std::vector<VkVertexInputBindingDescription> m_bindings_description;
			std::vector<VkVertexInputAttributeDescription> m_attributes_description;
			VkPipelineVertexInputStateCreateInfo m_vertex_input_state;

#endif
            
#if USED_GRAPHICS_API == METAL_API
      
            mtl_vertex_descriptor_shared_ptr m_mtl_vertex_descriptor = nullptr;
            
#endif
            
            vertex_attribute* get_data() const;
            ui32 get_size() const;

            virtual void bind(const std::array<i32, e_shader_attribute_max>& attributes) const = 0;
            virtual void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const = 0;
            
        public:
            
            vertex_declaration(ui32 size, vertex_attribute* external_data = nullptr);
            virtual ~vertex_declaration();

#if USED_GRAPHICS_API == VULKAN_API

			VkPipelineVertexInputStateCreateInfo get_vertex_input_state() const;

#endif
            
#if USED_GRAPHICS_API == METAL_API
            
            mtl_vertex_descriptor_shared_ptr get_mtl_vertex_descriptor() const;
            
#endif

        };
        
        class vertex_declaration_PTC : public vertex_declaration
        {
        private:
            
        protected:
            
            void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            
        public:
            
            vertex_declaration_PTC(ui32 size, vertex_attribute* external_data = nullptr);
            ~vertex_declaration_PTC();
        };
        
        class vertex_declaration_PT4B : public vertex_declaration
        {
        private:
            
        protected:
            
            void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            
        public:
            
            vertex_declaration_PT4B(ui32 size, vertex_attribute* external_data = nullptr);
            ~vertex_declaration_PT4B();
        };
        
        class vertex_declaration_PTNTC : public vertex_declaration
        {
        private:
            
        protected:
            
            void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            
        public:
            
            vertex_declaration_PTNTC(ui32 size, vertex_attribute* external_data = nullptr);
            ~vertex_declaration_PTNTC();
        };
        
    private:
        
        friend class ogl_graveyard_controller;
        
    protected:
        
        static std::mutex m_graveyard_mutex;
        static std::queue<ui32> m_handlers_graveyard;
        void add_to_graveyard(ui32 handler);

#if USED_GRAPHICS_API == VULKAN_API

		std::shared_ptr<vk_buffer> m_main_buffer = nullptr;
		std::shared_ptr<vk_buffer> m_staging_buffer = nullptr;

		VkPipelineVertexInputStateCreateInfo m_vk_vertex_input;
		VkPipelineInputAssemblyStateCreateInfo m_vk_input_assembly;

#endif
        
#if USED_GRAPHICS_API == METAL_API
        
        mtl_buffer_shared_ptr m_mtl_buffer_id = nullptr;
        
#endif
        
        ui32 m_handle;
        ui32 m_version;
        
        std::shared_ptr<vertex_declaration> m_declaration;
        ui32 m_allocated_size;
        ui32 m_used_size;
        
        ui32 m_mode;
        bool m_is_using_batch;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
    public:
        
        vbo(const std::shared_ptr<vertex_declaration>& declaration, ui32 mode, bool is_using_batch = false);
        ~vbo();
        
        ui32 get_id() const;
        ui32 get_version() const;
        
        bool is_using_batch() const;
        
        ui32 get_allocated_size() const;
        ui32 get_used_size() const;
        
        template<class T>
        T* lock() const
        {
            assert(m_declaration != nullptr);
            assert(m_declaration->get_data() != nullptr);
            return (T*)m_declaration->get_data();
        };
        
        void unlock(ui32 size = 0, bool submit_to_vram = true);
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        
        
#if USED_GRAPHICS_API == METAL_API
        
        mtl_buffer_shared_ptr get_mtl_buffer_id() const;
        mtl_vertex_descriptor_shared_ptr get_mtl_vertex_descriptor() const;
        
#endif

#if USED_GRAPHICS_API == VULKAN_API

		VkPipelineVertexInputStateCreateInfo get_vertex_input_state() const;

#endif

    };
};
