#pragma once
#include "scene_graph.h"
namespace game
{
class rhythm_ship_scene : public gb::scene_graph
{
    struct state;
    std::shared_ptr<state> state_;
    gb::input_context_listener_shared_ptr input_;
    std::weak_ptr<gb::input_context> context_;

  public:
    explicit rhythm_ship_scene(const gb::game_transition_shared_ptr &t) : gb::scene_graph(t) {}
    ~rhythm_ship_scene() override;
    void create() override;
};
} // namespace game
