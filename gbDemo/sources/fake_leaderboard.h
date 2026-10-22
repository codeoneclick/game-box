#pragma once
#include <cstdint>
#include <random>
#include <string>
#include <vector>
namespace game
{
struct RhythmRank
{
    std::string name;
    std::uint64_t score;
    bool local;
};
class FakeLeaderboard
{
    std::uint32_t seed_;
    std::mt19937 random_;
    std::vector<RhythmRank> bots_;
    std::vector<double> skills_;
    std::uint64_t next_ = 0;

  public:
    explicit FakeLeaderboard(std::uint32_t seed = 1337);
    void restart();
    void on_beat(std::uint64_t);
    std::vector<RhythmRank> ranking(std::uint64_t) const;
    std::uint64_t highest_bot_score() const;
};
} // namespace game
