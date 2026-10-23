#pragma once
#include "rhythm_session.h"
#include <array>
#include <map>
namespace game
{
struct RhythmDancerState
{
    RhythmAction action = RhythmAction::Tap;
    double start = -10;
    RhythmJudgement judgement = RhythmJudgement::none;
    float strength = 0;
};
struct RhythmBotOutcome
{
    size_t bot = 0;
    std::uint64_t id = 0;
    RhythmDancerState state;
    unsigned points() const
    {
        return state.judgement == RhythmJudgement::perfect ? 100
               : state.judgement == RhythmJudgement::great ? 70
               : state.judgement == RhythmJudgement::good  ? 30
                                                           : 0;
    }
};
class RhythmCrowdChoreography
{
    struct SyncFrame
    {
        int active;
        std::array<int, 50> results;
        explicit SyncFrame(int n = 40) : active(n)
        {
            results.fill(-1);
        }
    };
    std::uint32_t seed_ = 1337;
    double duration_ = 0;
    std::vector<RhythmEvent> events_;
    std::array<std::uint64_t, 49> next_{};
    std::array<RhythmDancerState, 50> states_;
    std::vector<RhythmBotOutcome> outcomes_;
    std::map<std::uint64_t, SyncFrame> sync_;
    int active_ = 40;
    void record(std::uint64_t, size_t, bool);

  public:
    void restart(std::uint32_t);
    void update(double, const RhythmSession &, int active_count);
    void local_resolution(const RhythmResolution &);
    void local_input(RhythmInput);
    double sync() const;
    RhythmDancerState dancer_state(size_t index, double) const
    {
        return states_[index % 50];
    }
    const std::vector<RhythmBotOutcome> &outcomes() const
    {
        return outcomes_;
    }
};
} // namespace game
