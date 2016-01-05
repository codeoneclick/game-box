//
//  shader.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "shader.h"
#include "shader_compiler_glsl.h"
#include "resource_status.h"
#include "texture.h"

namespace gb
{
    static ui32 g_shader_id = 0;
    
    extern const struct attribute_names
    {
        std::string m_position;
        std::string m_texcoord;
        std::string m_normal;
        std::string m_tangent;
        std::string m_color;
        std::string m_extra;
        
    } attributes_names;
    
    extern const struct uniform_names
    {
        std::string m_mat_m;
        std::string m_mat_v;
        std::string m_mat_p;
        std::string m_mat_n;
        std::string m_mat_i_vp;
        std::string m_mat_bones;
        std::string m_vec_camera_position;
        std::string m_vec_global_light_position;
        std::string m_mat_global_light_p;
        std::string m_mat_global_light_v;
        std::string m_vec_clip;
        std::string m_f32_camera_near;
        std::string m_f32_camera_far;
        std::string m_f32_timer;
        std::string m_i32_flag_01;
        std::string m_i32_flag_02;
        std::string m_i32_flag_03;
        std::string m_i32_flag_04;
        
    } uniform_names;
    
    extern const struct sampler_names
    {
        std::string m_sampler_01;
        std::string m_sampler_02;
        std::string m_sampler_03;
        std::string m_sampler_04;
        std::string m_sampler_05;
        std::string m_sampler_06;
        std::string m_sampler_07;
        std::string m_sampler_08;
        
    } sampler_names;
    
    const struct attribute_names attribute_names =
    {
        "a_position",
        "a_texcoord",
        "a_normal",
        "a_tangent",
        "a_color",
        "a_extra"
    };
    
    const struct uniform_names uniform_names =
    {
        "u_mat_m",
        "u_mat_v",
        "u_mat_p",
        "u_mat_n",
        "u_mat_i_vp",
        "u_mat_bones",
        "u_vec_camera_position",
        "u_vec_global_light_position",
        "u_mat_global_light_p",
        "u_mat_global_light_v",
        "u_vec_clip",
        "u_f32_camera_near",
        "u_f32_camera_far",
        "u_f32_timer",
        "u_i32_flag_01",
        "u_i32_flag_02",
        "u_i32_flag_03",
        "u_i32_flag_04"
    };
    
    const struct sampler_names sampler_names =
    {
        "sampler_01",
        "sampler_02",
        "sampler_03",
        "sampler_04",
        "sampler_05",
        "sampler_06",
        "sampler_07",
        "sampler_08"
    };
    
    shader_uniform::shader_uniform(e_uniform_type type) :
    m_type(type),
    m_mat4_value(0.f),
    m_mat4_array(nullptr),
    m_mat3_value(0.f),
    m_mat3_array(nullptr),
    m_vec4_value(0.f),
    m_vec4_array(nullptr),
    m_vec3_value(0.f),
    m_vec3_array(nullptr),
    m_vec2_value(0.f),
    m_vec2_array(nullptr),
    m_f32_value(0.f),
    m_f32_array(nullptr),
    m_i32_value(0),
    m_i32_array(nullptr),
    m_sampler_value(e_shader_sampler_01),
    m_texture_value(nullptr),
    m_array_size(0)
    {
        
    }
    
    shader_uniform::~shader_uniform(void)
    {
        
    }
    
    e_uniform_type shader_uniform::get_type(void) const
    {
        return m_type;
    }
    
    void shader_uniform::set_mat4(const glm::mat4& matrix)
    {
        assert(m_type == e_uniform_type_mat4);
        m_mat4_value = matrix;
    }
    
    void shader_uniform::set_mat4_array(glm::mat4 *matrices, i32 size)
    {
        assert(m_type == e_uniform_type_mat4_array);
        m_mat4_array = matrices;
        m_array_size = size;
    }
    
    void shader_uniform::set_mat3(const glm::mat3& matrix)
    {
        assert(m_type == e_uniform_type_mat3);
        m_mat3_value = matrix;
    }
    
