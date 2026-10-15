#pragma once
#include "ces_base_system.h"
namespace game {
class ces_sun_shadow_system final:public gb::ces_base_system {
 struct implementation;
 std::shared_ptr<implementation> impl_;
 std::bitset<std::numeric_limits<uint8_t>::max()> mask_;
 gb::ces_entity_weak_ptr root_;
protected:
 void on_feed_start(f32) override {}
 void on_feed(const gb::ces_entity_shared_ptr& root,f32) override;
 void on_feed_end(f32) override;
public:
 STTI_CLASS_GUID(ces_sun_shadow_system,gb::ces_base_system::g_guids_container)
 explicit ces_sun_shadow_system(const gb::scene_fabricator_shared_ptr&);
 void initialize(const gb::ces_entity_shared_ptr&);
 ~ces_sun_shadow_system() override;
};
}
