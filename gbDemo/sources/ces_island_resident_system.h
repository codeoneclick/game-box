#pragma once
#include "ces_base_system.h"
#include "resident_navigation.h"
#include "resident_behavior.h"
namespace game {
class TerrainEditorState;
class ces_island_resident_system final:public gb::ces_base_system {
    std::vector<resident_navigation> m_navigations;
    std::shared_ptr<TerrainEditorState> m_editor_state;
    std::vector<EditorStairs> m_stairs;
    terrain_config m_stairs_config;
    std::vector<resident_stair_link> m_stair_links;
    std::bitset<std::numeric_limits<uint8_t>::max()> m_mask;
    std::string m_fixed_pose;
    float m_fixed_time=-1;
    bool m_debug=false;
    double m_elapsed=0,m_next_report=0;
    void refresh_residents(const std::vector<uint8_t>& changed);
protected:
    void on_feed_start(f32) override;
    void on_feed(const gb::ces_entity_shared_ptr&,f32) override;
    void on_feed_end(f32) override {}
public:
    STTI_CLASS_GUID(ces_island_resident_system,gb::ces_base_system::g_guids_container)
    explicit ces_island_resident_system(std::vector<resident_navigation>);
    void set_woodcutting_state(const std::shared_ptr<TerrainEditorState>& state){m_editor_state=state;}
    // Apply worker-built CPU navigation on the game-loop thread only.
    void apply_navigation(std::vector<resident_navigation>);
    void apply_stairs(const std::vector<EditorStairs>&,const terrain_config&);
    void refresh_navigation(const std::vector<terrain_mesh_data>& parts,const std::vector<float>& heights,
                            const std::vector<float>& terrace_heights={});
};
}
