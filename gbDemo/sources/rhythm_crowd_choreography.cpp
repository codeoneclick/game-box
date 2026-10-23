#include "rhythm_crowd_choreography.h"
#include <algorithm>
#include <cmath>
namespace game
{
namespace
{
std::uint32_t mix(std::uint32_t v)
{
    v ^= v >> 16;
    v *= 0x7feb352du;
    v ^= v >> 15;
    v *= 0x846ca68bu;
    return v ^ (v >> 16);
}
double random01(std::uint32_t v)
{
    return (mix(v) & 0xffffff) / 16777216.0;
}
} // namespace
void RhythmCrowdChoreography::restart(std::uint32_t seed)
{
    seed_ = seed;
    duration_ = 0;
    events_.clear();
    next_.fill(0);
    states_.fill(RhythmDancerState());
    outcomes_.clear();
    sync_.clear();
}
void RhythmCrowdChoreography::record(std::uint64_t id, size_t index, bool correct)
{
    if (index >= 50 || (sync_.size() == 8 && id < sync_.begin()->first))
        return;
    auto it = sync_.find(id);
    if (it == sync_.end())
        it = sync_.emplace(id, SyncFrame(active_)).first;
    if (static_cast<int>(index) < it->second.active && it->second.results[index] < 0)
        it->second.results[index] = correct ? 1 : 0;
    while (sync_.size() > 8)
        sync_.erase(sync_.begin());
}
void RhythmCrowdChoreography::update(double t, const RhythmSession &session, int count)
{
    outcomes_.clear();
    if (!std::isfinite(t) || t < 0)
        return;
    active_ = std::max(1, std::min(50, count));
    if (events_.empty())
    {
        events_ = resolve_rhythm_events(session.track());
        duration_ = session.track().duration;
    }
    const auto windows = session.windows();
    for (size_t bot = 0; bot < 49; ++bot)
    {
        for (;;)
        {
            const auto id = next_[bot];
            auto event = events_[id % events_.size()];
            event.time += (id / events_.size()) * duration_;
            const auto key = seed_ ^ mix(static_cast<std::uint32_t>(id)) ^
                             mix(static_cast<std::uint32_t>(bot + 1) * 37);
            const double skill = .65 + random01(seed_ + static_cast<std::uint32_t>(bot)) * .32;
            const double chance = random01(key);
            const double bias = (random01(key + 1) - .5) * (.035 + (1 - skill) * .3);
            RhythmBotOutcome out;
            out.bot = bot;
            out.id = id;
            out.state.action = event.action;
            const bool missing = chance > skill;
            const bool wrong = !missing && random01(key + 2) < (1 - skill) * .24;
            if (wrong)
                out.state.action = static_cast<RhythmAction>(
                    (static_cast<unsigned>(event.action) + 1 + (mix(key + 3) % 4)) % 5);
            out.state.judgement = missing                             ? RhythmJudgement::miss
                                  : wrong                             ? RhythmJudgement::wrong
                                  : std::abs(bias) <= windows.perfect ? RhythmJudgement::perfect
                                  : std::abs(bias) <= windows.great   ? RhythmJudgement::great
                                  : std::abs(bias) <= windows.good    ? RhythmJudgement::good
                                                                      : RhythmJudgement::miss;
            const double due = event.time + (missing ? windows.good : bias);
            if (due > t + 1e-9)
                break;
            ++next_[bot];
            out.state.start = due;
            out.state.strength = missing ? .18f : wrong ? .4f : 1.f;
            states_[bot + 1] = out.state;
            outcomes_.push_back(out);
            record(id, bot + 1, out.points() > 0);
        }
    }
}
void RhythmCrowdChoreography::local_resolution(const RhythmResolution &result)
{
    if (result.id != std::numeric_limits<std::uint64_t>::max())
        record(result.id, 0,
               result.judgement == RhythmJudgement::perfect ||
                   result.judgement == RhythmJudgement::great ||
                   result.judgement == RhythmJudgement::good);
    if (result.has_input)
    {
        states_[0].judgement = result.judgement;
        states_[0].strength =
            result.judgement == RhythmJudgement::wrong || result.judgement == RhythmJudgement::miss
                ? .45f
                : 1.2f;
    }
}
void RhythmCrowdChoreography::local_input(RhythmInput input)
{
    if (input.time <= states_[0].start)
        return;
    states_[0].action = input.action;
    states_[0].start = input.time;
    states_[0].strength = .75f;
    states_[0].judgement = RhythmJudgement::none;
}
double RhythmCrowdChoreography::sync() const
{
    double correct = 0, total = 0;
    for (const auto &frame : sync_)
        for (int i = 0; i < frame.second.active; ++i)
        {
            ++total;
            if (frame.second.results[i] > 0)
                ++correct;
        }
    return total ? correct / total : 0;
}
} // namespace game
