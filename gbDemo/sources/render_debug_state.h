#pragma once
#include "main_headers.h"
#include "declarations.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>
namespace game {
struct RenderDebugAttachment {
    std::string name;
    gb::texture_shared_ptr texture;
    unsigned width=0,height=0;
    int mode=0; // 0 HDR color, 1 encoded color/normal, 2 position/depth, 3 field, 4 scalar.
};
struct RenderDebugPass {
    std::string id,label;
    bool enabled=true;
    uint64_t triangles=0;
    std::vector<RenderDebugAttachment> attachments;
};
// UI sees a snapshot and commands, never renderer-owned passes or generators.
struct RenderDebugState {
    std::string terrain_diagnostics;
    std::vector<RenderDebugPass> passes;
    std::function<void(const std::string&,bool)> set_enabled;
};
}
