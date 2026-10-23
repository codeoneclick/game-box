#pragma once
#include "rhythm_action.h"
namespace game
{
struct RhythmDancePose
{
    float x = 0, y = 0, scale_x = 1, scale_y = 1, lean = 0;
};
RhythmDancePose rhythm_dance_pose(RhythmAction, double age, float strength);
} // namespace game
