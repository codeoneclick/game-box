//
//  ss_output_render_technique_uniforms.h
//  gbDemo
//
//  Created by serhii.s on 4/23/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_render_technique_uniforms_component.h"
#include "shader.h"

class ss_output_shader_uniforms : public gb::ces_render_technique_uniforms_component::shader_uniforms
{
private:
    
protected:
    
    struct values
    {
        f32 m_vignetting_edge_size;
    } __attribute__ ((aligned(256)));
    
    values m_values;
    
public:
    
    ss_output_shader_uniforms(gb::ces_render_technique_uniforms_component::e_shader_uniform_type type) :  gb::ces_render_technique_uniforms_component::shader_uniforms(type)
    {
        m_uniforms["vignetting_edge_size"] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_f32);
    }
    
    void* get_values() override
    {
        m_values.m_vignetting_edge_size = m_uniforms["vignetting_edge_size"]->get_f32();
        return static_cast<void*>(&m_values);
    }
    
    ui32 get_values_size() override
    {
        return sizeof(ss_output_shader_uniforms::values);
    }
};

class ss_tv_shader_uniforms : public gb::ces_render_technique_uniforms_component::shader_uniforms
{
private:
    
protected:
    
    struct values
    {
        f32 m_enabled;
        f32 m_time;
    } __attribute__ ((aligned(256)));
    
    values m_values;
    
public:
    
    ss_tv_shader_uniforms(gb::ces_render_technique_uniforms_component::e_shader_uniform_type type) :  gb::ces_render_technique_uniforms_component::shader_uniforms(type)
    {
        m_uniforms["enabled"] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_f32);
        m_uniforms["time"] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_f32);
    }
    
    void* get_values() override
    {
        m_values.m_enabled = m_uniforms["enabled"]->get_f32();
        m_values.m_time = m_uniforms["time"]->get_f32();
        return static_cast<void*>(&m_values);
    }
    
    ui32 get_values_size() override
    {
        return sizeof(ss_tv_shader_uniforms::values);
    }
};

class ss_cross_fade_shader_uniforms : public gb::ces_render_technique_uniforms_component::shader_uniforms
{
private:
    
protected:
    
    struct values
    {
        f32 m_enabled;
        f32 m_progress;
    } __attribute__ ((aligned(256)));
    
    values m_values;
    
public:
    
    ss_cross_fade_shader_uniforms(gb::ces_render_technique_uniforms_component::e_shader_uniform_type type) :  gb::ces_render_technique_uniforms_component::shader_uniforms(type)
    {
        m_uniforms["enabled"] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_f32);
        m_uniforms["progress"] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_f32);
    }
    
    void* get_values() override
    {
        m_values.m_enabled = m_uniforms["enabled"]->get_f32();
        m_values.m_progress = m_uniforms["progress"]->get_f32();
        return static_cast<void*>(&m_values);
    }
    
    ui32 get_values_size() override
    {
        return sizeof(ss_cross_fade_shader_uniforms::values);
    }
};
