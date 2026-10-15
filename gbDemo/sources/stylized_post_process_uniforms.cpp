#include "stylized_post_process_uniforms.h"
#include "shader.h"
namespace game {
stylized_post_process_uniforms::stylized_post_process_uniforms(
    gb::ces_render_technique_uniforms_component::e_shader_uniform_type type)
    : shader_uniforms(type) {
  for (const auto &name :
       {"post_flags", "post_color", "post_shadow", "post_highlight",
        "post_bloom", "post_fog", "post_ranges", "post_dof", "post_extra",
        "post_camera", "post_display"}) {
    m_uniforms[name] =
        std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
    set(glm::vec4(0), name);
  }
  for (const auto &name : {"post_view", "post_projection"}) {
    m_uniforms[name] =
        std::make_shared<gb::shader_uniform>(gb::e_uniform_type_mat4);
    set(glm::mat4(1), name);
  }
  update(StylizedPostProcessSettings::desktop(), glm::vec3(0),
         glm::vec3(0, 0, -1), glm::mat4(1), glm::mat4(1), 0, false);
  get_values();
}
void stylized_post_process_uniforms::update(
    const StylizedPostProcessSettings &input, const glm::vec3 &camera,
    const glm::vec3 &look_at, const glm::mat4 &view,
    const glm::mat4 &projection, float water_level, bool manual_gamma) {
  auto s = input;
  s.sanitize();
  const int stage = s.implementationStage;
  set(glm::vec4(s.enableHDR ? 1.f : 0.f,
                s.enableToneMapping && stage >= 4 ? 1.f : 0.f,
                s.enableColorGrading && stage >= 5 ? 1.f : 0.f,
                0.f),
      "post_flags");
  set(glm::vec4(s.exposure, s.saturation, s.contrast, s.temperature),
      "post_color");
  set(glm::vec4(s.shadowTint[0], s.shadowTint[1], s.shadowTint[2], s.tint),
      "post_shadow");
  set(glm::vec4(s.highlightTint[0], s.highlightTint[1], s.highlightTint[2],
                s.buildingMidtoneLift),
      "post_highlight");
  set(glm::vec4(s.bloomThreshold, s.bloomStrength, s.bloomDivisor, s.debugView),
      "post_bloom");
  set(glm::vec4(s.fogColor[0], s.fogColor[1], s.fogColor[2], s.fogStrength),
      "post_fog");
  set(glm::vec4(s.fogStart, s.fogEnd, s.enableFog && stage >= 7 ? 1.f : 0.f,
                0.f),
      "post_ranges");
  set(glm::vec4(s.focusDistance, s.focusRange, s.nearBlur, s.farBlur),
      "post_dof");
  set(glm::vec4(0.f, 0.f,
                std::isfinite(water_level) ? water_level : 0.f, stage),
      "post_extra");
  glm::vec3 safe_camera = camera;
  if (!std::isfinite(camera.x) || !std::isfinite(camera.y) ||
      !std::isfinite(camera.z))
    safe_camera = glm::vec3(0);
  const float distance = glm::length(look_at - safe_camera);
  const float focus = std::isfinite(distance) ? std::max(.001f, distance) : 1.f;
  set(glm::vec4(safe_camera, focus), "post_camera");
  set(glm::vec4(manual_gamma ? 1.f : 0.f, 0, 0, 0), "post_display");
  set(view, "post_view");
  set(projection, "post_projection");
}
void *stylized_post_process_uniforms::get_values() {
  m_values.post_flags = m_uniforms["post_flags"]->get_vec4();
  m_values.post_color = m_uniforms["post_color"]->get_vec4();
  m_values.post_shadow = m_uniforms["post_shadow"]->get_vec4();
  m_values.post_highlight = m_uniforms["post_highlight"]->get_vec4();
  m_values.post_bloom = m_uniforms["post_bloom"]->get_vec4();
  m_values.post_fog = m_uniforms["post_fog"]->get_vec4();
  m_values.post_ranges = m_uniforms["post_ranges"]->get_vec4();
  m_values.post_dof = m_uniforms["post_dof"]->get_vec4();
  m_values.post_extra = m_uniforms["post_extra"]->get_vec4();
  m_values.post_camera = m_uniforms["post_camera"]->get_vec4();
  m_values.post_display = m_uniforms["post_display"]->get_vec4();
  m_values.post_view = m_uniforms["post_view"]->get_mat4();
  m_values.post_projection = m_uniforms["post_projection"]->get_mat4();
  return &m_values;
}
} // namespace game
