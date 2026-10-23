#include "fake_leaderboard.h"
#include "rhythm_session.h"
#include <algorithm>
namespace game
{
FakeLeaderboard::FakeLeaderboard(std::uint32_t s) : seed_(s)
{
    restart();
}
void FakeLeaderboard::restart()
{
    random_.seed(seed_);
    next_ = 0;
    bots_.clear();
    skills_.clear();
    outcomes_.assign(10, 0);
    for (const char *n : {"BeatFox", "PixelPirate", "NeonSamurai", "DiscoBot", "MoonDancer",
                          "BassWizard", "StarPilot", "GrooveCat", "NovaKnight", "WaveRider"})
    {
        bots_.push_back({n, 0, false});
        skills_.push_back(std::uniform_real_distribution<double>(.65, .98)(random_));
    }
}
void FakeLeaderboard::on_beat(std::uint64_t id)
{
    if (id < next_)
        return;
    while (next_ <= id)
    {
        for (size_t i = 0; i < bots_.size(); ++i)
        {
            double v = std::uniform_real_distribution<double>(0, 1)(random_);
            bots_[i].score += v > skills_[i]        ? 0
                              : v < skills_[i] * .4 ? 100
                              : v < skills_[i] * .8 ? 70
                                                    : 30;
        }
        ++next_;
    }
}
void FakeLeaderboard::on_outcome(size_t bot, std::uint64_t id, RhythmJudgement judgement)
{
    if (bot >= bots_.size() || id < outcomes_[bot])
        return;
    outcomes_[bot] = id + 1;
    bots_[bot].score += judgement == RhythmJudgement::perfect ? 100
                        : judgement == RhythmJudgement::great ? 70
                        : judgement == RhythmJudgement::good  ? 30
                                                              : 0;
}
std::vector<RhythmRank> FakeLeaderboard::ranking(std::uint64_t local) const
{
    auto r = bots_;
    r.insert(r.begin(), {"YOU", local, true});
    std::stable_sort(r.begin(), r.end(),
                     [](const RhythmRank &a, const RhythmRank &b) { return a.score > b.score; });
    return r;
}
std::uint64_t FakeLeaderboard::highest_bot_score() const
{
    std::uint64_t s = 0;
    for (auto &b : bots_)
        s = std::max(s, b.score);
    return s;
}
} // namespace game
