#pragma once
#include "fake_leaderboard.h"
#include "rhythm_audio_transport.h"
#include "rhythm_session.h"
#include "rhythm_ship_playback.h"
#include "scene_graph.h"
namespace game
{
struct RhythmShipDebugSettings
{
    RhythmPresentationSettings presentation;
    float volume = .65f, stage_intensity = 1;
    int crowd_count = 40;
};
class RhythmShipUI
{
    struct implementation;
    std::shared_ptr<implementation> impl_;

  public:
    RhythmShipUI(const gb::scene_fabricator_shared_ptr &, std::shared_ptr<RhythmSession>,
                 std::shared_ptr<FakeLeaderboard>, std::shared_ptr<gb::al::RhythmAudioTransport>,
                 std::shared_ptr<RhythmShipDebugSettings>, glm::ivec2,
                 std::function<void(const RhythmTrack &)>,
                 std::shared_ptr<RhythmShipPlayback> playback = {});
    gb::ces_entity_shared_ptr root() const;
    void update();
    void set_player_position(glm::vec2);
    void set_crowd_sync(double);
    void toggle_debug();
    void set_overlay_blocked(bool);
    bool contains(glm::ivec2) const;
    bool keyboard_captured() const
    {
        return false;
    }
    bool consume_force_beat();
};
} // namespace game
