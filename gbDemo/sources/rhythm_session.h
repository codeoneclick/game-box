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
    wrong,
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
    std::uint64_t perfect_count = 0, great_count = 0, good_count = 0, wrong_count = 0,
                  miss_count = 0;
    std::uint64_t score = 0, combo = 0, max_combo = 0, judgements = 0;
    double energy = .35;
    double accuracy() const
    {
        return judgements ? 100.0 * earned / (100.0 * judgements) : 0;
    }
    std::uint64_t earned = 0;
};
struct RhythmResolution
{
    std::uint64_t id = std::numeric_limits<std::uint64_t>::max();
    RhythmEvent event;
    RhythmJudgement judgement = RhythmJudgement::none;
    RhythmAction input = RhythmAction::Tap;
    double input_time = 0, error = 0, presentation_time = 0;
    bool has_input = false;
};
class RhythmSession
{
    RhythmTrack track_;
    std::vector<double> timeline_;
    std::vector<RhythmInput> taps_;
    std::vector<RhythmEvent> actions_;
    std::vector<RhythmResolution> resolutions_;
    RhythmResolution last_resolution_;
    bool has_resolution_ = false;
    double update_time_ = 0;
    std::vector<RhythmBeat> events_;
    std::set<std::uint64_t> resolved_;
    RhythmScore score_;
    RhythmWindows windows_;
    double time_ = 0, judgement_time_ = -1;
    std::uint64_t next_event_ = 0, next_expire_ = 0;
    RhythmJudgement judgement_ = RhythmJudgement::none;
    RhythmBeat beat(std::uint64_t) const;
    RhythmScheduledEvent action_at(std::uint64_t) const;
    void expire(double);
    void judge(RhythmJudgement, double, const RhythmScheduledEvent *, const RhythmInput *);
    void tap(const RhythmInput &);

  public:
    explicit RhythmSession(RhythmTrack);
    void restart();
    void enqueue_tap(double);
    void enqueue_input(RhythmInput);
    const std::vector<RhythmResolution> &resolutions() const
    {
        return resolutions_;
    }
    bool last_resolution(RhythmResolution &out) const
    {
        if (!has_resolution_)
            return false;
        out = last_resolution_;
        return true;
    }
    std::vector<RhythmScheduledEvent> upcoming(double raw_time, size_t count) const;
    void update(double, double judgement_delay = 0);
    bool set_windows(RhythmWindows);
    RhythmWindows windows() const
    {
        return windows_;
    }
    void set_energy(double);
    const RhythmScore &score() const
    {
        return score_;
    }
    const std::vector<RhythmBeat> &beat_events() const
    {
        return events_;
    }
    RhythmJudgement last_judgement() const
    {
        return judgement_;
    }
    double last_judgement_time() const
    {
        return judgement_time_;
    }
    double beat_position(double seconds) const;
    const RhythmTrack &track() const
    {
        return track_;
    }
    void debug_lead(std::uint64_t n)
    {
        score_.score = n;
    }
};
const char *rhythm_judgement_name(RhythmJudgement);
} // namespace game
