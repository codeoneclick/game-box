#include "rhythm_dance_pose.h"
#include <algorithm>
#include <cmath>
namespace game
{
RhythmDancePose rhythm_dance_pose(RhythmAction action, double age, float strength)
{
    RhythmDancePose p;
    if (!std::isfinite(age) || age < 0 || age >= .55)
        return p;
    const float power = std::max(0.f, std::min(1.5f, strength));
    const float hit = static_cast<float>(std::sin(age / .55 * 3.141592653589793)) * power;
    if (action == RhythmAction::Tap)
    {
        // A quick attack makes the visible accent follow the input, not a late
        // midpoint of a long sine animation. Three taps remain visually distinct.
        if (age >= .24)
            return p;
        const float tap = power * static_cast<float>(age < .06 ? age / .06 : (.24 - age) / .18);
        p.y = .3f * tap;
        p.scale_y = 1 - .1f * tap;
        p.scale_x = 1 + .08f * tap;
    }
    else
    {
        // Step accent in 100ms, settle for 80ms, return to the anchor by 550ms.
        float u = age < .10   ? static_cast<float>(age / .10)
                  : age < .18 ? 1.f
                              : static_cast<float>((.55 - age) / .37);
        u = std::max(0.f, std::min(1.f, u));
        const float step = 1.5f * u * u * (3 - 2 * u) * power;
        if (action == RhythmAction::SwipeLeft)
            p.x = -step;
        if (action == RhythmAction::SwipeRight)
            p.x = step;
        if (action == RhythmAction::SwipeUp)
            p.z = -step;
        if (action == RhythmAction::SwipeDown)
            p.z = step;
        p.y = .08f * hit;
        p.lean = -p.x * .08f;
    }
    return p;
}
} // namespace game
