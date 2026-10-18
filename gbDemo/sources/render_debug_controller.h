#pragma once
#include "render_debug_state.h"
namespace game {
struct StylizedPostProcessSettings;
class RenderDebugController {
    struct implementation;
    std::shared_ptr<implementation> impl_;
public:
    RenderDebugController(const gb::scene_fabricator_shared_ptr&,
                          const std::shared_ptr<gb::render_pipeline>&,
                          const std::shared_ptr<StylizedPostProcessSettings>&);
    std::shared_ptr<RenderDebugState> state() const;
    void update();
};
}
