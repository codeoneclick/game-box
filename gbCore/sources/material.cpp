//
//  material.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "material.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "resource.h"
#include "texture.h"
#include "shader_loading_operation.h"
#include "texture_loading_operation.h"
#include "blending_configuration.h"
#include "vk_initializers.h"
#include "vk_device.h"
#include "vk_swap_chain.h"
#include "vk_utils.h"

#if USED_GRAPHICS_API == METAL_API

#include "mtl_render_encoder.h"
#include "mtl_render_pipeline_state.h"
#include "mtl_depth_stencil_state.h"
#include "mtl_buffer.h"
#include "mtl_vertex_descriptor.h"

#endif

namespace gb
{
    material_cached_parameters::material_cached_parameters() :
    m_shader(nullptr)
    {
        std::for_each(m_textures.begin(), m_textures.end(), [](std::shared_ptr<texture>& iterator){
            iterator = nullptr;
        });
        
        m_is_depth_test = true;
        m_is_depth_mask = true;
        
        m_is_culling = false;
        m_culling_mode = gl::constant::back;
        
        m_blending_parameters.resize(1, std::make_shared<blending_parameters>());
        
        m_blending_parameters.at(0)->m_is_blending = false;
        m_blending_parameters.at(0)->m_blending_equation = gl::constant::func_add;
        m_blending_parameters.at(0)->m_blending_function_source = gl::constant::src_alpha;
        m_blending_parameters.at(0)->m_blending_function_destination = gl::constant::one_minus_src_alpha;
        
        gl::command::disable(gl::constant::blend);
        gl::command::blend_function(m_blending_parameters.at(0)->m_blending_function_source, m_blending_parameters.at(0)->m_blending_function_destination);
        gl::command::blend_equation(m_blending_parameters.at(0)->m_blending_equation);
        
        m_is_stencil_test = false;
        m_stencil_function = gl::constant::always;
        m_stencil_function_parameter_1 = 1;
        m_stencil_function_parameter_2 = 255;
        m_stencil_mask_parameter = 255;
        
        m_is_color_mask_r = true;
        m_is_color_mask_g = true;
        m_is_color_mask_b = true;
        m_is_color_mask_a = true;
        
        m_is_debugging = false;
        m_is_batching = false;
        
        m_z_order = 0;
    }
    
    std::shared_ptr<material_cached_parameters> material::m_cached_parameters = nullptr;
    std::once_flag g_cached_parameters_created;
    std::shared_ptr<material_cached_parameters> material::get_cached_parameters()
    {
        std::call_once(g_cached_parameters_created, []{
            m_cached_parameters = std::make_shared<material_cached_parameters>();
            gl::command::enable(gl::constant::depth_test);
            gl::command::depth_mask(true);
        });
        return m_cached_parameters;
    }
    
    material::material() :
    m_parameters(std::make_shared<material_cached_parameters>()),
    m_guid("unknown")
    {
        
    }
    
    material::~material()
    {
        
    }
    
