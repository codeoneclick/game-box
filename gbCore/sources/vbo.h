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

namespace gb
{
    class vbo final : public resource_transfering_data
    {
    public:
        
        struct vertex_attribute_pack
        {
            i8 m_data[32];
        };
        
        struct vertex_attribute
        {
            glm::vec3 m_position; // 12
            glm::uint32 m_texcoord; // 4 = 16
            glm::uint32 m_normal; // 4 = 20
            glm::uint32 m_tangent; // 4 = 24
            glm::u8vec4 m_color; // 4 = 28
            glm::u8vec4 m_extra; // 4 = 32
        };
        
        struct vertex_attribute_PTC
        {
            glm::vec3 m_position; // 12
            glm::uint32 m_texcoord; // 4 = 16
            glm::u8vec4 m_color; // 4 = 20
            i8 m_unused[12]; // 12 = 32
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
        
        class vertex_declaration
        {
        private:
            
        protected:
            
            ui32 m_size;
            vertex_attribute* m_data;
            
        public:
            
            vertex_declaration(ui32 size);
            virtual ~vertex_declaration();
            
            vertex_attribute* get_data() const;
            ui32 get_size() const;
            
            virtual void bind(const std::array<i32, e_shader_attribute_max>& attributes) const = 0;
            virtual void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const = 0;
        };
        
        class vertex_declaration_PTC : public vertex_declaration
        {
        private:
            
        protected:
            
        public:
            
            vertex_declaration_PTC(ui32 size);
            ~vertex_declaration_PTC();
            
            void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        };
        
        class vertex_declaration_PT4B : public vertex_declaration
        {
        private:
            
        protected:
            
        public:
            
            vertex_declaration_PT4B(ui32 size);
            ~vertex_declaration_PT4B();
            
            void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
            void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        };
        
    private:
        
        friend class ogl_graveyard_controller;
        
    protected:
        
        static std::mutex m_graveyard_mutex;
        static std::queue<ui32> m_handlers_graveyard;
        void add_to_graveyard(ui32 handler);
        
        ui32 m_handle;
        ui32 m_version;
        
        vertex_attribute_pack* m_data_pack;
        ui32 m_allocated_size;
        ui32 m_used_size;
        
        ui32 m_mode;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
    public:
        
        vbo(ui32 size, ui32 mode);
        ~vbo();
        
        ui32 get_id() const;
        ui32 get_version() const;
        
        ui32 get_allocated_size() const;
        ui32 get_used_size() const;
        
        vertex_attribute* lock() const;
        void unlock(bool is_bathing = false, ui32 size = 0);
        
        glm::vec2 get_min_bound() const;
        glm::vec2 get_max_bound() const;
        
        void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
    };
};
