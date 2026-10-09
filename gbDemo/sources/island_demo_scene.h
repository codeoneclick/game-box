#pragma once
#include "scene_graph.h"
#include "terrain_data.h"

namespace game
{
    class island_demo_scene final : public gb::scene_graph
    {
    private:
        terrain_data m_terrain;
        gb::input_context_listener_shared_ptr m_orbit_listener;
        gb::input_context_listener_shared_ptr m_resident_listener;
        std::weak_ptr<gb::input_context> m_input_context;
    public:
        explicit island_demo_scene(const gb::game_transition_shared_ptr& transition);
        ~island_demo_scene() override;
        void create() override;
    };
}
