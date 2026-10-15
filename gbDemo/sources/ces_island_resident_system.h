#pragma once
#include "ces_base_system.h"
#include "resident_navigation.h"
namespace game {
class ces_island_resident_system final:public gb::ces_base_system {
    std::vector<resident_navigation> m_navigations;
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
    // Apply worker-built CPU navigation on the game-loop thread only.
    void apply_navigation(std::vector<resident_navigation>);
    void refresh_navigation(const std::vector<terrain_mesh_data>& parts,const std::vector<float>& heights,
                            const std::vector<float>& terrace_heights={});
};
}