    material_shared_ptr material::construct(const std::shared_ptr<material_configuration> &configuration)
    {
        material_shared_ptr material = std::make_shared<gb::material>();
        assert(configuration);
        
        material->m_technique_name = configuration->get_technique_name();
        material->m_technique_pass = configuration->get_technique_pass();
        
        material->set_culling(configuration->get_culling());
        material->set_culling_mode(configuration->get_culling_mode());
        
        const auto blending_configurations = configuration->get_blendings_configurations();
        material->m_parameters->m_blending_parameters.clear();
        material->m_parameters->m_blending_parameters.resize(blending_configurations.size(), nullptr);
        
        ui32 blending_configuration_index = 0;
        for (const auto& blending_configuration_it : blending_configurations)
        {
            const auto blending_configuration = std::static_pointer_cast<gb::blending_configuration>(blending_configuration_it);
            material->m_parameters->m_blending_parameters[blending_configuration_index] = std::make_shared<blending_parameters>();
            material->m_parameters->m_blending_parameters[blending_configuration_index]->m_attachment_index = blending_configuration->get_attachment_index();
            material->set_blending(blending_configuration->get_is_enabled(), blending_configuration_index);
            material->set_blending_function_source(blending_configuration->get_blending_function_source(), blending_configuration_index);
            material->set_blending_function_destination(blending_configuration->get_blending_function_destination(), blending_configuration_index);
            material->set_blending_equation(blending_configuration->get_blending_equation(), blending_configuration_index);
            blending_configuration_index++;
        }
        assert(material->m_parameters->m_blending_parameters.size() > 0);
        
        std::sort(material->m_parameters->m_blending_parameters.begin(), material->m_parameters->m_blending_parameters.end(), [](const std::shared_ptr<blending_parameters>& a, const std::shared_ptr<blending_parameters>& b) -> bool {
            return a->m_attachment_index < b->m_attachment_index;
        });
        
        material->set_stencil_test(configuration->get_stencil_test());
        material->set_stencil_function(configuration->get_stencil_function());
        material->set_stencil_function_parameter_1(configuration->get_stencil_function_parameter_1());
        material->set_stencil_function_parameter_2(configuration->get_stencil_function_parameter_2());
        material->set_stencil_mask_parameter(configuration->get_stencil_mask_parameter());
        
        material->set_depth_test(configuration->get_depth_test());
        material->set_depth_mask(configuration->get_depth_mask());
        
        material->set_debugging(configuration->get_debugging());
        material->set_is_batching(configuration->get_is_batching());
        
        material->set_z_order(configuration->get_z_order());
        
        material->set_color_mask_r(configuration->get_color_mask_r());
        material->set_color_mask_g(configuration->get_color_mask_g());
        material->set_color_mask_b(configuration->get_color_mask_b());
        material->set_color_mask_a(configuration->get_color_mask_a());
        
		material->update_guid();
        
#if USED_GRAPHICS_API == METAL_API
        
        material->m_render_encoder = std::make_shared<mtl_render_encoder>();
        material->m_mvp_uniforms_buffer = std::make_shared<mtl_buffer>(sizeof(shader_mvp_uniforms));
        
#endif

        return material;
    }
    
    void material::update_guid()
    {
        std::stringstream guid_string_stream;
        guid_string_stream<<m_technique_name<<m_technique_pass;
        guid_string_stream<<m_parameters->m_z_order;
        if(m_parameters->m_shader)
        {
            guid_string_stream<<m_parameters->m_shader->get_guid();
        }
        for(const auto& texture : m_parameters->m_textures)
        {
            if(texture)
            {
                guid_string_stream<<texture->get_guid();
            }
        }
        guid_string_stream<<m_parameters->m_stencil_function;
        guid_string_stream<<m_parameters->m_is_stencil_test;
        m_guid = guid_string_stream.str();
    }
    
    const std::string& material::get_guid() const
    {
        return m_guid;
    }
    
    void material::set_shader(const material_shared_ptr& material,
                              const std::shared_ptr<material_configuration>& configuration,
                              const resource_accessor_shared_ptr& resource_accessor, bool force)
    {
        std::shared_ptr<shader_configuration> shader_configuration =
        std::static_pointer_cast<gb::shader_configuration>(configuration->get_shader_configuration());
        assert(shader_configuration != nullptr);
        shader_shared_ptr shader = resource_accessor->get_resource<gb::shader, gb::shader_loading_operation>(shader_configuration->get_filename(), force);
        material->set_shader(shader);
    }
    
