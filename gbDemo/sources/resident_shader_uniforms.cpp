#include "resident_shader_uniforms.h"
#include <stdexcept>
namespace game {
resident_bone_uniforms::resident_bone_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode,const std::string& name):shader_uniforms(mode,name){
    for(auto& b:m_bones)b=glm::mat4(1);
    m_uniforms["u_bones"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4_array,64);
    m_uniforms["u_bones"]->set(m_bones,64);
}
void resident_bone_uniforms::update(const glm::mat4* bones,int count){
    if(!bones||count<1||count>64)throw std::runtime_error("Invalid resident bone palette");
    for(int i=0;i<64;++i)m_bones[i]=i<count?bones[i]:glm::mat4(1);
    m_uniforms["u_bones"]->set(m_bones,64);
}
resident_color_uniforms::resident_color_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode,const std::string& name):shader_uniforms(mode,name){
    m_uniforms["view"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
    m_uniforms["clothing_color"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
    m_uniforms["sun"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
    update(glm::mat4(1),glm::vec4(.055f,.23f,.43f,1));
}
void resident_color_uniforms::update(const glm::mat4& view,const glm::vec4& color){
    static_assert(sizeof(values)==96,"Resident fragment uniform layout");
    m_values={view,color,glm::vec4(.35f,.8f,.45f,0)};
    set(view,"view");set(color,"clothing_color");set(m_values.sun,"sun");
}
}
