#pragma once
#include "ces_base_system.h"
#include "ocean_wave_model.h"
#include "ocean_mesh_builder.h"
#include <deque>
namespace game {
    class ces_ocean_system final : public gb::ces_base_system {
        ocean_settings m_settings;
        double m_time=0;
        float m_fixed_time=-1;
        int m_debug_mode=0;
        glm::vec3 m_optics_intensity=glm::vec3(1.f);
        bool m_diagnostics_reported=false;
        std::vector<gb::ces_entity_weak_ptr> m_receivers;
        struct surface {
            gb::ces_entity_weak_ptr water,foam;
            terrain_mesh_data source;
            std::shared_ptr<ocean_mesh_quadtree> tree;
            std::shared_ptr<gb::mesh_3d> original;
            std::shared_ptr<gb::mesh_3d> water_mesh,foam_mesh;
            std::shared_ptr<gb::ibo> water_indices,foam_indices;
            std::vector<uint16_t> last_water,last_foam;
            ocean_visibility selected;
            bool horizon=false;
        };
        std::vector<surface> m_surfaces;
        std::array<float,16> m_cull_matrix = {};
        bool m_cull_valid=false;
        void update_surface_visibility(const glm::mat4&);
        void rebuild_surface(surface&,const terrain_mesh_data&);
        struct interaction {gb::ces_entity_weak_ptr source,mask;std::shared_ptr<gb::mesh_3d> mesh;};
        std::vector<interaction> m_interactions;
        // Match the scene's 12 rendered-update grace. Frame acquisition waits
        // the Vulkan fences before feeds; presentation follows all feeds.
        std::deque<std::pair<int,std::shared_ptr<gb::mesh_3d>>> m_retired_interaction_meshes;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_buoyancy_mask;
    protected:
        void on_feed_start(f32) override {}
        void on_feed(const gb::ces_entity_shared_ptr&,f32) override;
        void on_feed_end(f32) override {}
    public:
        STTI_CLASS_GUID(ces_ocean_system,gb::ces_base_system::g_guids_container)
        explicit ces_ocean_system(const ocean_settings&);
        void add_surface(const gb::ces_entity_shared_ptr& water,const gb::ces_entity_shared_ptr& foam,
                         const terrain_mesh_data&,bool horizon);
        void reset_surface(const gb::ces_entity_shared_ptr& water,const terrain_mesh_data&);
        void add_receiver(const gb::ces_entity_shared_ptr&);
        void add_interaction(const gb::ces_entity_shared_ptr& source,const gb::ces_entity_shared_ptr& mask);
        void set_mean_level(float);
        float surface_height(float x,float z)const{return sample_ocean(m_settings,x,z,m_fixed_time>=0?m_fixed_time:m_time).position.y;}
        void set_world_dimensions(float world_size,float water_size);
        void set_optics_intensity(const glm::vec3& value){m_optics_intensity=glm::clamp(value,glm::vec3(0),glm::vec3(3));}
        void set_terrain_dimensions(float height,float shore){m_settings.terrain_layer_height=height;m_settings.terrain_shore_height=shore;}
    };
}
