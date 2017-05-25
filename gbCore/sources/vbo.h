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
#include "std_extensions.h"

namespace gb
{
    enum e_vertex_attribute_type
    {
        e_vertex_attribute_type_position = 0,
        e_vertex_attribute_type_texcoord,
        e_vertex_attribute_type_normal,
        e_vertex_attribute_type_tangent,
        e_vertex_attribute_type_color,
        e_vertex_attribute_type_extra,
        e_vertex_attribute_type_extra_01,
        e_vertex_attribute_type_extra_02,
        e_vertex_attribute_type_max
    };
    
    struct vertex_attribute
    {
    protected:
        
        std::function<void*(e_vertex_attribute_type)> m_attributes_offset;
        
        vertex_attribute()
        {
            position.getter([]() {
                assert(false);
                return glm::vec3(0.f);
            });
            position.setter([](const glm::vec3& position) {
                assert(false);
            });
            
            texcoord.getter([]() {
                assert(false);
                return 0;
            });
            texcoord.setter([](glm::uint32 texcoord) {
                assert(false);
            });
            
            normal.getter([]() {
                assert(false);
                return 0;
            });
            normal.setter([](glm::uint32 normal) {
                assert(false);
            });
            
            tangent.getter([]() {
                assert(false);
                return 0;
            });
            tangent.setter([](glm::uint32 tangent) {
                assert(false);
            });
            
            color.getter([]() {
                assert(false);
                return glm::u8vec4(0);
            });
            color.setter([](const glm::u8vec4& color) {
                assert(false);
            });
            
            extra.getter([]() {
                assert(false);
                return glm::u8vec4(0);
            });
            extra.setter([](const glm::u8vec4& extra) {
                assert(false);
            });
            
            m_attributes_offset = [=](e_vertex_attribute_type type) {
                assert(false);
                return nullptr;
            };
        };
        
    public:
        
        std::property_rw<glm::vec3>   position;
        std::property_rw<glm::uint32> texcoord;
        std::property_rw<glm::uint32> normal;
        std::property_rw<glm::uint32> tangent;
        std::property_rw<glm::u8vec4> color;
        std::property_rw<glm::u8vec4> extra;
        
        void* attribute_offset(e_vertex_attribute_type type) const
        {
            return m_attributes_offset(type);
        };
    };
    
    struct vertex_attribute_PTC : public vertex_attribute
    {
        glm::vec3 m_position;
        glm::uint32 m_texcoord;
        glm::u8vec4 m_color;
        
        vertex_attribute_PTC()
        {
            position.getter([=]() {
                return m_position;
            });
            position.setter([=](const glm::vec3& position) {
                m_position = position;
            });
            
            texcoord.getter([=]() {
                return m_texcoord;
            });
            texcoord.setter([=](glm::uint32 texcoord) {
                m_texcoord = texcoord;
            });
            
            color.getter([=]() {
                return m_color;
            });
            color.setter([=](const glm::u8vec4& color) {
                m_color = color;
            });
            
            m_attributes_offset = [=](e_vertex_attribute_type type) -> void* {
                void* result = nullptr;
                switch(type)
                {
                    case e_vertex_attribute_type_position:
                    {
                        result = (void*)offsetof(vertex_attribute_PTC, m_position);
                    }
                        break;
                    case e_vertex_attribute_type_texcoord:
                    {
                        result = (void*)offsetof(vertex_attribute_PTC, m_texcoord);
                    }
                        break;
                    case e_vertex_attribute_type_color:
                    {
                        result = (void*)offsetof(vertex_attribute_PTC, m_color);
                    }
                        break;
                    default:
                        assert(false);
                        break;
                }
                assert(false);
                return result;
            };
        };
    };
    
    struct vertex_attribute_PTCE : public vertex_attribute
    {
        glm::vec3   m_position;
        glm::uint32 m_texcoord;
        glm::u8vec4 m_color;
        glm::u8vec4 m_extra;
        
        vertex_attribute_PTCE()
        {
            position.getter([=]() {
                return m_position;
            });
            position.setter([=](const glm::vec3& position) {
                m_position = position;
            });
            
            texcoord.getter([=]() {
                return m_texcoord;
            });
            texcoord.setter([=](glm::uint32 texcoord) {
                m_texcoord = texcoord;
            });
            
            color.getter([=]() {
                return m_color;
            });
            color.setter([=](const glm::u8vec4& color) {
                m_color = color;
            });
            
            extra.getter([=]() {
                return m_color;
            });
            extra.setter([=](const glm::u8vec4& color) {
                m_color = color;
            });
            
            m_attributes_offset = [=](e_vertex_attribute_type type) -> void* {
                void* result = nullptr;
                switch(type)
                {
                    case e_vertex_attribute_type_position:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_position);
                    }
                        break;
                    case e_vertex_attribute_type_texcoord:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_texcoord);
                    }
                        break;
                    case e_vertex_attribute_type_color:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_color);
                    }
                        break;
                    case e_vertex_attribute_type_extra:
                    {
                        result = (void*)offsetof(vertex_attribute_PTCE, m_extra);
                    }
                        break;
                    default:
                        assert(false);
                        break;
                }
                assert(false);
                return result;
            };
        };
    };
    
    class vertex_declaration : public std::enable_shared_from_this<vertex_declaration>
    {
    private:
        
    protected:
        
        static std::set<uintptr_t> g_guids_container;
        
        std::bitset<e_vertex_attribute_type_max> m_active_attributes;
        
        vertex_attribute* m_data;
        
        i32 m_allocated_size;
        i32 m_used_size;
        
        vertex_declaration(i32 size);
        
    public:
        
        CTTI_CLASS_GUID(vertex_declaration, vertex_declaration::g_guids_container)
        virtual ~vertex_declaration();
        
        std::property_ro<i32> allocated_size;
        std::property_rw<i32> used_size;
        
        std::property_rw<vertex_attribute*> data;
        
        bool is_attribute_active(e_vertex_attribute_type type);
    };
    
    class vertex_declaration_PTC : public vertex_declaration
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(vertex_declaration_PTC, vertex_declaration_PTC::g_guids_container)
        vertex_declaration_PTC(i32 size);
        ~vertex_declaration_PTC();
    };
    
    class vertex_declaration_PTCE : public vertex_declaration
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(vertex_declaration_PTCE, vertex_declaration_PTCE::g_guids_container)
        vertex_declaration_PTCE(i32 size);
        ~vertex_declaration_PTCE();
    };
    
    class vbo final : public resource_transfering_data
    {
    public:
        
    private:
        
    protected:
        
        ui32 m_handle;
        ui32 m_version;
        
        std::shared_ptr<vertex_declaration> m_declaration;
        
        ui32 m_mode;
        
        glm::vec2 m_min_bound;
        glm::vec2 m_max_bound;
        
    public:
        
        vbo(const vertex_declaration_shared_ptr& declaration, ui32 mode);
        ~vbo();
        
        std::property_ro<i32> allocated_size;
        std::property_rw<i32> used_size;
        
        std::property_ro<glm::vec2> min_bound;
        std::property_ro<glm::vec2> max_bound;
        
        ui32 get_id() const;
        ui32 get_version() const;
        
        vertex_attribute* lock() const;
        void unlock(bool is_bathing = false, ui32 size = 0);
        
        void bind(const std::array<i32, e_shader_attribute_max>& attributes) const;
        void unbind(const std::array<i32, e_shader_attribute_max>& attributes) const;
    };
};

