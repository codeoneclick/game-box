#pragma once
#include "ces_base_system.h"
#include "ocean_wave_model.h"
namespace game {
    class ces_ocean_system final : public gb::ces_base_system {
        ocean_settings m_settings;
        double m_time=0;
        float m_fixed_time=-1;
        int m_debug_mode=0;
        bool m_diagnostics_reported=false;
        std::vector<gb::ces_entity_weak_ptr> m_receivers;
        std::vector<std::pair<gb::ces_entity_weak_ptr,gb::ces_entity_weak_ptr>> m_interactions;
        // Keep replaced/detached geometry alive until scene teardown: draws may still be in flight.
        std::vector<std::shared_ptr<gb::mesh_3d>> m_interaction_meshes;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_buoyancy_mask;
    protected:
        void on_feed_start(f32) override {}
        void on_feed(const gb::ces_entity_shared_ptr&,f32) override;
        void on_feed_end(f32) override {}
    public:
        STTI_CLASS_GUID(ces_ocean_system,gb::ces_base_system::g_guids_container)
        explicit ces_ocean_system(const ocean_settings&);
        void add_surface(const gb::ces_entity_shared_ptr&);
        void add_receiver(const gb::ces_entity_shared_ptr&);
        void add_interaction(const gb::ces_entity_shared_ptr& source,const gb::ces_entity_shared_ptr& mask);
    };
}
