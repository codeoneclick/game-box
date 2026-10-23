#pragma once
#include "rhythm_action.h"
#include <array>
namespace game
{
struct RhythmStageFrame
{
    float sweep = 0, vertical = 1, floor_scale = 1, glow = 0, speaker = 0;
};
RhythmStageFrame rhythm_stage_state(RhythmAction, double action_age, double beat_age, double energy,
                                    double sync, float intensity, bool directional_enabled);
struct RhythmStagePulse
{
    bool active = false;
    float scale = 1, opacity = 0;
};
class RhythmStagePulses
{
    std::array<double, 3> times_{{-10, -10, -10}};
    size_t next_ = 0;

  public:
    void trigger(double t)
    {
        times_[next_] = t;
        next_ = (next_ + 1) % 3;
    }
    RhythmStagePulse sample(size_t, double now) const;
};
} // namespace game
