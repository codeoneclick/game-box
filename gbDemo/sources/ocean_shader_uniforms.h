#pragma once
#include "ces_shader_uniforms_component.h"
#include "ces_render_technique_uniforms_component.h"
#include "ocean_wave_model.h"
#include "stylized_lighting_uniforms.h"
namespace game {
    class ocean_shader_uniforms final : public gb::ces_shader_uniforms_component::shader_uniforms
    {
        struct values {
            glm::vec4 waves[4],motions[4],time_level,camera,deep,shallow,foam,params,sun,extra;
            glm::vec4 foam_bands,foam_detail,foam_shadow,optics,caustic_params,interaction,foam_shape;
            glm::mat4 view,projection;
            StylizedLightingUniformValues lighting;
        } m_values;
    public:
        ocean_shader_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode,const std::string& name);
        void update(const ocean_settings&,float seconds,const glm::vec3& camera,const glm::mat4& view,const glm::mat4& projection,float wetness,int debug_mode);
        void* get_values() override;
        ui32 get_values_size() override;
    };
    class ocean_field_debug_uniforms final : public gb::ces_render_technique_uniforms_component::shader_uniforms {
        struct values {glm::vec4 debug;glm::mat4 view,projection;glm::vec4 camera,time_level,sun;} m_values;
    public:
        explicit ocean_field_debug_uniforms(gb::ces_render_technique_uniforms_component::e_shader_uniform_type type);
        void* get_values() override;
        ui32 get_values_size() override {static_assert(sizeof(values)==192,"Atmosphere compose uniform layout");return sizeof(values);}
    };
}
