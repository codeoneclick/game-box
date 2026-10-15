#pragma once
#include "ces_shader_uniforms_component.h"
#include "stylized_lighting_settings.h"

namespace game {
struct StylizedLightingUniformValues {
    glm::vec4 light_sun, light_ambient, light_shape, light_options;
    glm::vec4 light_material0, light_material1, light_camera;
};
inline StylizedLightingUniformValues make_lighting_uniforms(const glm::vec3& camera){
    const auto& s=stylized_lighting_settings();
    return {glm::vec4(s.sunColor[0],s.sunColor[1],s.sunColor[2],s.sunIntensity*s.daylight()),
        glm::vec4(s.ambientColor[0],s.ambientColor[1],s.ambientColor[2],s.ambientStrength),
        glm::vec4(s.wrapAmount,s.diffusePower,float(s.mode),s.globalSpecularMultiplier),
        glm::vec4(s.vegetationBacklightStrength,s.enableStylizedSpecular?1.f:0.f,s.enableVegetationBacklight?1.f:0.f,s.daylight()),
        glm::vec4(s.grassSpecular,s.woodSpecular,s.stoneSpecular,s.leavesSpecular),
        glm::vec4(s.plasterSpecular,s.metalSpecular,s.specularPower,0),glm::vec4(camera,0)};
}
inline const std::array<const char*,7>& lighting_uniform_names(){
    static const std::array<const char*,7> names={{"light_sun","light_ambient","light_shape","light_options","light_material0","light_material1","light_camera"}};return names;
}
inline void set_lighting_uniforms(gb::ces_shader_uniforms_component::shader_uniforms& uniforms,const StylizedLightingUniformValues& values){
    static_assert(sizeof(StylizedLightingUniformValues)==112,"Lighting uniform ABI");
    uniforms.set(values.light_sun,"light_sun");uniforms.set(values.light_ambient,"light_ambient");
    uniforms.set(values.light_shape,"light_shape");uniforms.set(values.light_options,"light_options");
    uniforms.set(values.light_material0,"light_material0");uniforms.set(values.light_material1,"light_material1");
    uniforms.set(values.light_camera,"light_camera");
}
}
