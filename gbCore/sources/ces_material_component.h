//
//  ces_material_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "material.h"
#include "shader.h"

namespace gb
{
    class ces_material_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::unordered_map<std::string, std::unordered_map<i32, material_shared_ptr>> m_materials;
        
        void bind_custom_shader_uniforms(const material_shared_ptr& material);

        std::function<void(const material_shared_ptr&)> m_bind_material_imposer_callback;
        
    public:
        
        STTI_CLASS_GUID(ces_material_component, ces_base_component::g_guids_container)
        ces_material_component();
        ~ces_material_component();
        
        void add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material);
        void remove_material(const std::string& technique_name, i32 technique_pass);
        
        void set_is_batching(bool value, const std::string& technique_name = "", i32 technique_pass = -1);
        bool get_is_batching(const std::string& technique_name, i32 technique_pass) const;
        bool get_is_batching() const;
        
        material_shared_ptr get_material(const std::string& technique_name, i32 technique_pass) const;
        
#if USED_GRAPHICS_API == VULKAN_API

		void on_bind(const std::string& technique_name, i32 technique_pass,
                     const VkPipelineVertexInputStateCreateInfo& vertex_input_state,
			const material_shared_ptr& material = nullptr);
        
#elif USED_GRAPHICS_API == METAL_API
        
        void on_bind(const std::string& technique_name, i32 technique_pass,
                     const mtl_vertex_descriptor_shared_ptr& vertex_descriptor,
                     const material_shared_ptr& material = nullptr);
#else

		void on_bind(const std::string& technique_name, i32 technique_pass,
			const material_shared_ptr& material = nullptr);

#endif
        
        void on_unbind(const std::string& technique_name, i32 technique_pass,
                       const material_shared_ptr& material = nullptr);
        
        void set_bind_material_imposer_callback(const std::function<void(const material_shared_ptr&)>& callback);
        
        void set_texture(const texture_shared_ptr& texture, e_shader_sampler sampler,
                         const std::string& technique_name = "", i32 technique_pass = -1);
        
        template<typename T_VALUE>
        void set_custom_shader_uniform(T_VALUE value, const std::string& uniform,
                                       const std::string& technique_name = "", i32 technique_pass = -1);
        
        template<typename T_VALUE>
        void set_custom_shader_uniform_array(T_VALUE array, i32 size, const std::string& uniform,
                                             const std::string& technique_name = "", i32 technique_pass = -1);
        
        void set_custom_shader_uniforms(const std::unordered_map<std::string, std::shared_ptr<shader_uniform>>& uniforms,
                                        const std::string& technique_name = "", i32 technique_pass = -1);
    };
};

#include "ces_material_component.hpp"

#endif
