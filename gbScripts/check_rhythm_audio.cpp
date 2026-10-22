#include "rhythm_audio_transport.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>
int main()
{
    using namespace gb::al;
    bool rejected = false;
    try
    {
        make_rhythm_pcm({1e-8, {}, ""});
    }
    catch (const std::invalid_argument &)
    {
        rejected = true;
    }
    assert(rejected);
    assert(unwrap_rhythm_playback_seconds(10, 70, 30, 5) == 70);
    assert(unwrap_rhythm_playback_seconds(5, 65, 30, 35) == 65);
    assert(unwrap_rhythm_playback_seconds(9, 9, 30, 10) == 10);
    RhythmAudioConfig c{1, {.46875, 1}, ""};
    auto pcm = make_rhythm_pcm(c);
    assert(pcm.size() == 48000);
    assert(pcm[22510] != 0 && pcm[10] != 0 && pcm[18000] == 0);
    RhythmAudioTransport a;
    _putenv_s("GB_RHYTHM_SILENT", "1");
    assert(a.start(c));
    assert(a.mode() == RhythmAudioMode::silent);
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    assert(a.seconds() >= .02);
    a.stop();
    a.stop();
    a.start(c);
    assert(a.seconds() < .02);
    _putenv_s("GB_RHYTHM_SILENT", "");
    a.start(c);
    for (int i = 0; i < 30 && a.seconds() < .02; ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << a.seconds() << " " << a.diagnostic() << std::endl;
    }
    assert(a.seconds() > .02);
    std::cout << "Audio transport checks passed: " << a.diagnostic() << "\n";
}
