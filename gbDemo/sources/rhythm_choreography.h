#pragma once
#include "rhythm_track.h"
namespace game
{
enum class RhythmDifficulty
{
    Easy,
    Normal,
    Hard
};
struct RhythmChoreographySettings
{
    RhythmDifficulty difficulty = RhythmDifficulty::Normal;
    unsigned enabled_actions = 31;
    std::uint32_t seed = 1337;
};
struct RhythmPresentationSettings
{
    double prompt_lead = 1.25;
    int preview_count = 2;
    float swipe_threshold = 24;
    bool beat_visualization = true, crowd_choreography = true, stage_directional = true;
};
bool generate_rhythm_choreography(const RhythmTrack &, const RhythmChoreographySettings &,
                                  RhythmTrack &, std::string &);
} // namespace game
