#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace game
{
struct RhythmTrack
{
    std::string id = "rhythm_ship_metronome", audio_file;
    double bpm = 128, offset = 0, duration = 30;
    std::vector<double> beats;
};
struct RhythmBeat
{
    std::uint64_t id;
    double time;
};
RhythmTrack default_rhythm_track();
bool load_rhythm_track(const std::string &, RhythmTrack &, std::string &);
std::vector<double> resolve_rhythm_beats(const RhythmTrack &);
RhythmBeat rhythm_beat_at(const RhythmTrack &, std::uint64_t);
} // namespace game