    void material::set_textures(const material_shared_ptr& material,
                                const std::shared_ptr<material_configuration>& configuration,
                                const resource_accessor_shared_ptr& resource_accessor, bool force)
    {
        for(const auto& iterator : configuration->get_textures_configurations())
        {
            std::shared_ptr<texture_configuration> texture_configuration =
            std::static_pointer_cast<gb::texture_configuration>(iterator);
            assert(texture_configuration != nullptr);
            
            texture_shared_ptr texture = nullptr;
            std::string texture_filename = texture_configuration->get_texture_filename().length() != 0 ?
            texture_configuration->get_texture_filename() : texture_configuration->get_render_technique_name();
            texture = resource_accessor->get_resource<gb::texture, gb::texture_loading_operation>(texture_filename, force);
            
            assert(texture != nullptr);
            texture->set_wrap_mode(texture_configuration->get_wrap_mode());
            texture->set_mag_filter(texture_configuration->get_mag_filter());
            texture->set_min_filter(texture_configuration->get_min_filter());
            assert(texture_configuration->get_sampler_index() >= 0 &&
                   texture_configuration->get_sampler_index() < e_shader_sampler_max);
            material->set_texture(texture, static_cast<e_shader_sampler>(texture_configuration->get_sampler_index()));
        }
    }
    
