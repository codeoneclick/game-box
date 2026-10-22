#pragma once
#include "main_headers.h"
#include "declarations.h"
#include "ces_base_system.h"
namespace game {
class TerrainEditorInput;
class EditorHandCursor final : public gb::ces_base_system {
    struct implementation;
    std::shared_ptr<implementation> impl_;
    std::bitset<std::numeric_limits<uint8_t>::max()> mask_;
    bool surface_height(float x,float z,float& height,std::string& tag,size_t& tests,const glm::vec2* path_end=nullptr);
protected:
    void on_feed_start(f32) override {}
    void on_feed(const gb::ces_entity_shared_ptr&,f32) override {}
    void on_feed_end(f32) override;
public:
    STTI_CLASS_GUID(EditorHandCursor,gb::ces_base_system::g_guids_container)
    EditorHandCursor(const gb::scene_fabricator_shared_ptr&,const gb::camera_3d_shared_ptr&,const gb::window_impl_shared_ptr&,const std::shared_ptr<TerrainEditorInput>&,std::function<float(float,float)> water_height);
    gb::ces_entity_shared_ptr root() const;
    void update(const TerrainEditorInput&,float dt);
};
}
