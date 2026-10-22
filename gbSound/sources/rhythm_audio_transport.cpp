#include "rhythm_audio_transport.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#else
#include "audio_engine.h"
#include <OpenAL/al.h>
#endif
namespace gb
{
namespace al
{
namespace
{
using Clock = std::chrono::steady_clock;
constexpr int rate = 48000;
} // namespace
double unwrap_rhythm_playback_seconds(double raw, double elapsed, double duration, double previous)
{
    if (!std::isfinite(raw) || !std::isfinite(elapsed) || !std::isfinite(duration) || raw < 0 ||
        duration <= 0)
        return previous;
    return std::max(previous,
                    raw + std::max(0.0, std::floor((elapsed - raw) / duration + .5)) * duration);
}
std::vector<std::int16_t> make_rhythm_pcm(const RhythmAudioConfig &c)
{
    if (!std::isfinite(c.duration) || c.duration <= 0 || c.duration > 3600)
        throw std::invalid_argument("Invalid audio duration");
    if (std::llround(c.duration * rate) < 1)
        throw std::invalid_argument("Audio duration is less than one sample");
    std::vector<std::int16_t> pcm(static_cast<size_t>(std::llround(c.duration * rate)), 0);
    for (size_t b = 0; b < c.beats.size(); ++b)
    {
        double t = c.beats[b];
        if (!std::isfinite(t) || t < 0 || t > c.duration)
            throw std::invalid_argument("Invalid click time");
        size_t start = static_cast<size_t>(std::llround(t * rate)) % pcm.size();
        for (int j = 0; j < 2400; ++j)
        {
            size_t i = (start + j) % pcm.size();
            double x = j / double(rate), v = std::sin(x * 6.283185307 * (b % 4 ? 1100 : 1700)) *
                                             std::exp(-x * 110) * 16000;
            pcm[i] = static_cast<std::int16_t>(std::max(-32767., std::min(32767., pcm[i] + v)));
        }
    }
    return pcm;
}
struct RhythmAudioTransport::implementation
{
    RhythmAudioMode mode = RhythmAudioMode::silent;
    Clock::time_point origin = Clock::now();
    bool active = false;
    float volume = .65f;
    double duration = 0;
    mutable double last = 0;
    std::string diagnostic = "SILENT CLOCK";
    std::vector<std::int16_t> pcm;
#if defined(_WIN32)
    HWAVEOUT device = nullptr;
    WAVEHDR header = {};
    bool prepared = false;
    mutable std::uint32_t previous = 0;
    mutable std::uint64_t wraps = 0;
#else
    ALuint source = 0, buffer = 0;
    mutable double cycle = 0, previous = 0;
#endif
};
RhythmAudioTransport::RhythmAudioTransport() : impl_(new implementation) {}
RhythmAudioTransport::~RhythmAudioTransport() { stop(); }
void RhythmAudioTransport::stop()
{
    auto &p = *impl_;
#if defined(_WIN32)
    if (p.device)
    {
        waveOutReset(p.device);
        if (p.prepared)
            waveOutUnprepareHeader(p.device, &p.header, sizeof(p.header));
        waveOutClose(p.device);
    }
    p.device = nullptr;
    p.prepared = false;
    p.header = {};
    p.previous = 0;
    p.wraps = 0;
#else
    if (p.source)
    {
        alSourceStop(p.source);
        alSourcei(p.source, AL_BUFFER, 0);
        alDeleteSources(1, &p.source);
    }
    if (p.buffer)
        alDeleteBuffers(1, &p.buffer);
    p.source = p.buffer = 0;
    p.cycle = p.previous = 0;
#endif
    p.pcm.clear();
    p.active = false;
    p.last = 0;
    p.mode = RhythmAudioMode::silent;
}
bool RhythmAudioTransport::start(const RhythmAudioConfig &c)
{
    stop();
    auto &p = *impl_;
    try
    {
        p.pcm = make_rhythm_pcm(c);
    }
    catch (const std::exception &e)
    {
        p.diagnostic = e.what();
        return false;
    }
    p.duration = c.duration;
    p.origin = Clock::now();
    p.active = true;
    p.diagnostic = "SILENT CLOCK: audio device unavailable";
    if (const char *silent = std::getenv("GB_RHYTHM_SILENT"))
        if (std::string(silent) == "1")
        {
            p.diagnostic = "SILENT CLOCK: forced";
            return true;
        }
#if defined(_WIN32)
    WAVEFORMATEX format = {};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 1;
    format.nSamplesPerSec = rate;
    format.wBitsPerSample = 16;
    format.nBlockAlign = 2;
    format.nAvgBytesPerSec = rate * 2;
    if (waveOutOpen(&p.device, WAVE_MAPPER, &format, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR)
    {
        p.device = nullptr;
        return true;
    }
    p.header.lpData = reinterpret_cast<LPSTR>(p.pcm.data());
    p.header.dwBufferLength = static_cast<DWORD>(p.pcm.size() * 2);
    p.header.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
    p.header.dwLoops = 0xffffffff;
    if (waveOutPrepareHeader(p.device, &p.header, sizeof(p.header)) == MMSYSERR_NOERROR)
    {
        p.prepared = true;
        if (waveOutWrite(p.device, &p.header, sizeof(p.header)) == MMSYSERR_NOERROR)
        {
            p.mode = RhythmAudioMode::playback;
            p.diagnostic = c.audio_file.empty()
                               ? "PCM metronome / sample clock"
                               : "PCM metronome (configured external track unsupported on Windows)";
            set_volume(p.volume);
            return true;
        }
    }
    waveOutReset(p.device);
    if (p.prepared)
        waveOutUnprepareHeader(p.device, &p.header, sizeof(p.header));
    waveOutClose(p.device);
    p.device = nullptr;
    p.prepared = false;
#else
    // Reuse the engine's existing OpenAL device/context, but own this PCM source
    // synchronously. Legacy audio_player startup callbacks can touch recycled
    // source IDs after stop; direct PCM avoids that race and cache accumulation.
    if (!audio_engine::init())
        return true;
    alGetError();
    alGenBuffers(1, &p.buffer);
    alGenSources(1, &p.source);
    if (alGetError() == AL_NO_ERROR && p.buffer && p.source)
    {
        alBufferData(p.buffer, AL_FORMAT_MONO16, p.pcm.data(),
                     static_cast<ALsizei>(p.pcm.size() * 2), rate);
        alSourcei(p.source, AL_BUFFER, p.buffer);
        alSourcei(p.source, AL_LOOPING, AL_TRUE);
        alSourcef(p.source, AL_GAIN, p.volume);
        alSourcePlay(p.source);
        if (alGetError() == AL_NO_ERROR)
        {
            p.mode = RhythmAudioMode::playback;
            p.diagnostic = c.audio_file.empty()
                               ? "OpenAL PCM / sample clock"
                               : "OpenAL PCM metronome (external track unsupported)";
            p.origin = Clock::now();
            return true;
        }
    }
    if (p.source)
    {
        alSourceStop(p.source);
        alDeleteSources(1, &p.source);
        p.source = 0;
    }
    if (p.buffer)
    {
        alDeleteBuffers(1, &p.buffer);
        p.buffer = 0;
    }
#endif
    return true;
}
double RhythmAudioTransport::seconds() const
{
    auto &p = *impl_;
    if (!p.active)
        return 0;
    double wall = std::chrono::duration<double>(Clock::now() - p.origin).count(), t = wall;
#if defined(_WIN32)
    if (p.device && p.mode == RhythmAudioMode::playback)
    {
        MMTIME position = {};
        position.wType = TIME_SAMPLES;
        if (waveOutGetPosition(p.device, &position, sizeof(position)) == MMSYSERR_NOERROR)
        {
            std::uint32_t raw = position.wType == TIME_SAMPLES ? position.u.sample
                                : position.wType == TIME_BYTES
                                    ? position.u.cb / 2
                                    : static_cast<std::uint32_t>(position.u.ms * 48);
            if (raw < p.previous && p.previous - raw > 0x80000000u)
                p.wraps += (1ull << 32);
            p.previous = raw;
            t = (p.wraps + raw) / double(rate);
        }
        else
        {
            waveOutReset(p.device);
            waveOutUnprepareHeader(p.device, &p.header, sizeof(p.header));
            waveOutClose(p.device);
            p.device = nullptr;
            p.prepared = false;
            p.mode = RhythmAudioMode::silent;
            p.diagnostic = "SILENT CLOCK: playback position failed";
            p.origin = Clock::now() - std::chrono::duration_cast<Clock::duration>(
                                          std::chrono::duration<double>(p.last));
            t = p.last;
        }
    }
#else
    if (p.mode == RhythmAudioMode::playback)
    {
        ALint state = 0, sample = 0;
        alGetError();
        alGetSourcei(p.source, AL_SOURCE_STATE, &state);
        alGetSourcei(p.source, AL_SAMPLE_OFFSET, &sample);
        if (alGetError() != AL_NO_ERROR || state != AL_PLAYING || sample < 0)
        {
            alSourceStop(p.source);
            p.mode = RhythmAudioMode::silent;
            p.diagnostic = "SILENT CLOCK: OpenAL playback failed";
            p.origin = Clock::now() - std::chrono::duration_cast<Clock::duration>(
                                          std::chrono::duration<double>(p.last));
            t = p.last;
        }
        else
        {
            const double raw = sample / double(rate);
            if (raw + p.duration * .5 < p.previous)
                p.cycle += p.duration;
            // Hardware position is authoritative; wall time only disambiguates
            // multiple complete wraps when updates were stalled for whole loops.
            p.previous = raw;
            t = unwrap_rhythm_playback_seconds(raw, wall, p.duration, p.last);
        }
    }
#endif
    p.last = std::max(p.last, t);
    return p.last;
}
void RhythmAudioTransport::set_volume(float v)
{
    auto &p = *impl_;
    if (!std::isfinite(v))
        return;
    p.volume = std::max(0.f, std::min(1.f, v));
#if defined(_WIN32)
    if (p.device)
    {
        DWORD x = static_cast<DWORD>(p.volume * 65535);
        waveOutSetVolume(p.device, x | (x << 16));
    }
#else
    if (p.source)
        alSourcef(p.source, AL_GAIN, p.volume);
#endif
}
RhythmAudioMode RhythmAudioTransport::mode() const { return impl_->mode; }
std::string RhythmAudioTransport::diagnostic() const { return impl_->diagnostic; }
} // namespace al
} // namespace gb
