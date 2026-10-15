#pragma once
#include "ces_render_technique_uniforms_component.h"
#include "stylized_post_process.h"
namespace game {
class stylized_post_process_uniforms final
    : public gb::ces_render_technique_uniforms_component::shader_uniforms {
  struct values {
    glm::vec4 post_flags, post_color, post_shadow, post_highlight, post_bloom;
    glm::vec4 post_fog, post_ranges, post_dof, post_extra, post_camera,
        post_display;
    glm::mat4 post_view, post_projection;
  } m_values;

public:
  explicit stylized_post_process_uniforms(
      gb::ces_render_technique_uniforms_component::e_shader_uniform_type type);
  void update(const StylizedPostProcessSettings &, const glm::vec3 &camera,
              const glm::vec3 &look_at, const glm::mat4 &view,
              const glm::mat4 &projection, float water_level,
              bool manual_gamma);
  void *get_values() override;
  ui32 get_values_size() override {
    static_assert(sizeof(values) == 304,
                  "Stylized post-process uniform layout");
    return sizeof(values);
  }
};
} // namespace game
