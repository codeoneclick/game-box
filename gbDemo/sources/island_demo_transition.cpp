#include "island_demo_transition.h"
#include "island_demo_scene.h"
#include "scene_fabricator.h"


namespace game
{
    island_demo_transition::island_demo_transition(const std::string& guid, const gb::window_impl_shared_ptr& window, bool is_offscreen)
        : gb::game_transition(guid,window,is_offscreen) {}

    void island_demo_transition::create_scene()
    {
        m_scene=gb::ces_entity::construct<island_demo_scene>(shared_from_this());
        set_fabricator(std::make_shared<gb::scene_fabricator>());
    }

    void island_demo_transition::destroy_scene() { m_scene=nullptr; }
}
