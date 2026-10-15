#pragma once
#include "ces_shader_uniforms_component.h"
#include "stylized_lighting_uniforms.h"
namespace game {
class resident_bone_uniforms final:public gb::ces_shader_uniforms_component::shader_uniforms {
    glm::mat4 m_bones[64];
    int m_previous_count=0;
public:
    resident_bone_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode,const std::string&);
    void update(const glm::mat4*,int count,float ground_height=0.f);
    void* get_values() override{return m_bones;}
    ui32 get_values_size() override{return sizeof(m_bones);}
};
class resident_color_uniforms final:public gb::ces_shader_uniforms_component::shader_uniforms {
    struct values {glm::mat4 view;glm::vec4 clothing_color,sun;StylizedLightingUniformValues lighting;} m_values;
    float m_outline_id=8000000.f;
public:
    resident_color_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode,const std::string&);
    void update(const glm::mat4&,const glm::vec4&,const glm::vec3& camera=glm::vec3(0));
    void set_outline_id(float id){m_outline_id=id;}
    void* get_values() override{return &m_values;}
    ui32 get_values_size() override{return sizeof(m_values);}
};
}
