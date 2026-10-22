#pragma once
#include "rhythm_track.h"
#include <set>
namespace game
{
enum class RhythmJudgement
{
    none,
    perfect,
    great,
    good,
    miss
};
struct RhythmWindows
{
    double perfect, great, good;
    RhythmWindows(double p = .040, double g = .080, double d = .130) : perfect(p), great(g), good(d)
    {
    }
};
struct RhythmScore
{
    std::uint64_t score = 0, combo = 0, max_combo = 0, judgements = 0;
    double energy = .35;
    double accuracy() const { return judgements ? 100.0 * earned / (100.0 * judgements) : 0; }
    std::uint64_t earned = 0;
};
class RhythmSession
{
    RhythmTrack track_;
    std::vector<double> timeline_, taps_;
    std::vector<RhythmBeat> events_;
    std::set<std::uint64_t> resolved_;
    RhythmScore score_;
    RhythmWindows windows_;
    double time_ = 0, judgement_time_ = -1;
    std::uint64_t next_event_ = 0, next_expire_ = 0;
    RhythmJudgement judgement_ = RhythmJudgement::none;
    RhythmBeat beat(std::uint64_t) const;
    void expire(double);
    void judge(RhythmJudgement, double);
    void tap(double);

  public:
    explicit RhythmSession(RhythmTrack);
    void restart();
    void enqueue_tap(double);
    void update(double);
    bool set_windows(RhythmWindows);
    RhythmWindows windows() const { return windows_; }
    void set_energy(double);
    const RhythmScore &score() const { return score_; }
    const std::vector<RhythmBeat> &beat_events() const { return events_; }
    RhythmJudgement last_judgement() const { return judgement_; }
    double last_judgement_time() const { return judgement_time_; }
    double beat_position(double seconds) const;
    const RhythmTrack &track() const { return track_; }
    void debug_lead(std::uint64_t n) { score_.score = n; }
};
const char *rhythm_judgement_name(RhythmJudgement);
} // namespace game
