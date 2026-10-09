#include "ocean_shader_uniforms.h"
#include "shader.h"
namespace game {
    ocean_shader_uniforms::ocean_shader_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode,const std::string& name):shader_uniforms(mode,name)
    {
        for(const auto& key:{"wave0","wave1","wave2","wave3","motion0","motion1","motion2","motion3","time_level","camera","deep","shallow","foam","params","sun","extra","foam_bands","foam_detail","foam_shadow","optics","caustic_params","interaction","foam_shape"})
            m_uniforms[key]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
        m_uniforms["view"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
        m_uniforms["projection"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
    }
    void ocean_shader_uniforms::update(const ocean_settings& s,float t,const glm::vec3& camera,const glm::mat4& v,const glm::mat4& p,float wetness,int debug)
    {
        for(int i=0;i<4;++i) {
            const auto& w=s.waves[i]; const auto d=glm::normalize(glm::vec2(w.dx,w.dz));
            set(glm::vec4(d,w.wavelength,w.amplitude),"wave"+std::to_string(i));
            set(glm::vec4(w.steepness,w.speed,0,0),"motion"+std::to_string(i));
        }
        set(glm::vec4(t,s.mean_level,0,0),"time_level");set(glm::vec4(camera,1),"camera");
        set(glm::vec4(s.deep_color[0],s.deep_color[1],s.deep_color[2],1),"deep");
        set(glm::vec4(s.shallow_color[0],s.shallow_color[1],s.shallow_color[2],1),"shallow");
        set(glm::vec4(s.foam_color[0],s.foam_color[1],s.foam_color[2],1),"foam");
        set(glm::vec4(s.foam_width,s.absorption,s.refraction_strength,s.highlight_strength),"params");
        set(glm::vec4(s.foam_inner_distance,s.foam_width,s.foam_noise_scale,s.foam_noise_speed),"foam_bands");
        set(glm::vec4(s.foam_sharpness,s.foam_outer_strength,s.crest_foam_strength,0),"foam_detail");
        set(glm::vec4(s.foam_shadow_color[0],s.foam_shadow_color[1],s.foam_shadow_color[2],s.facet_strength),"foam_shadow");
        set(glm::vec4(s.depth_fade_distance,s.refraction_depth_distance,s.fresnel_power,s.reflection_strength),"optics");
        set(glm::vec4(s.caustic_scale,s.caustic_speed,s.caustic_strength,s.caustic_depth_falloff),"caustic_params");
        set(glm::vec4(-160,-160,320,1.f/1024),"interaction");
        set(glm::vec4(s.foam_mid_min_width,s.foam_mid_max_width,s.foam_warp_strength,s.foam_height_offset),"foam_shape");
        set(glm::vec4(s.sun_direction.x,s.sun_direction.y,s.sun_direction.z,0),"sun");
        set(glm::vec4(debug,wetness,0,0),"extra");set(v,"view");set(p,"projection");
    }
    void* ocean_shader_uniforms::get_values()
    {
        for(int i=0;i<4;++i) {m_values.waves[i]=m_uniforms["wave"+std::to_string(i)]->get_vec4();m_values.motions[i]=m_uniforms["motion"+std::to_string(i)]->get_vec4();}
        m_values.time_level=m_uniforms["time_level"]->get_vec4();m_values.camera=m_uniforms["camera"]->get_vec4();
        m_values.deep=m_uniforms["deep"]->get_vec4();m_values.shallow=m_uniforms["shallow"]->get_vec4();m_values.foam=m_uniforms["foam"]->get_vec4();
        m_values.params=m_uniforms["params"]->get_vec4();m_values.sun=m_uniforms["sun"]->get_vec4();m_values.extra=m_uniforms["extra"]->get_vec4();
        m_values.foam_bands=m_uniforms["foam_bands"]->get_vec4();m_values.foam_detail=m_uniforms["foam_detail"]->get_vec4();
        m_values.foam_shadow=m_uniforms["foam_shadow"]->get_vec4();m_values.optics=m_uniforms["optics"]->get_vec4();m_values.caustic_params=m_uniforms["caustic_params"]->get_vec4();
        m_values.interaction=m_uniforms["interaction"]->get_vec4();m_values.foam_shape=m_uniforms["foam_shape"]->get_vec4();
        m_values.view=m_uniforms["view"]->get_mat4();m_values.projection=m_uniforms["projection"]->get_mat4();return &m_values;
    }
    ui32 ocean_shader_uniforms::get_values_size() {static_assert(sizeof(values)==496,"Ocean uniform layout");return sizeof(values);}
    ocean_field_debug_uniforms::ocean_field_debug_uniforms(gb::ces_render_technique_uniforms_component::e_shader_uniform_type type):shader_uniforms(type){
        m_uniforms["field_debug"]=std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);set(glm::vec4(0),"field_debug");
    }
    void* ocean_field_debug_uniforms::get_values(){m_value=m_uniforms["field_debug"]->get_vec4();return &m_value;}
}