    void shader_uniform::set_mat3_array(glm::mat3 *matrices, i32 size)
    {
        assert(m_type == e_uniform_type_mat3_array);
        m_mat3_array = matrices;
        m_array_size = size;
    }
    
    void shader_uniform::set_vec4(const glm::vec4& vector)
    {
        assert(m_type == e_uniform_type_vec4);
        m_vec4_value = vector;
    }
    
    void shader_uniform::set_vec4_array(glm::vec4 *vectors, i32 size)
    {
        assert(m_type = e_uniform_type_vec4_array);
        m_vec4_array = vectors;
        m_array_size = size;
    }
    
    void shader_uniform::set_vec3(const glm::vec3& vector)
    {
        assert(m_type == e_uniform_type_vec3);
        m_vec3_value = vector;
    }
    
    void shader_uniform::set_vec3_array(glm::vec3 *vectors, i32 size)
    {
        assert(m_type = e_uniform_type_vec3_array);
        m_vec3_array = vectors;
        m_array_size = size;
    }
    
    void shader_uniform::set_vec2(const glm::vec2& vector)
    {
        assert(m_type == e_uniform_type_vec2);
        m_vec2_value = vector;
    }
    
    void shader_uniform::set_vec2_array(glm::vec2 *vectors, i32 size)
    {
        assert(m_type = e_uniform_type_vec2_array);
        m_vec2_array = vectors;
        m_array_size = size;
    }
    
    void shader_uniform::set_f32(f32 value)
    {
        assert(m_type == e_uniform_type_f32);
        m_f32_value = value;
    }
    
    void shader_uniform::set_f32_array(f32 *values, i32 size)
    {
        assert(m_type = e_uniform_type_f32_array);
        m_f32_array = values;
        m_array_size = size;
    }
    
    void shader_uniform::set_i32(i32 value)
    {
        assert(m_type == e_uniform_type_i32);
        m_i32_value = value;
    }
    
    void shader_uniform::set_i32_array(i32* values, i32 size)
    {
        assert(m_type = e_uniform_type_f32_array);
        m_i32_array = values;
        m_array_size = size;
    }
    
    void shader_uniform::set_sampler(const std::shared_ptr<texture> &texture, gb::e_shader_sampler sampler)
    {
        assert(m_type == e_uniform_type_sampler);
        m_texture_value = texture;
        m_sampler_value = sampler;
    }
    
    const glm::mat4& shader_uniform::get_mat4() const
    {
        assert(m_type == e_uniform_type_mat4);
        return m_mat4_value;
    }
    
    const glm::mat4* shader_uniform::get_mat4_array() const
    {
        assert(m_type == e_uniform_type_mat4_array);
        return m_mat4_array;
    }
    
    const glm::mat3& shader_uniform::get_mat3() const
    {
        assert(m_type == e_uniform_type_mat3);
        return m_mat3_value;
    }
    
    const glm::mat3* shader_uniform::get_mat3_array() const
    {
        assert(m_type == e_uniform_type_mat3_array);
        return m_mat3_array;
    }
    
    const glm::vec4& shader_uniform::get_vec4() const
    {
        assert(m_type == e_uniform_type_vec4);
        return m_vec4_value;
    }
    
    const glm::vec4* shader_uniform::get_vec4_array() const
    {
        assert(m_type == e_uniform_type_vec4_array);
        return m_vec4_array;
    }
    
    const glm::vec3& shader_uniform::get_vec3() const
    {
        assert(m_type == e_uniform_type_vec3);
        return m_vec3_value;
    }
    
    const glm::vec3* shader_uniform::get_vec3_array() const
    {
        assert(m_type == e_uniform_type_vec3_array);
        return m_vec3_array;
    }
    
    const glm::vec2& shader_uniform::get_vec2() const
    {
        assert(m_type == e_uniform_type_vec2);
        return m_vec2_value;
    }
    
