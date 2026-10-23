#include "rhythm_audio_transport.h"
#include "rhythm_pcm.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <fstream>
#include <cmath>
int main()
{
    using namespace gb::al;
    RhythmPCM decoded;
    std::string decode_error;
    assert(!decode_rhythm_mp3("missing.mp3",decoded,decode_error));
    assert(!decode_error.empty());
    {
        std::ifstream source("../../../gbResources/audioera-stylish-trailer-percussion-622881.mp3",std::ios::binary);
        std::vector<char> bytes((std::istreambuf_iterator<char>(source)),std::istreambuf_iterator<char>());
        assert(bytes.size()>10000);
        {std::ofstream truncated("truncated.mp3",std::ios::binary);truncated.write(bytes.data(),bytes.size()/2);}
        assert(!decode_rhythm_mp3("truncated.mp3",decoded,decode_error));
        {std::ofstream invalid("invalid.mp3",std::ios::binary);invalid<<"this is not audio";}
        assert(!decode_rhythm_mp3("invalid.mp3",decoded,decode_error));
    }
    RhythmPositionCounter counter;
    assert(std::abs(counter.seconds(176400,RhythmPositionUnit::bytes,44100,2)-1)<1e-9);
    counter.reset();
    counter.seconds(0xfffffff0u,RhythmPositionUnit::frames,44100,2);
    assert(counter.seconds(16,RhythmPositionUnit::frames,44100,2)>4294967296.0/44100);
    counter.reset();
    assert(counter.seconds(44100,RhythmPositionUnit::frames,44100,2)==1);
    assert(counter.seconds(1000,RhythmPositionUnit::milliseconds,44100,2)==1);
    assert(counter.seconds(1100,RhythmPositionUnit::milliseconds,44100,2)==1.1);
    for(const auto &file:{"audioera-stylish-trailer-percussion-622881.mp3", "ikoliks-uplifting-action-sport-energetic-rock-619094.mp3", "strawberry_candy-sport-intense-metal-fast-and-strong-622134.mp3"}) {
        assert(decode_rhythm_mp3(std::string("../../../gbResources/")+file,decoded,decode_error));
        assert(decoded.channels==2 && decoded.sample_rate==44100);
        assert(decoded.duration()>79 && decoded.duration()<139);
        std::ofstream raw(std::string(file)+".pcm",std::ios::binary);
        raw.write(reinterpret_cast<const char*>(decoded.samples.data()),decoded.samples.size()*2);
        std::cout<<file<<" decoded="<<decoded.duration()<<" frames="<<decoded.samples.size()/2<<std::endl;
    }
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
