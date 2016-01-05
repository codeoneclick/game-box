//
//  material.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef material_h
#define material_h

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
        GLenum m_culling_mode;
        
        bool m_is_blending;
        GLenum m_blending_function_source;
        GLenum m_blending_function_destination;
        
        bool m_is_stencil_test;
        GLenum m_stencil_function;
        i32 m_stencil_function_parameter_1;
        i32 m_stencil_function_parameter_2;
        i32 m_stencil_mask_parameter;
        
        bool m_is_depth_test;
        bool m_is_depth_mask;
    
        bool m_is_debugging;
        
        shader_shared_ptr m_shader;
        std::array<texture_shared_ptr, e_shader_sampler_max> m_textures;
    };
    
    class material
    {
    private:
        
    protected:
        
        std::map<std::string, std::shared_ptr<shader_uniform>> m_custom_shader_uniforms;
        std::shared_ptr<material_cached_parameters> m_parameters;
        
        std::string m_guid;
        
        static std::shared_ptr<material_cached_parameters> m_cached_parameters;
        
    public:
        
        material();
        ~material();
        
        static material_shared_ptr construct(const std::shared_ptr<material_configuration>& configuration);
        
        static std::shared_ptr<material_cached_parameters> get_cached_parameters();
        
        static void set_shader(const material_shared_ptr& material,
                               const std::shared_ptr<material_configuration>& configuration,
                               const resource_accessor_shared_ptr& resource_accessor);
        
        static void set_textures(const material_shared_ptr& material,
                                 const std::shared_ptr<material_configuration>& configuration,
                                 const resource_accessor_shared_ptr& resource_accessor);
        
        bool is_culling() const;
        GLenum get_culling_mode(void) const;
        
        bool is_blending() const;
        GLenum get_blending_function_source() const;
        GLenum get_blending_function_destination() const;
        
        bool is_stencil_test() const;
        GLenum get_stencil_function() const;
        i32 get_stencil_function_parameter_1() const;
        i32 get_stencil_function_parameter_2() const;
        i32 get_stencil_mask_parameter() const;
        
        bool is_depth_test() const;
        bool is_depth_mask() const;
        
        bool is_debugging() const;
        
        const std::string& get_guid() const;
        
        shader_shared_ptr get_shader() const;
        texture_shared_ptr get_texture(e_shader_sampler sampler) const;
        e_shader_sampler get_sampler_index(const texture_shared_ptr& texture) const;
        
        void set_culling(bool value);
        void set_culling_mode(GLenum value);
        
        void set_blending(bool value);
        void set_blending_function_source(GLenum value);
        void set_blending_function_destination(GLenum value);
        
        void set_stencil_test(bool value);
        void set_stencil_function(GLenum value);
        void set_stencil_function_parameter_1(i32 value);
        void set_stencil_function_parameter_2(i32 value);
        void set_stencil_mask_parameter(i32 value);
        
        void set_depth_test(bool value);
        void set_depth_mask(bool value);
        
        void set_debugging(bool value);
        
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
        
        const std::map<std::string, std::shared_ptr<shader_uniform>>& get_custom_uniforms(void) const;
        
        void bind();
        void unbind();
    };
};

#endif