    const glm::vec2* shader_uniform::get_vec2_array() const
    {
        assert(m_type == e_uniform_type_vec2_array);
        return m_vec2_array;
    }
    
    f32 shader_uniform::get_f32() const
    {
        assert(m_type == e_uniform_type_f32);
        return m_f32_value;
    }
    
    f32* shader_uniform::get_f32_array() const
    {
        assert(m_type == e_uniform_type_f32_array);
        return m_f32_array;
    }
    
    i32 shader_uniform::get_i32() const
    {
        assert(m_type == e_uniform_type_i32);
        return m_i32_value;
    }
    
    i32* shader_uniform::get_i32_array() const
    {
        assert(m_type == e_uniform_type_i32_array);
        return m_i32_array;
    }
    
    e_shader_sampler shader_uniform::get_sampler() const
    {
        assert(m_type == e_uniform_type_sampler);
        return m_sampler_value;
    }
    
    std::shared_ptr<texture> shader_uniform::get_texture() const
    {
        assert(m_type == e_uniform_type_sampler);
        return m_texture_value;
    }
    
    i32 shader_uniform::get_array_size() const
    {
        return m_array_size;
    }
    
    shader_transfering_data::shader_transfering_data() :
    m_shader_id(0),
    m_vs_filename(""),
    m_fs_filename(""),
    m_vs_source_code(""),
    m_fs_source_code("")
    {
        m_type = e_resource_transfering_data_type_shader;
    }
    
    shader::shader(const std::string& guid) :
    gb::resource(e_resource_type_shader, guid),
    m_data(nullptr)
    {
        m_attributes[e_shader_attribute_position] = -1;
        m_attributes[e_shader_attribute_texcoord] = -1;
        m_attributes[e_shader_attribute_normal] = -1;
        m_attributes[e_shader_attribute_tangent] = -1;
        m_attributes[e_shader_attribute_color] = -1;
        m_attributes[e_shader_attribute_extra] = -1;
    }
    
    std::shared_ptr<shader> shader::construct(const std::string& guid,
                                              const std::string& vs_source_code,
                                              const std::string& fs_source_code)
    {
        std::shared_ptr<shader> shader = std::make_shared<gb::shader>(guid);
        
        std::string out_message = "";
        bool out_success = false;
        ui32 vs_handle = shader_compiler_glsl::compile(vs_source_code, GL_VERTEX_SHADER, &out_message, &out_success);
        if(!out_success)
        {
            std::cout<<out_message<<std::endl;
            return nullptr;
        }
        
        ui32 fs_handle = shader_compiler_glsl::compile(fs_source_code, GL_FRAGMENT_SHADER, &out_message, &out_success);
        if(!out_success)
        {
            std::cout<<out_message<<std::endl;
            return nullptr;
        }
        
        ui32 shader_id = shader_compiler_glsl::link(vs_handle, fs_handle, &out_message, &out_success);
        if(!out_success)
        {
            std::cout<<out_message<<std::endl;
            return nullptr;
        }
        
        shader->m_data = std::make_shared<shader_transfering_data>();
        shader->m_data->m_shader_id = shader_id;
        shader->m_data->m_vs_source_code = vs_source_code;
        shader->m_data->m_fs_source_code = fs_source_code;
        assert(shader->m_data->m_shader_id != 0);
        shader->setup_uniforms();
        
        shader->m_status |= e_resource_status_loaded;
        shader->m_status |= e_resource_status_commited;
        return shader;
    }
    
    shader::~shader(void)
    {
        glDeleteProgram(m_data->m_shader_id);
    }
    
