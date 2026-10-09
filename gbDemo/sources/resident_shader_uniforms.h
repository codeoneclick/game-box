#pragma once
#include "ces_shader_uniforms_component.h"
namespace game {
class resident_bone_uniforms final:public gb::ces_shader_uniforms_component::shader_uniforms {
    glm::mat4 m_bones[64];
public:
    resident_bone_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode,const std::string&);
    void update(const glm::mat4*,int count);
    void* get_values() override{return m_bones;}
    ui32 get_values_size() override{return sizeof(m_bones);}
};
class resident_color_uniforms final:public gb::ces_shader_uniforms_component::shader_uniforms {
    struct values {glm::mat4 view;glm::vec4 clothing_color,sun;} m_values;
public:
    resident_color_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode,const std::string&);
    void update(const glm::mat4&,const glm::vec4&);
    void* get_values() override{return &m_values;}
    ui32 get_values_size() override{return sizeof(m_values);}
};
}
