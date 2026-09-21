#pragma once

#include "scene_graph.h"
#include "ns_declarations.h"

namespace game
{
    class main_menu_scene : public gb::scene_graph
    {
    private:
        gb::input_context_listener_shared_ptr m_orbit_listener;
        std::weak_ptr<gb::input_context> m_input_context;

    public:
        explicit main_menu_scene(const gb::game_transition_shared_ptr& transition);
        ~main_menu_scene() override;

        void create() override;
    };
}