    void shader::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_shader:
            {
                m_data = std::static_pointer_cast<shader_transfering_data>(data);
                m_status |= e_resource_status_loaded;
            }
                break;
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    void shader::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_shader:
            {
                m_data->m_shader_id = std::static_pointer_cast<shader_transfering_data>(data)->m_shader_id;
                shader::setup_uniforms();
                m_status |= e_resource_status_commited;
            }
                break;
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    void shader::setup_uniforms(void)
    {
        m_uniforms[e_shader_uniform_mat_m] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_m.c_str());
        m_uniforms[e_shader_uniform_mat_v] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_v.c_str());
        m_uniforms[e_shader_uniform_mat_p] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_p.c_str());
        m_uniforms[e_shader_uniform_mat_n] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_n.c_str());
        m_uniforms[e_shader_uniform_mat_i_vp] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_i_vp.c_str());
        m_uniforms[e_shader_uniform_mat_bones] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_bones.c_str());
        m_uniforms[e_shader_uniform_vec_camera_position] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_vec_camera_position.c_str());
        m_uniforms[e_shader_uniform_f32_camera_near] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_f32_camera_near.c_str());
        m_uniforms[e_shader_uniform_f32_camera_far] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_f32_camera_far.c_str());
        m_uniforms[e_shader_uniform_vec_clip] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_vec_clip.c_str());
        m_uniforms[e_shader_uniform_vec_global_light_position] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_vec_global_light_position.c_str());
        m_uniforms[e_shader_uniform_mat_global_light_p] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_global_light_p.c_str());
        m_uniforms[e_shader_uniform_mat_global_light_v] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_mat_global_light_v.c_str());
        m_uniforms[e_shader_uniform_f32_timer] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_f32_timer.c_str());
        
        m_uniforms[e_shader_uniform_i32_flag_01] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_i32_flag_01.c_str());
        m_uniforms[e_shader_uniform_i32_flag_02] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_i32_flag_02.c_str());
        m_uniforms[e_shader_uniform_i32_flag_03] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_i32_flag_03.c_str());
        m_uniforms[e_shader_uniform_i32_flag_04] = gl_get_uniform_location(m_data->m_shader_id, uniform_names.m_i32_flag_04.c_str());
        
        m_samplers[e_shader_sampler_01] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_01.c_str());
        m_samplers[e_shader_sampler_02] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_02.c_str());
        m_samplers[e_shader_sampler_03] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_03.c_str());
        m_samplers[e_shader_sampler_04] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_04.c_str());
        m_samplers[e_shader_sampler_05] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_05.c_str());
        m_samplers[e_shader_sampler_06] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_06.c_str());
        m_samplers[e_shader_sampler_07] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_07.c_str());
        m_samplers[e_shader_sampler_08] = gl_get_uniform_location(m_data->m_shader_id, sampler_names.m_sampler_08.c_str());
        
        m_attributes.at(e_shader_attribute_position) = gl_get_attribute_location(m_data->m_shader_id, attribute_names.m_position.c_str());
        m_attributes.at(e_shader_attribute_texcoord) = gl_get_attribute_location(m_data->m_shader_id, attribute_names.m_texcoord.c_str());
        m_attributes.at(e_shader_attribute_normal) = gl_get_attribute_location(m_data->m_shader_id, attribute_names.m_normal.c_str());
        m_attributes.at(e_shader_attribute_tangent) = gl_get_attribute_location(m_data->m_shader_id, attribute_names.m_tangent.c_str());
        m_attributes.at(e_shader_attribute_color) = gl_get_attribute_location(m_data->m_shader_id, attribute_names.m_color.c_str());
        m_attributes.at(e_shader_attribute_extra) = gl_get_attribute_location(m_data->m_shader_id, attribute_names.m_extra.c_str());
        
        for(ui32 i = 0; i < e_shader_uniform_max + e_shader_sampler_max; ++i)
        {
            m_cached_uniform[i] = nullptr;
        }
    }
    
    std::string shader::get_vs_filename(void) const
    {
        return resource::is_loaded() ? m_data->m_vs_filename : "";
    }
    
    std::string shader::get_fs_filename(void) const
    {
        return resource::is_loaded() ? m_data->m_fs_filename : "";
    }
    
    std::string shader::get_vs_source_code(void) const
    {
        return resource::is_loaded() ? m_data->m_vs_source_code : "";
    }
    
    std::string shader::get_fs_source_code(void) const
    {
        return resource::is_loaded() ? m_data->m_fs_source_code : "";
    }
    
    const std::array<i32, e_shader_attribute_max>& shader::get_attributes(void) const
    {
        return m_attributes;
    }
    
    i32 shader::get_custom_uniform(const std::string& uniform)
    {
        i32 handle = -1;
        const auto iterator = m_custom_uniforms.find(uniform);
        if(iterator != m_custom_uniforms.end())
        {
            handle = iterator->second;
        }
        else
        {
            handle = gl_get_uniform_location(m_data->m_shader_id, uniform.c_str());
            m_custom_uniforms.insert(std::make_pair(uniform, handle));
        }
        return handle;
    }
    
    void shader::set_mat3(const glm::mat3 &matrix, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            if(m_cached_uniform[uniform] != nullptr && m_cached_uniform[uniform]->get_mat3() == matrix)
            {
                return;
            }
            else if(m_cached_uniform[uniform] == nullptr)
            {
                m_cached_uniform[uniform] = std::make_shared<shader_uniform>(e_uniform_type_mat3);
            }
            
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_matrix_3fv(handle, 1, 0, &matrix[0][0]);
            m_cached_uniform[uniform]->set_mat3(matrix);
        }
    }
    
    void shader::set_custom_mat3(const glm::mat3x3 &matrix, const std::string &uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_matrix_3fv(shader::get_custom_uniform(uniform), 1, 0, &matrix[0][0]);
        }
    }
    
    void shader::set_mat4(const glm::mat4x4 &matrix, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            if(m_cached_uniform[uniform] != nullptr && m_cached_uniform[uniform]->get_mat4() == matrix)
            {
                return;
            }
            else if(m_cached_uniform[uniform] == nullptr)
            {
                m_cached_uniform[uniform] = std::make_shared<shader_uniform>(e_uniform_type_mat4);
            }
            
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_matrix_4fv(handle, 1, 0, &matrix[0][0]);
            m_cached_uniform[uniform]->set_mat4(matrix);
        }
    }
    
    void shader::set_custom_mat4(const glm::mat4x4 &matrix, const std::string &uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_matrix_4fv(shader::get_custom_uniform(uniform), 1, 0, &matrix[0][0]);
        }
    }
    
    void shader::set_mat4_array(const glm::mat4x4* matrix, ui32 size, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_matrix_4fv(handle, size, 0, &matrix[0][0][0]);
        }
    }
    
    void shader::set_custom_mat4_array(const glm::mat4x4* matrix, ui32 size, const std::string& uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_matrix_4fv(shader::get_custom_uniform(uniform), size, 0, &matrix[0][0][0]);
        }
    }
    
    void shader::set_vec2(const glm::vec2 &vector, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            if(m_cached_uniform[uniform] != nullptr && m_cached_uniform[uniform]->get_vec2() == vector)
            {
                return;
            }
            else if(m_cached_uniform[uniform] == nullptr)
            {
                m_cached_uniform[uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec2);
            }
            
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_vector_2fv(handle, 1, &vector[0]);
            m_cached_uniform[uniform]->set_vec2(vector);
        }
    }
    
    void shader::set_custom_vec2(const glm::vec2 &vector, const std::string &uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_vector_2fv(shader::get_custom_uniform(uniform), 1, &vector[0]);
        }
    }
    
    void shader::set_custom_vec2_array(const glm::vec2* vectors, ui32 size, const std::string& uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_vector_2fv(shader::get_custom_uniform(uniform), size, &vectors[0][0]);
        }
    }
    
    void shader::set_vec3(const glm::vec3 &vector, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            if(m_cached_uniform[uniform] != nullptr && m_cached_uniform[uniform]->get_vec3() == vector)
            {
                return;
            }
            else if(m_cached_uniform[uniform] == nullptr)
            {
                m_cached_uniform[uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec3);
            }
            
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_vector_3fv(handle, 1, &vector[0]);
            m_cached_uniform[uniform]->set_vec3(vector);
        }
    }
    
    void shader::set_custom_vec3(const glm::vec3 &vector, const std::string &uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_vector_3fv(shader::get_custom_uniform(uniform), 1, &vector[0]);
        }
    }
    
    void shader::set_custom_vec3_array(const glm::vec3* vectors, ui32 size, const std::string& uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_vector_3fv(shader::get_custom_uniform(uniform), size, &vectors[0][0]);
        }
    }
    
    void shader::set_vec4(const glm::vec4 &vector, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            if(m_cached_uniform[uniform] != nullptr && m_cached_uniform[uniform]->get_vec4() == vector)
            {
                return;
            }
            else if(m_cached_uniform[uniform] == nullptr)
            {
                m_cached_uniform[uniform] = std::make_shared<shader_uniform>(e_uniform_type_vec4);
            }
            
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_vector_4fv(handle, 1, &vector[0]);
            m_cached_uniform[uniform]->set_vec4(vector);
        }
    }
    
    void shader::set_custom_vec4(const glm::vec4 &vector, const std::string &uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_vector_4fv(shader::get_custom_uniform(uniform), 1, &vector[0]);
        }
    }
    
    void shader::set_custom_vec4_array(const glm::vec4* vectors, ui32 size, const std::string& uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_vector_4fv(shader::get_custom_uniform(uniform), size, &vectors[0][0]);
        }
    }
    
    void shader::set_f32(f32 value, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            if(m_cached_uniform[uniform] != nullptr && m_cached_uniform[uniform]->get_f32() == value)
            {
                return;
            }
            else if(m_cached_uniform[uniform] == nullptr)
            {
                m_cached_uniform[uniform] = std::make_shared<shader_uniform>(e_uniform_type_f32);
            }
            
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_1f(handle, value);
            m_cached_uniform[uniform]->set_f32(value);
        }
    }
    
    void shader::set_custom_f32(f32 value, const std::string &uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_1f(shader::get_custom_uniform(uniform), value);
        }
    }
    
    void shader::set_i32(i32 value, e_shader_uniform uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            if(m_cached_uniform[uniform] != nullptr && m_cached_uniform[uniform]->get_i32() == value)
            {
                return;
            }
            else if(m_cached_uniform[uniform] == nullptr)
            {
                m_cached_uniform[uniform] = std::make_shared<shader_uniform>(e_uniform_type_i32);
            }
            
            i32 handle = m_uniforms[uniform];
            gl_get_uniform_1i(handle, value);
            m_cached_uniform[uniform]->set_i32(value);
        }
    }
    
    void shader::set_custom_i32(i32 value, const std::string &uniform)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_get_uniform_1i(shader::get_custom_uniform(uniform), value);
        }
    }
    
    void shader::set_texture(const std::shared_ptr<texture> &texture, gb::e_shader_sampler sampler)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            assert(sampler < e_shader_sampler_max);
            gl_set_active_texture(GL_TEXTURE0 + sampler);
            texture->bind();
            gl_get_uniform_1i(m_samplers[sampler], sampler);
        }
    }
    
    void shader::bind(void) const
    {
        if(resource::is_loaded() && resource::is_commited() && g_shader_id != m_data->m_shader_id)
        {
            g_shader_id = m_data->m_shader_id;
            gl_use_program(m_data->m_shader_id);
        }
    }
    
    void shader::unbind(void) const
    {
        
    }
    
    i32 shader::get_custom_attribute(const std::string& attribute_name)
    {
        i32 attribute = gl_get_attribute_location(m_data->m_shader_id, attribute_name.c_str());
        if(attribute != -1)
        {
            m_custom_attributes.insert(std::make_pair(attribute_name, attribute));
        }
        return attribute;
    }
    
    const std::unordered_map<std::string, i32>& shader::get_custom_attributes() const
    {
        return m_custom_attributes;
    }
    
    bool shader::is_custom_attributes_exist() const
    {
        return m_custom_attributes.size() != 0;
    }
}