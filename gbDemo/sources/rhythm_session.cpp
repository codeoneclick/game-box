#include "rhythm_session.h"
#include <algorithm>
#include <cmath>
#include <limits>
namespace game
{
double RhythmSession::beat_position(double seconds) const
{
    if (!std::isfinite(seconds) || seconds < 0)
        return -1;
    const double cycle = std::floor(seconds / track_.duration),
                 local = seconds - cycle * track_.duration;
    auto next = std::upper_bound(timeline_.begin(), timeline_.end(), local);
    const auto i = static_cast<size_t>(next - timeline_.begin());
    const double previous = i ? timeline_[i - 1] : timeline_.back() - track_.duration;
    const double following =
        i < timeline_.size() ? timeline_[i] : timeline_.front() + track_.duration;
    return cycle * timeline_.size() + static_cast<double>(i) - 1 +
           (local - previous) / (following - previous);
}
RhythmSession::RhythmSession(RhythmTrack t)
    : track_(std::move(t)), timeline_(resolve_rhythm_beats(track_)),
      actions_(resolve_rhythm_events(track_))
{
}
RhythmBeat RhythmSession::beat(std::uint64_t id) const
{
    return {id, (id / timeline_.size()) * track_.duration + timeline_[id % timeline_.size()]};
}
RhythmScheduledEvent RhythmSession::action_at(std::uint64_t id) const
{
    auto e = actions_[id % actions_.size()];
    e.time += (id / actions_.size()) * track_.duration;
    return RhythmScheduledEvent(id, e, id < next_expire_ || resolved_.count(id) != 0);
}
std::vector<RhythmScheduledEvent> RhythmSession::upcoming(double t, size_t count) const
{
    std::vector<RhythmScheduledEvent> out;
    if (!std::isfinite(t) || t < 0)
        return out;
    count = std::min<size_t>(count, 16);
    const auto loop = static_cast<std::uint64_t>(t / track_.duration);
    std::uint64_t start = loop * actions_.size();
    if (loop)
        start -= actions_.size();
    // Musical command ownership is independent of the tolerance/expiry window.
    // Keep the earlier event at an exact midpoint, matching tap()'s tie rule.
    while ((action_at(start).event.time + action_at(start + 1).event.time) * .5 < t - 1e-9)
        ++start;
    for (size_t i = 0; i < count; ++i)
        out.push_back(action_at(start + i));
    return out;
}
void RhythmSession::restart()
{
    taps_.clear();
    events_.clear();
    resolved_.clear();
    score_ = {};
    time_ = 0;
    judgement_time_ = -1;
    next_event_ = next_expire_ = 0;
    resolutions_.clear();
    has_resolution_ = false;
    judgement_ = RhythmJudgement::none;
}
void RhythmSession::enqueue_tap(double t)
{
    enqueue_input(RhythmInput(t, RhythmAction::Tap));
}
void RhythmSession::enqueue_input(RhythmInput input)
{
    if (std::isfinite(input.time) && input.time >= 0 && input.time >= time_ - .5 &&
        static_cast<unsigned>(input.action) < 5 && taps_.size() < 1024)
        taps_.push_back(input);
}
void RhythmSession::set_energy(double e)
{
    if (std::isfinite(e))
        score_.energy = std::max(0., std::min(1., e));
}
bool RhythmSession::set_windows(RhythmWindows w)
{
    if (!std::isfinite(w.perfect) || !std::isfinite(w.great) || !std::isfinite(w.good) ||
        w.perfect <= 0 || w.great < w.perfect || w.good < w.great || w.good > .500)
        return false;
    windows_ = w;
    return true;
}
void RhythmSession::judge(RhythmJudgement j, double t, const RhythmScheduledEvent *event,
                          const RhythmInput *input)
{
    RhythmResolution result;
    result.judgement = j;
    result.input_time = t;
    result.presentation_time = update_time_;
    if (event)
    {
        result.id = event->id;
        result.event = event->event;
    }
    if (input)
    {
        result.has_input = true;
        result.input = input->action;
        result.input_time = input->time;
        result.error = event ? input->time - event->event.time : 0;
    }
    last_resolution_ = result;
    has_resolution_ = true;
    resolutions_.push_back(result);
    if (j == RhythmJudgement::perfect)
        ++score_.perfect_count;
    if (j == RhythmJudgement::great)
        ++score_.great_count;
    if (j == RhythmJudgement::good)
        ++score_.good_count;
    if (j == RhythmJudgement::wrong)
        ++score_.wrong_count;
    if (j == RhythmJudgement::miss)
        ++score_.miss_count;
    int points = j == RhythmJudgement::perfect ? 100
                 : j == RhythmJudgement::great ? 70
                 : j == RhythmJudgement::good  ? 30
                                               : 0;
    score_.score += points;
    score_.earned += points;
    ++score_.judgements;
    score_.combo = points ? score_.combo + 1 : 0;
    score_.max_combo = std::max(score_.combo, score_.max_combo);
    set_energy(score_.energy + (points == 100  ? .025
                                : points == 70 ? .015
                                : points == 30 ? .005
                                               : -.020));
    judgement_ = j;
    judgement_time_ = t;
}
void RhythmSession::expire(double t)
{
    while (action_at(next_expire_).event.time + windows_.good < t - 1e-9)
    {
        auto b = action_at(next_expire_++);
        if (!resolved_.erase(b.id))
            judge(RhythmJudgement::miss, b.event.time + windows_.good, &b, nullptr);
    }
}
void RhythmSession::tap(const RhythmInput &input)
{
    const double t = input.time;
    std::uint64_t nearest = 0;
    double error = std::numeric_limits<double>::max();
    auto loop = static_cast<std::uint64_t>(t / track_.duration);
    for (auto l = loop ? loop - 1 : 0; l <= loop + 1; ++l)
    {
        const double local = t - l * track_.duration;
        auto it =
            std::lower_bound(actions_.begin(), actions_.end(), local,
                             [](const RhythmEvent &e, double value) { return e.time < value; });
        for (int k = 0; k < 2; ++k)
        {
            if (k == 1)
            {
                if (it == actions_.begin())
                    continue;
                --it;
            }
            if (it == actions_.end())
                continue;
            auto id = l * actions_.size() + static_cast<size_t>(it - actions_.begin());
            double e = std::abs(action_at(id).event.time - t);
            if (e < error - 1e-9 || (std::abs(e - error) <= 1e-9 && id < nearest))
            {
                error = e;
                nearest = id;
            }
        }
    }
    if (error <= windows_.good + 1e-9)
    {
        if (nearest < next_expire_ || resolved_.count(nearest))
            return;
        resolved_.insert(nearest);
        auto event = action_at(nearest);
        judge(input.action != event.event.action ? RhythmJudgement::wrong
              : error <= windows_.perfect + 1e-9 ? RhythmJudgement::perfect
              : error <= windows_.great + 1e-9   ? RhythmJudgement::great
                                                 : RhythmJudgement::good,
              t, &event, &input);
    }
    else
        judge(RhythmJudgement::miss, t, nullptr, &input);
}
void RhythmSession::update(double t, double judgement_delay)
{
    events_.clear();
    resolutions_.clear();
    if (!std::isfinite(t) || t < time_)
        return;
    update_time_ = t;
    std::stable_sort(taps_.begin(), taps_.end(),
                     [](const RhythmInput &a, const RhythmInput &b) { return a.time < b.time; });
    size_t n = 0;
    while (n < taps_.size() && taps_[n].time <= t)
    {
        expire(taps_[n].time);
        tap(taps_[n++]);
    }
    taps_.erase(taps_.begin(), taps_.begin() + n);
    expire(t -
           std::max(0.0, std::min(.25, std::isfinite(judgement_delay) ? judgement_delay : 0.0)));
    while (beat(next_event_).time <= t)
    {
        events_.push_back(beat(next_event_++));
    }
    time_ = t;
}
const char *rhythm_judgement_name(RhythmJudgement j)
{
    switch (j)
    {
    case RhythmJudgement::perfect:
        return "PERFECT";
    case RhythmJudgement::great:
        return "GREAT";
    case RhythmJudgement::good:
        return "GOOD";
    case RhythmJudgement::miss:
        return "MISS";
    case RhythmJudgement::wrong:
        return "WRONG";
    default:
        return "READY";
    }
}
} // namespace game
