#pragma once
#include "rhythm_pcm.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
namespace gb
{
namespace al
{
struct RhythmAudioConfig
{
    double duration;
    std::vector<double> beats;
    std::string audio_file;
};
enum class RhythmAudioMode
{
    initializing,
    playback,
    silent
};
std::vector<std::int16_t> make_rhythm_pcm(const RhythmAudioConfig &);
double unwrap_rhythm_playback_seconds(double raw, double elapsed, double duration, double previous);
class RhythmAudioTransport
{
    struct implementation;
    std::unique_ptr<implementation> impl_;

  public:
    RhythmAudioTransport();
    ~RhythmAudioTransport();
    bool start(const RhythmAudioConfig &);
    bool start_pcm(std::shared_ptr<const RhythmPCM>, const std::string &label);
    void stop();
    double seconds() const;
    void set_volume(float);
    RhythmAudioMode mode() const;
    std::string diagnostic() const;
};
} // namespace al
} // namespace gb