    bool material::is_culling() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_culling;
    }
    
    ui32 material::get_culling_mode() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_culling_mode;
    }
    
    bool material::is_blending() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_blending_parameters.at(0)->m_is_blending;
    }
    
    ui32 material::get_blending_function_source() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_blending_parameters.at(0)->m_blending_function_source;
    }
    
    ui32 material::get_blending_function_destination() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_blending_parameters.at(0)->m_blending_function_destination;
    }
    
    ui32 material::get_blending_equation() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_blending_parameters.at(0)->m_blending_equation;
    }
    
    bool material::is_stencil_test() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_stencil_test;
    }
    
    ui32 material::get_stencil_function() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_stencil_function;
    }
    
    i32 material::get_stencil_function_parameter_1() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_stencil_function_parameter_1;
    }
    
    i32 material::get_stencil_function_parameter_2() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_stencil_function_parameter_2;
    }
    
    i32 material::get_stencil_mask_parameter() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_stencil_mask_parameter;
    }
    
    bool material::is_depth_test() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_depth_test;
    }
    
    bool material::is_depth_mask() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_depth_mask;
    }
    
    bool  material::is_color_mask_r() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_color_mask_r;
    }
    
    bool  material::is_color_mask_g() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_color_mask_g;
    }
    
    bool  material::is_color_mask_b() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_color_mask_b;
    }
    
    bool  material::is_color_mask_a() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_color_mask_a;
    }
    
    bool material::is_debugging() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_debugging;
    }
    
    bool material::get_is_batching() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_is_batching;
    }
    
    ui32 material::get_z_order() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_z_order;
    }
    
    shader_shared_ptr material::get_shader() const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_shader;
    }
    
    std::shared_ptr<texture> material::get_texture(e_shader_sampler sampler) const
    {
        assert(m_parameters != nullptr);
        return m_parameters->m_textures.at(sampler);
    }
    
    e_shader_sampler material::get_sampler_index(const std::shared_ptr<texture>& texture) const
    {
        assert(m_parameters != nullptr);
        for(ui32 i = 0; i < m_parameters->m_textures.size(); ++i)
        {
            if(texture == m_parameters->m_textures.at(i))
            {
                return static_cast<e_shader_sampler>(i);
            }
        }
        assert(false);
        return e_shader_sampler_01;
    }
    
    void material::set_culling(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_culling = value;
    }
    
    void material::set_culling_mode(ui32 value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_culling_mode = value;
    }
    
    void material::set_blending(bool value, ui32 attachment_index)
    {
        assert(m_parameters != nullptr);
        assert(m_parameters->m_blending_parameters.size() > attachment_index);
        m_parameters->m_blending_parameters.at(attachment_index)->m_is_blending = value;
    }
    
    void material::set_blending_function_source(ui32 value, ui32 attachment_index)
    {
        assert(m_parameters != nullptr);
        assert(m_parameters->m_blending_parameters.size() > attachment_index);
        m_parameters->m_blending_parameters.at(attachment_index)->m_blending_function_source = value;
    }
    
    void material::set_blending_function_destination(ui32 value, ui32 attachment_index)
    {
        assert(m_parameters != nullptr);
        assert(m_parameters->m_blending_parameters.size() > attachment_index);
        m_parameters->m_blending_parameters.at(attachment_index)->m_blending_function_destination = value;
    }
    
    void material::set_blending_equation(ui32 value, ui32 attachment_index)
    {
        assert(m_parameters);
        assert(m_parameters->m_blending_parameters.size() > attachment_index);
        m_parameters->m_blending_parameters.at(attachment_index)->m_blending_equation = value;
    }
    
    void material::set_stencil_test(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_stencil_test = value;
    }
    
    void material::set_stencil_function(ui32 value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_stencil_function = value;
    }
    
    void material::set_stencil_function_parameter_1(i32 value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_stencil_function_parameter_1 = value;
    }
    
    void material::set_stencil_function_parameter_2(i32 value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_stencil_function_parameter_2 = value;
    }
    
    void material::set_stencil_mask_parameter(i32 value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_stencil_mask_parameter = value;
    }
    
    void material::set_depth_test(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_depth_test = value;
    }
    
    void material::set_depth_mask(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_depth_mask = value;
    }
    
    void material::set_color_mask_r(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_color_mask_r = value;
    }
    
    void material::set_color_mask_g(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_color_mask_g = value;
    }
    
    void material::set_color_mask_b(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_color_mask_b = value;
    }
    
    void material::set_color_mask_a(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_color_mask_a = value;
    }
    
    void material::set_debugging(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_debugging = value;
    }
    
    void material::set_is_batching(bool value)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_is_batching = value;
    }
    
    void material::set_z_order(ui32 z_order)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_z_order = z_order;
    }
    
    void material::set_shader(const shader_shared_ptr& shader)
    {
        assert(m_parameters != nullptr);
        m_parameters->m_shader = shader;
        material::update_guid();
    }
    
    void material::set_texture(const texture_shared_ptr& texture,
                               e_shader_sampler sampler)
    {
        assert(texture != nullptr);
        assert(m_parameters != nullptr);
        m_parameters->m_textures.at(sampler) = texture;
        material::update_guid();
    }
    
    void material::set_custom_shader_uniform(const glm::mat4& matrix, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_mat4);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(matrix);
    }
    
    void material::set_custom_shader_uniform(glm::mat4* matrices, i32 size, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_mat4_array, size);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(matrices, size);
    }

    
    void material::set_custom_shader_uniform(const glm::mat3& matrix, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_mat3);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(matrix);
    }
    
    void material::set_custom_shader_uniform(const glm::vec4& vector, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_vec4);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(vector);
    }
    
    void material::set_custom_shader_uniform(glm::vec4* vectors, i32 size, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_vec4_array, size);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(vectors, size);
    }
    
    void material::set_custom_shader_uniform(const glm::vec3& vector, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_vec3);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(vector);
    }
    
    void material::set_custom_shader_uniform(const glm::vec2& vector, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_vec2);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(vector);
    }
    
    void material::set_custom_shader_uniform(f32 value, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_f32);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(value);
    }
    
    void material::set_custom_shader_uniform(i32 value, const std::string& uniform)
    {
        const auto& iterator = m_custom_shader_uniforms.find(uniform);
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        if(iterator != m_custom_shader_uniforms.end())
        {
            current_uniform = iterator->second;
        }
        else
        {
            current_uniform = std::make_shared<shader_uniform>(e_uniform_type_i32);
            m_custom_shader_uniforms.insert(std::make_pair(uniform, current_uniform));
        }
        current_uniform->set(value);
    }
    
    const std::map<std::string, std::shared_ptr<shader_uniform>>& material::get_custom_uniforms() const
    {
        return m_custom_shader_uniforms;
    }
    
    void material::bind_custom_shader_uniforms()
    {
        std::shared_ptr<shader_uniform> current_uniform = nullptr;
        for(const auto& iterator : m_custom_shader_uniforms)
        {
            current_uniform = iterator.second;
            assert(current_uniform);
            
            switch (current_uniform->get_type())
            {
                case e_uniform_type_mat4:
                {
                    m_parameters->m_shader->set_custom_mat4(current_uniform->get_mat4(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_mat4_array:
                {
                    m_parameters->m_shader->set_custom_mat4_array(current_uniform->get_mat4_array(),
                                                                  current_uniform->get_array_size(),
                                                                  iterator.first);
                }
                    break;
                    
                case e_uniform_type_mat3:
                {
                    m_parameters->m_shader->set_custom_mat3(current_uniform->get_mat3(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec4:
                {
                    m_parameters->m_shader->set_custom_vec4(current_uniform->get_vec4(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec4_array:
                {
                    m_parameters->m_shader->set_custom_vec4_array(current_uniform->get_vec4_array(),
                                                                  current_uniform->get_array_size(),
                                                                  iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec3:
                {
                    m_parameters->m_shader->set_custom_vec3(current_uniform->get_vec3(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_vec2:
                {
                    m_parameters->m_shader->set_custom_vec2(current_uniform->get_vec2(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_f32:
                {
                    m_parameters->m_shader->set_custom_f32(current_uniform->get_f32(), iterator.first);
                }
                    break;
                    
                case e_uniform_type_i32:
                {
                    m_parameters->m_shader->set_custom_i32(current_uniform->get_i32(), iterator.first);
                }
                    break;
                    
                default:
                {
                    assert(false);
                }
                    break;
            }
        }
    }
    
#if USED_GRAPHICS_API == VULKAN_API

	void material::bind(const VkPipelineVertexInputStateCreateInfo& vertex_input_state)

#elif USED_GRAPHICS_API == METAL_API
    
    void material::bind(const mtl_vertex_descriptor_shared_ptr& vertex_descriptor)
    
#else

	void material::bind()

#endif

    {
        assert(m_parameters != nullptr);
        assert(m_parameters->m_shader != nullptr);
        
#if USED_GRAPHICS_API == VULKAN_API

		if (!m_is_pipeline_constructed)
		{
			construct_pipeline(vertex_input_state);
		}
        
#elif USED_GRAPHICS_API == METAL_API
        
        if (!m_render_pipeline_state)
        {
            m_render_pipeline_state = std::make_shared<mtl_render_pipeline_state>(m_parameters,
                                                                                  vertex_descriptor);
        }
        
        if (!m_depth_stencil_state)
        {
            m_depth_stencil_state = std::make_shared<mtl_depth_stencil_state>(m_parameters);
        }
        
        m_render_encoder->bind(m_technique_name);
        m_render_encoder->set_render_pipeline_state(m_render_pipeline_state);
        m_render_encoder->set_depth_stencil_state(m_depth_stencil_state);
        
        if (m_parameters->m_is_culling)
        {
            m_render_encoder->set_cull_mode(m_parameters->m_culling_mode);
        }
        else
        {
            m_render_encoder->set_cull_mode_none();
        }

#endif
        
        m_parameters->m_shader->bind();
        
        for(ui32 i = 0; i < e_shader_sampler_max; ++i)
        {
            if(m_parameters->m_textures[i] != nullptr)
            {
                m_parameters->m_shader->set_texture(m_parameters->m_textures[i], static_cast<e_shader_sampler>(i));
                
#if USED_GRAPHICS_API == METAL_API
                
                if (m_parameters->m_textures[i]->get_mtl_texture_id())
                {
                    m_render_encoder->set_texture(m_parameters->m_textures[i]->get_mtl_texture_id(), i);
                }
                
#endif
                
            }
        }
        
        if(m_parameters->m_is_depth_test &&
           material::get_cached_parameters()->m_is_depth_test != m_parameters->m_is_depth_test)
        {
            gl::command::enable(gl::constant::depth_test);
            gl::command::depth_function(gl::constant::lequal);
            material::get_cached_parameters()->m_is_depth_test = m_parameters->m_is_depth_test;
        }
        else if(material::get_cached_parameters()->m_is_depth_test != m_parameters->m_is_depth_test)
        {
            gl::command::disable(gl::constant::depth_test);
            material::get_cached_parameters()->m_is_depth_test = m_parameters->m_is_depth_test;
        }
        
        if(m_parameters->m_is_depth_mask &&
           material::get_cached_parameters()->m_is_depth_mask != m_parameters->m_is_depth_mask)
        {
            gl::command::depth_mask(gl::constant::yes);
            material::get_cached_parameters()->m_is_depth_mask = m_parameters->m_is_depth_mask;
        }
        else if(material::get_cached_parameters()->m_is_depth_mask != m_parameters->m_is_depth_mask)
        {
            gl::command::depth_mask(gl::constant::no);
            material::get_cached_parameters()->m_is_depth_mask = m_parameters->m_is_depth_mask;
        }
        
        if(m_parameters->m_is_culling &&
           material::get_cached_parameters()->m_is_culling != m_parameters->m_is_culling)
        {
            gl::command::enable(gl::constant::cull_face);
            material::get_cached_parameters()->m_is_culling = m_parameters->m_is_culling;
        }
        else if(material::get_cached_parameters()->m_is_culling != m_parameters->m_is_culling)
        {
            gl::command::disable(gl::constant::cull_face);
            material::get_cached_parameters()->m_is_culling = m_parameters->m_is_culling;
        }
        
        if(material::get_cached_parameters()->m_culling_mode != m_parameters->m_culling_mode)
        {
            gl::command::cull_face(m_parameters->m_culling_mode);
            material::get_cached_parameters()->m_culling_mode = m_parameters->m_culling_mode;
        }
        
        if(m_parameters->m_blending_parameters.at(0)->m_is_blending &&
           material::get_cached_parameters()->m_blending_parameters.at(0)->m_is_blending != m_parameters->m_blending_parameters.at(0)->m_is_blending)
        {
            gl::command::enable(gl::constant::blend);
            material::get_cached_parameters()->m_blending_parameters.at(0)->m_is_blending = m_parameters->m_blending_parameters.at(0)->m_is_blending;
        }
        else if(material::get_cached_parameters()->m_blending_parameters.at(0)->m_is_blending != m_parameters->m_blending_parameters.at(0)->m_is_blending)
        {
            gl::command::disable(gl::constant::blend);
            material::get_cached_parameters()->m_blending_parameters.at(0)->m_is_blending = m_parameters->m_blending_parameters.at(0)->m_is_blending;
        }
        
        if(material::get_cached_parameters()->m_blending_parameters.at(0)->m_blending_function_source !=
           m_parameters->m_blending_parameters.at(0)->m_blending_function_source ||
           material::get_cached_parameters()->m_blending_parameters.at(0)->m_blending_function_destination !=
           m_parameters->m_blending_parameters.at(0)->m_blending_function_destination)
        {
            gl::command::blend_function(m_parameters->m_blending_parameters.at(0)->m_blending_function_source, m_parameters->m_blending_parameters.at(0)->m_blending_function_destination);
            material::get_cached_parameters()->m_blending_parameters.at(0)->m_blending_function_source = m_parameters->m_blending_parameters.at(0)->m_blending_function_source;
            material::get_cached_parameters()->m_blending_parameters.at(0)->m_blending_function_destination = m_parameters->m_blending_parameters.at(0)->m_blending_function_destination;
        }
        
        if(material::get_cached_parameters()->m_blending_parameters.at(0)->m_blending_equation != m_parameters->m_blending_parameters.at(0)->m_blending_equation)
        {
            gl::command::blend_equation(m_parameters->m_blending_parameters.at(0)->m_blending_equation);
            material::get_cached_parameters()->m_blending_parameters.at(0)->m_blending_equation = m_parameters->m_blending_parameters.at(0)->m_blending_equation;
        }
        
        if(m_parameters->m_is_stencil_test &&
           material::get_cached_parameters()->m_is_stencil_test != m_parameters->m_is_stencil_test)
        {
            gl::command::enable(gl::constant::stencil_test);
            material::get_cached_parameters()->m_is_stencil_test = m_parameters->m_is_stencil_test;
        }
        else if(material::get_cached_parameters()->m_is_stencil_test != m_parameters->m_is_stencil_test)
        {
            gl::command::disable(gl::constant::stencil_test);
            material::get_cached_parameters()->m_is_stencil_test = m_parameters->m_is_stencil_test;
        }
        
        if(m_parameters->m_is_color_mask_r != material::get_cached_parameters()->m_is_color_mask_r ||
           m_parameters->m_is_color_mask_g != material::get_cached_parameters()->m_is_color_mask_g ||
           m_parameters->m_is_color_mask_b != material::get_cached_parameters()->m_is_color_mask_b ||
           m_parameters->m_is_color_mask_a != material::get_cached_parameters()->m_is_color_mask_a)
        {
            material::get_cached_parameters()->m_is_color_mask_r = m_parameters->m_is_color_mask_r;
            material::get_cached_parameters()->m_is_color_mask_g = m_parameters->m_is_color_mask_g;
            material::get_cached_parameters()->m_is_color_mask_b = m_parameters->m_is_color_mask_b;
            material::get_cached_parameters()->m_is_color_mask_a = m_parameters->m_is_color_mask_a;
            
            gl::command::color_mask(m_parameters->m_is_color_mask_r, m_parameters->m_is_color_mask_g,
                          m_parameters->m_is_color_mask_b, m_parameters->m_is_color_mask_a);
        }
        
        if(material::get_cached_parameters()->m_stencil_function != m_parameters->m_stencil_function ||
           material::get_cached_parameters()->m_stencil_function_parameter_1 != m_parameters->m_stencil_function_parameter_1 ||
           material::get_cached_parameters()->m_stencil_function_parameter_2 != m_parameters->m_stencil_function_parameter_2)
        {
            gl::command::stencil_function(m_parameters->m_stencil_function, m_parameters->m_stencil_function_parameter_1,
                                m_parameters->m_stencil_function_parameter_2);
            
            material::get_cached_parameters()->m_stencil_function = m_parameters->m_stencil_function;
            material::get_cached_parameters()->m_stencil_function_parameter_1 = m_parameters->m_stencil_function_parameter_1;
            material::get_cached_parameters()->m_stencil_function_parameter_2 = m_parameters->m_stencil_function_parameter_2;
        }
        
        if(material::get_cached_parameters()->m_stencil_mask_parameter != m_parameters->m_stencil_mask_parameter)
        {
            gl::command::stencil_mask(m_parameters->m_stencil_mask_parameter);
            material::get_cached_parameters()->m_stencil_mask_parameter = m_parameters->m_stencil_mask_parameter;
        }
        material::bind_custom_shader_uniforms();

#if USED_GRAPHICS_API == VULKAN_API

		ui32 current_image_index = vk_device::get_instance()->get_current_image_index();
		VkCommandBuffer draw_cmd_buffer = vk_device::get_instance()->get_draw_cmd_buffer(current_image_index);

		vkCmdBindPipeline(draw_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
		const auto descriptor_set = m_parameters->m_shader->get_descriptor_set();
		vkCmdBindDescriptorSets(draw_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_parameters->m_shader->get_pipeline_layout(), 0, 1, &descriptor_set, 0, nullptr);

#endif

    }
    
    void material::unbind()
    {
        assert(m_parameters != nullptr);
        assert(m_parameters->m_shader != nullptr);
        m_parameters->m_shader->unbind();
        
#if USED_GRAPHICS_API == METAL_API
        
        m_render_encoder->unbind(m_technique_name);
        
#endif
        
    }

#if USED_GRAPHICS_API == VULKAN_API

	void material::construct_pipeline(const VkPipelineVertexInputStateCreateInfo& vertex_input_state)
	{
		m_input_assembly_state = vk_initializers::pipeline_input_assembly_state_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);
		m_rasterization_state = vk_initializers::pipeline_rasterization_state_create_info(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);
		m_color_blend_attachment = vk_initializers::pipeline_color_blend_attachment_state(0xf, VK_FALSE);
		m_color_blend_state = vk_initializers::pipeline_color_blend_state_create_info(1, &m_color_blend_attachment);
		m_depth_stencil_state = vk_initializers::pipeline_depth_stencil_state_create_info(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

		VkViewport viewport = vk_initializers::viewport(vk_swap_chain::get_instance()->get_swap_chain_extent().width, vk_swap_chain::get_instance()->get_swap_chain_extent().height, 0.f, 1.f);
		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = vk_swap_chain::get_instance()->get_swap_chain_extent();

		m_viewport_state = vk_initializers::pipeline_viewport_state_create_info(1, 1, 0);
		m_viewport_state.pViewports = &viewport;
		m_viewport_state.pScissors = &scissor;

		m_multisampling_state = vk_initializers::pipeline_multisample_state_create_info(VK_SAMPLE_COUNT_1_BIT, 0);

		VkPipelineShaderStageCreateInfo shader_stages[] = { m_parameters->m_shader->get_vs_shader_stage(), m_parameters->m_shader->get_fs_shader_stage() };

		m_graphics_pipeline = vk_initializers::pipeline_create_info(m_parameters->m_shader->get_pipeline_layout(), vk_swap_chain::get_instance()->get_render_pass(), 0);
		m_graphics_pipeline.stageCount = 2;
		m_graphics_pipeline.pStages = shader_stages;
		m_graphics_pipeline.pVertexInputState = &vertex_input_state;
		m_graphics_pipeline.pInputAssemblyState = &m_input_assembly_state;
		m_graphics_pipeline.pViewportState = &m_viewport_state;
		m_graphics_pipeline.pRasterizationState = &m_rasterization_state;
		m_graphics_pipeline.pMultisampleState = &m_multisampling_state;
		m_graphics_pipeline.pColorBlendState = &m_color_blend_state;
		m_graphics_pipeline.pDepthStencilState = &m_depth_stencil_state;
		m_graphics_pipeline.subpass = 0;
		m_graphics_pipeline.basePipelineHandle = VK_NULL_HANDLE;

		VK_CHECK(vkCreateGraphicsPipelines(vk_device::get_instance()->get_logical_device(), VK_NULL_HANDLE, 1, &m_graphics_pipeline, nullptr, &m_pipeline));


		//VkDescriptorBufferInfo descriptor_buffer_info = {};
		//descriptor_buffer_info.buffer = uniformBuffer;
		//descriptor_buffer_info.offset = 0;
		//descriptor_buffer_info.range = sizeof(vk_buffer);

		m_is_pipeline_constructed = true;
	}

#endif
    
#if USED_GRAPHICS_API == METAL_API
    
    mtl_render_encoder_shared_ptr material::get_render_encoder() const
    {
        return m_render_encoder;
    }
    
    mtl_buffer_shared_ptr material::get_mvp_uniforms_buffer() const
    {
        return m_mvp_uniforms_buffer;
    }
    
    mtl_buffer_shared_ptr material::get_custom_uniform_buffer(ui32 size)
    {
        if (!m_custom_uniforms_buffer)
        {
            m_custom_uniforms_buffer = std::make_shared<mtl_buffer>(size);
        }
        assert(m_custom_uniforms_buffer->get_size() == size);
        return m_custom_uniforms_buffer;
    }
    
#endif

}

#endif
