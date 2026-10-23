#include "rhythm_gesture.h"
#include <algorithm>
#include <cmath>
namespace game
{
void RhythmGesture::begin(double x, double y, double t, std::uint64_t epoch)
{
    cancel();
    if (!std::isfinite(x) || !std::isfinite(y) || !std::isfinite(t))
        return;
    x_ = x;
    y_ = y;
    start_ = t;
    epoch_ = epoch;
    active_ = true;
}
bool RhythmGesture::move(double x, double y, double t, std::uint64_t epoch, RhythmInput &out)
{
    if (!active_)
        return false;
    if (epoch != epoch_ || !std::isfinite(t) || t < start_ || !std::isfinite(x) ||
        !std::isfinite(y))
    {
        cancel();
        return false;
    }
    if (swiped_)
        return false;
    const double dx = x - x_, dy = y - y_, ax = std::abs(dx), ay = std::abs(dy);
    const double distance = std::isfinite(threshold) ? std::max(4., threshold) : 24;
    if (std::max(ax, ay) < distance)
        return false;
    const double ratio = std::max(1.05, std::isfinite(axis_ratio) ? axis_ratio : 1.25);
    if (ax >= ay * ratio)
        out = RhythmInput(t, dx < 0 ? RhythmAction::SwipeLeft : RhythmAction::SwipeRight);
    else if (ay >= ax * ratio)
        out = RhythmInput(t, dy < 0 ? RhythmAction::SwipeUp : RhythmAction::SwipeDown);
    else
        return false;
    swiped_ = true;
    return true;
}
bool RhythmGesture::end(double x, double y, double t, std::uint64_t epoch, RhythmInput &out)
{
    if (!active_)
        return false;
    if (move(x, y, t, epoch, out))
    {
        cancel();
        return true;
    }
    const bool tap = active_ && !swiped_ && epoch == epoch_ && std::isfinite(x) &&
                     std::isfinite(y) && std::isfinite(t) && t >= start_ &&
                     std::max(std::abs(x - x_), std::abs(y - y_)) <
                         (std::isfinite(threshold) ? std::max(4., threshold) : 24);
    cancel();
    if (tap)
        out = RhythmInput(t, RhythmAction::Tap);
    return tap;
}
} // namespace game
