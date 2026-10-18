#pragma once
#include "main_headers.h"
#include "declarations.h"
#include <memory>
namespace game {
class TerrainEditorState;
class TerrainEditorInput;
class EditorBuildingRenderer {
    struct implementation;
    std::shared_ptr<implementation> impl_;
public:
    EditorBuildingRenderer(const gb::scene_fabricator_shared_ptr&,const std::shared_ptr<TerrainEditorState>&);
    gb::ces_entity_shared_ptr root() const;
    void set_stairs_preview(const gb::shape_3d_shared_ptr&);
    void update(const TerrainEditorInput&,const glm::mat4& view);
};
}
