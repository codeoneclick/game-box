#pragma once
#include "rhythm_action.h"
namespace game
{
class RhythmGesture
{
    bool active_ = false, swiped_ = false;
    double x_ = 0, y_ = 0, start_ = 0;
    std::uint64_t epoch_ = 0;

  public:
    double threshold = 24, axis_ratio = 1.25;
    void begin(double x, double y, double t, std::uint64_t epoch);
    bool move(double x, double y, double t, std::uint64_t epoch, RhythmInput &);
    bool end(double x, double y, double t, std::uint64_t epoch, RhythmInput &);
    void cancel()
    {
        active_ = swiped_ = false;
    }
};
} // namespace game
