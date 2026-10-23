#include "rhythm_stage_state.h"
#include <algorithm>
#include <cmath>
namespace game
{
namespace
{
float bounded(double v, double low, double high)
{
    return static_cast<float>(std::isfinite(v) ? std::max(low, std::min(high, v)) : low);
}
} // namespace
RhythmStageFrame rhythm_stage_state(RhythmAction action, double age, double beat_age, double energy,
                                    double sync, float intensity, bool enabled)
{
    RhythmStageFrame f;
    const float power = bounded(intensity, 0, 3);
    const float pulse =
        std::isfinite(age) && age >= 0 && age < .65 ? static_cast<float>(std::exp(-age * 5)) : 0;
    const float beat =
        std::isfinite(beat_age) && beat_age >= 0 ? static_cast<float>(std::exp(-beat_age * 9)) : 0;
    f.glow = power * (.1f + bounded(energy, 0, 1) * .5f + bounded(sync, 0, 1) * .4f + beat * .4f);
    if (enabled)
    {
        if (action == RhythmAction::Tap)
            f.speaker = pulse;
        if (action == RhythmAction::SwipeLeft)
            f.sweep = -pulse;
        if (action == RhythmAction::SwipeRight)
            f.sweep = pulse;
        if (action == RhythmAction::SwipeUp)
            f.vertical = 1 + pulse * .8f;
        if (action == RhythmAction::SwipeDown)
            f.floor_scale = 1 - pulse * .4f;
    }
    return f;
}
RhythmStagePulse RhythmStagePulses::sample(size_t slot, double t) const
{
    RhythmStagePulse p;
    const double age = t - times_[slot % 3];
    if (!std::isfinite(age) || age < 0 || age >= 1)
        return p;
    p.active = true;
    p.scale = static_cast<float>(.8 + age * 8);
    p.opacity = static_cast<float>(1 - age);
    return p;
}
} // namespace game
