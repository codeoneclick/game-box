#pragma once
#include "game_transition.h"
namespace game
{
class rhythm_ship_transition : public gb::game_transition
{
  protected:
    void create_scene() override;
    void destroy_scene() override;

  public:
    rhythm_ship_transition(const std::string &guid, const gb::window_impl_shared_ptr &window,
                           bool offscreen)
        : gb::game_transition(guid, window, offscreen)
    {
    }
};
} // namespace game
