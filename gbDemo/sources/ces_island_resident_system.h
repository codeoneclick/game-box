#pragma once
#include "ces_base_system.h"
#include "resident_navigation.h"
namespace game {
class ces_island_resident_system final:public gb::ces_base_system {
    resident_navigation m_navigation;
    std::bitset<std::numeric_limits<uint8_t>::max()> m_mask;
    std::string m_fixed_pose;
    float m_fixed_time=-1;
    bool m_debug=false;
    double m_elapsed=0,m_next_report=0;
protected:
    void on_feed_start(f32) override;
    void on_feed(const gb::ces_entity_shared_ptr&,f32) override;
    void on_feed_end(f32) override {}
public:
    STTI_CLASS_GUID(ces_island_resident_system,gb::ces_base_system::g_guids_container)
    explicit ces_island_resident_system(resident_navigation);
};
}
