#pragma once
#include "game_transition.h"

namespace game
{
    class island_demo_transition final : public gb::game_transition
    {
    protected:
        void create_scene() override;
        void destroy_scene() override;
        glm::ivec2 get_ws_technique_size(const std::string&,const glm::ivec2&) const override;
    public:
        island_demo_transition(const std::string& guid, const gb::window_impl_shared_ptr& window, bool is_offscreen);
    };
}
