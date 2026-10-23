#pragma once
#include "rhythm_prompt_model.h"
#include "scene_graph.h"
namespace game
{
class RhythmPromptView
{
    struct implementation;
    std::shared_ptr<implementation> impl_;

  public:
    explicit RhythmPromptView(const gb::scene_fabricator_shared_ptr &);
    gb::ces_entity_shared_ptr root() const;
    void update(const RhythmPromptState &, glm::vec2 projected_anchor, glm::ivec2 viewport,
                bool enabled);
};
} // namespace game
