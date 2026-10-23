#include "rhythm_dance_pose.h"
#include <algorithm>
#include <cmath>
namespace game
{
RhythmDancePose rhythm_dance_pose(RhythmAction action, double age, float strength)
{
    RhythmDancePose p;
    if (!std::isfinite(age) || age < 0 || age >= .45)
        return p;
    const float hit = static_cast<float>(std::sin(age / .45 * 3.141592653589793)) *
                      std::max(0.f, std::min(1.5f, strength));
    if (action == RhythmAction::Tap)
    {
        p.y = .3f * hit;
        p.scale_y = 1 - .1f * hit;
        p.scale_x = 1 + .08f * hit;
    }
    if (action == RhythmAction::SwipeLeft || action == RhythmAction::SwipeRight)
    {
        const float sign = action == RhythmAction::SwipeLeft ? -1.f : 1.f;
        p.x = sign * .9f * hit;
        p.lean = -sign * .23f * hit;
    }
    if (action == RhythmAction::SwipeUp)
    {
        p.y = 1.35f * hit;
        p.scale_y = 1 + .15f * hit;
    }
    if (action == RhythmAction::SwipeDown)
    {
        p.y = -.15f * hit;
        p.scale_y = 1 - .32f * hit;
        p.scale_x = 1 + .18f * hit;
    }
    return p;
}
} // namespace game
