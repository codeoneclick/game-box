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
    : track_(std::move(t)), timeline_(resolve_rhythm_beats(track_))
{
}
RhythmBeat RhythmSession::beat(std::uint64_t id) const
{
    return {id, (id / timeline_.size()) * track_.duration + timeline_[id % timeline_.size()]};
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
    judgement_ = RhythmJudgement::none;
}
void RhythmSession::enqueue_tap(double t)
{
    if (std::isfinite(t) && t >= 0 && t >= time_ - .5)
        taps_.push_back(t);
}
void RhythmSession::set_energy(double e)
{
    if (std::isfinite(e))
        score_.energy = std::max(0., std::min(1., e));
}
bool RhythmSession::set_windows(RhythmWindows w)
{
    double gap = track_.duration - timeline_.back() + timeline_.front();
    for (size_t i = 1; i < timeline_.size(); ++i)
        gap = std::min(gap, timeline_[i] - timeline_[i - 1]);
    if (!std::isfinite(w.perfect) || !std::isfinite(w.great) || !std::isfinite(w.good) ||
        w.perfect <= 0 || w.great < w.perfect || w.good < w.great || w.good * 2 >= gap)
        return false;
    windows_ = w;
    return true;
}
void RhythmSession::judge(RhythmJudgement j, double t)
{
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
    while (beat(next_expire_).time + windows_.good < t - 1e-9)
    {
        auto b = beat(next_expire_++);
        if (!resolved_.erase(b.id))
            judge(RhythmJudgement::miss, b.time + windows_.good);
    }
}
void RhythmSession::tap(double t)
{
    std::uint64_t nearest = 0;
    double error = std::numeric_limits<double>::max();
    auto loop = static_cast<std::uint64_t>(t / track_.duration);
    for (auto l = loop ? loop - 1 : 0; l <= loop + 1; ++l)
    {
        const double local = t - l * track_.duration;
        auto it = std::lower_bound(timeline_.begin(), timeline_.end(), local);
        for (int k = 0; k < 2; ++k)
        {
            if (k == 1)
            {
                if (it == timeline_.begin())
                    continue;
                --it;
            }
            if (it == timeline_.end())
                continue;
            auto id = l * timeline_.size() + static_cast<size_t>(it - timeline_.begin());
            double e = std::abs(beat(id).time - t);
            if (e < error)
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
        judge(error <= windows_.perfect + 1e-9 ? RhythmJudgement::perfect
              : error <= windows_.great + 1e-9 ? RhythmJudgement::great
                                               : RhythmJudgement::good,
              t);
    }
    else
        judge(RhythmJudgement::miss, t);
}
void RhythmSession::update(double t, double judgement_delay)
{
    events_.clear();
    if (!std::isfinite(t) || t < time_)
        return;
    std::sort(taps_.begin(), taps_.end());
    size_t n = 0;
    while (n < taps_.size() && taps_[n] <= t)
    {
        expire(taps_[n]);
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
    default:
        return "READY";
    }
}
} // namespace game
