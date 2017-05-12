//
//  shader.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef shader_h
#define shader_h

#include "resource.h"

namespace gb
{
    enum e_uniform_type
    {
        e_uniform_type_mat4 = 0,
        e_uniform_type_mat4_array,
        e_uniform_type_mat3,
        e_uniform_type_mat3_array,
        e_uniform_type_vec4,
        e_uniform_type_vec4_array,
        e_uniform_type_vec3,
        e_uniform_type_vec3_array,
        e_uniform_type_vec2,
        e_uniform_type_vec2_array,
        e_uniform_type_f32,
        e_uniform_type_f32_array,
        e_uniform_type_i32,
        e_uniform_type_i32_array,
        e_uniform_type_sampler
    };
    
    enum e_shader_sampler
    {
        e_shader_sampler_01 = 0,
        e_shader_sampler_02,
        e_shader_sampler_03,
        e_shader_sampler_04,
        e_shader_sampler_05,
        e_shader_sampler_06,
        e_shader_sampler_07,
        e_shader_sampler_08,
        e_shader_sampler_max
    };
    
    enum e_shader_attribute
    {
        e_shader_attribute_position = 0,
        e_shader_attribute_texcoord,
        e_shader_attribute_normal,
        e_shader_attribute_tangent,
        e_shader_attribute_color,
        e_shader_attribute_extra,
        e_shader_attribute_max
    };
    
    enum e_shader_uniform
    {
        e_shader_uniform_mat_m = 0,
        e_shader_uniform_mat_p,
        e_shader_uniform_mat_v,
        e_shader_uniform_max
    };
    
    class shader_uniform
    {
    private:
        
        e_uniform_type m_type;
        
        glm::mat4  m_mat4_value;
        glm::mat4* m_mat4_array;
        glm::mat3  m_mat3_value;
        glm::mat3* m_mat3_array;
        glm::vec4  m_vec4_value;
        glm::vec4* m_vec4_array;
        glm::vec3  m_vec3_value;
        glm::vec3* m_vec3_array;
        glm::vec2  m_vec2_value;
        glm::vec2* m_vec2_array;
        f32  m_f32_value;
        f32* m_f32_array;
        i32  m_i32_value;
        i32* m_i32_array;
        
        i32 m_array_size;
        
        e_shader_sampler m_sampler_value;
        texture_shared_ptr m_texture_value;
        
    protected:
        
    public:
        
        shader_uniform(e_uniform_type type);
        
        ~shader_uniform();
        
        e_uniform_type get_type() const;
        
        void set_mat4(const glm::mat4& matrix);
        void set_mat4_array(glm::mat4* matrices, i32 size);
        void set_mat3(const glm::mat3& matrix);
        void set_mat3_array(glm::mat3* matrices, i32 size);
        void set_vec4(const glm::vec4& vector);
        void set_vec4_array(glm::vec4* vectors, i32 size);
        void set_vec3(const glm::vec3& vector);
        void set_vec3_array(glm::vec3* vectors, i32 size);
        void set_vec2(const glm::vec2& vector);
        void set_vec2_array(glm::vec2* vectors, i32 size);
        void set_f32(f32 value);
        void set_f32_array(f32* values, i32 size);
        void set_i32(i32 value);
        void set_i32_array(i32* values, i32 size);
        void set_sampler(const texture_shared_ptr& texture, e_shader_sampler sampler);
        
        const glm::mat4& get_mat4() const;
        const glm::mat4* get_mat4_array() const;
        const glm::mat3& get_mat3() const;
        const glm::mat3* get_mat3_array() const;
        const glm::vec4& get_vec4() const;
        const glm::vec4* get_vec4_array() const;
        const glm::vec3& get_vec3() const;
        const glm::vec3* get_vec3_array() const;
        const glm::vec2& get_vec2() const;
        const glm::vec2* get_vec2_array() const;
        f32 get_f32() const;
        f32* get_f32_array() const;
        i32 get_i32() const;
        i32* get_i32_array() const;
        e_shader_sampler get_sampler() const;
        texture_shared_ptr get_texture() const;
        
        i32 get_array_size() const;
    };
    
    struct shader_transfering_data : public resource_transfering_data
    {
        std::string m_vs_source_code;
        std::string m_fs_source_code;
        ui32 m_shader_id;
        
        shader_transfering_data();
        ~shader_transfering_data() = default;
    };
    
    class shader : public resource
    {
    public:
        
    private:
        
    protected:
        
        ui32 m_shader_id;
        
        i32 m_uniforms[gb::e_shader_uniform_max];
        i32 m_samplers[gb::e_shader_sampler_max];
        std::array<i32, e_shader_attribute_max> m_attributes;
        
        std::unordered_map<std::string, i32> m_custom_uniforms;
        std::unordered_map<std::string, i32> m_custom_attributes;
        
        std::vector<std::shared_ptr<shader_uniform>> m_cached_uniform;
        
        void on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data>& data);
        void on_transfering_data_commited(const std::shared_ptr<resource_transfering_data>& data);
        
        void setup_uniforms();
        i32 get_custom_uniform(const std::string& uniform);
        
    public:
        
        shader(const std::string& guid);
        
        static shader_shared_ptr construct(const std::string& guid,
                                           const std::string& vs_source_code,
                                           const std::string& fs_source_code);
        
        ~shader();
        
        const std::array<i32, e_shader_attribute_max>& get_attributes() const;
        
        void set_mat3(const glm::mat3& matrix, e_shader_uniform uniform);
        void set_custom_mat3(const glm::mat3& matrix, const std::string& uniform);
        void set_mat4(const glm::mat4& matrix, e_shader_uniform uniform);
        void set_custom_mat4(const glm::mat4& matrix, const std::string& uniform);
        void set_mat4_array(const glm::mat4* matrix, ui32 size, e_shader_uniform uniform);
        void set_custom_mat4_array(const glm::mat4* matrices, ui32 size, const std::string& uniform);
        void set_vec2(const glm::vec2& vector, e_shader_uniform uniform);
        void set_custom_vec2(const glm::vec2& vector, const std::string& uniform);
        void set_custom_vec2_array(const glm::vec2* vectors, ui32 size, const std::string& uniform);
        void set_vec3(const glm::vec3& vector, e_shader_uniform uniform);
        void set_custom_vec3(const glm::vec3& vector, const std::string& uniform);
        void set_custom_vec3_array(const glm::vec3* vectors, ui32 size, const std::string& uniform);
        void set_vec4(const glm::vec4& vector, e_shader_uniform uniform);
        void set_custom_vec4(const glm::vec4& vector, const std::string& uniform);
        void set_custom_vec4_array(const glm::vec4* vectors, ui32 size, const std::string& uniform);
        void set_f32(f32 value, e_shader_uniform uniform);
        void set_custom_f32(f32 value, const std::string& uniform);
        void set_i32(i32 value, e_shader_uniform uniform);
        void set_custom_i32(i32 value, const std::string& uniform);
        void set_texture(const texture_shared_ptr& texture, e_shader_sampler sampler);
        
        i32 get_custom_attribute(const std::string& attribute_name);
        const std::unordered_map<std::string, i32>& get_custom_attributes() const;
        bool is_custom_attributes_exist() const;
        
        void bind() const;
        void unbind() const;
    };
};

#endif
