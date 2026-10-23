#pragma once
#include "rhythm_track.h"
namespace game
{
struct RhythmTrackEntry
{
    std::string title, map_file;
    RhythmTrack track;
};
bool load_rhythm_catalog(const std::string &, std::vector<RhythmTrackEntry> &, std::string &);
bool prepare_rhythm_track(const RhythmTrackEntry &, double, RhythmTrack &, std::string &);
} // namespace game
