#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace gb
{
namespace al
{
struct RhythmPCM
{
    unsigned sample_rate = 0, channels = 0;
    std::vector<std::int16_t> samples;
    double duration() const
    {
        return sample_rate && channels ? double(samples.size() / channels) / sample_rate : 0;
    }
};
bool decode_rhythm_mp3(const std::string &, RhythmPCM &, std::string &);
enum class RhythmPositionUnit
{
    frames,
    bytes,
    milliseconds
};
class RhythmPositionCounter
{
    bool initialized_ = false;
    RhythmPositionUnit unit_ = RhythmPositionUnit::frames;
    std::uint32_t previous_ = 0;
    std::uint64_t wraps_ = 0;
    double bias_ = 0, last_ = 0;

  public:
    void reset()
    {
        *this = RhythmPositionCounter();
    }
    double seconds(std::uint32_t, RhythmPositionUnit, unsigned rate, unsigned channels);
};
} // namespace al
} // namespace gb
