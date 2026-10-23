#include "rhythm_pcm.h"
#define DR_MP3_IMPLEMENTATION
#define DR_MP3_NO_SIMD
#include "../libraries/dr_mp3/dr_mp3.h"
#include <algorithm>
#include <fstream>
#include <memory>
#include <stdexcept>
namespace gb
{
namespace al
{
namespace
{
// Strict Layer III framing: dr_mp3 deliberately accepts playable prefixes.
// Reject cut frames so a corrupt download cannot silently become a short song.
// Whole-frame truncation with no declared length is detected later by the map.
void validate_frames(const std::vector<unsigned char> &bytes)
{
    size_t pos = 0, end = bytes.size(), frames = 0;
    unsigned source_rate = 0, source_channels = 0;
    if (end >= 10 && bytes[0] == 'I' && bytes[1] == 'D' && bytes[2] == '3')
    {
        size_t size = 0;
        for (int i = 6; i < 10; ++i)
        {
            if (bytes[i] & 128)
                throw std::runtime_error("Invalid ID3 size");
            size = (size << 7) | bytes[i];
        }
        pos = 10 + size + ((bytes[3] == 4 && (bytes[5] & 16)) ? 10 : 0);
        if (pos > end)
            throw std::runtime_error("Truncated ID3");
    }
    if (end >= 128 && bytes[end - 128] == 'T' && bytes[end - 127] == 'A' && bytes[end - 126] == 'G')
        end -= 128;
    const int rates[] = {44100, 48000, 32000};
    const int bitrates1[] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320};
    const int bitrates2[] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160};
    while (pos < end)
    {
        if (end - pos < 4)
            throw std::runtime_error("Truncated MP3 frame");
        const auto a = bytes[pos + 1], b = bytes[pos + 2];
        const unsigned version = (a >> 3) & 3, layer = (a >> 1) & 3, bitrate = b >> 4,
                       sample = (b >> 2) & 3;
        if (bytes[pos] != 255 || (a & 224) != 224 || version == 1 || layer != 1 || !bitrate ||
            bitrate == 15 || sample == 3)
            throw std::runtime_error("Invalid/unsupported MP3 frame or trailing metadata");
        const unsigned rate = rates[sample] / (version == 3 ? 1 : version == 2 ? 2 : 4);
        const unsigned channels = (bytes[pos + 3] >> 6) == 3 ? 1 : 2;
        if (frames && (rate != source_rate || channels != source_channels))
            throw std::runtime_error("Changing MP3 format is unsupported");
        source_rate = rate;
        source_channels = channels;
        const size_t length = (version == 3 ? 144000 : 72000) *
                                  (version == 3 ? bitrates1[bitrate] : bitrates2[bitrate]) / rate +
                              ((b >> 1) & 1);
        if (length < 4 || length > end - pos)
            throw std::runtime_error("Truncated MP3 frame");
        pos += length;
        ++frames;
    }
    if (frames < 2)
        throw std::runtime_error("Too few MP3 frames");
}
} // namespace
double RhythmPositionCounter::seconds(std::uint32_t value, RhythmPositionUnit unit, unsigned rate,
                                      unsigned channels)
{
    if (!rate || !channels)
        return last_;
    const double scale =
        unit == RhythmPositionUnit::milliseconds
            ? 1000.0
            : double(rate) * (unit == RhythmPositionUnit::bytes ? channels * 2 : 1);
    if (initialized_ && unit_ != unit)
    {
        wraps_ = 0;
        bias_ = last_ - value / scale;
    }
    else if (initialized_ && value < previous_ && previous_ - value > 0x80000000u)
        wraps_ += (1ull << 32);
    initialized_ = true;
    unit_ = unit;
    previous_ = value;
    last_ = std::max(last_, bias_ + double(wraps_ + value) / scale);
    return last_;
}
bool decode_rhythm_mp3(const std::string &path, RhythmPCM &out, std::string &error)
{
    try
    {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file || file.tellg() <= 0 || file.tellg() > 64 * 1024 * 1024)
            throw std::runtime_error("Missing/oversized MP3: " + path);
        const auto size = static_cast<size_t>(file.tellg());
        file.seekg(0);
        std::vector<unsigned char> bytes(size);
        file.read(reinterpret_cast<char *>(bytes.data()), size);
        if (!file)
            throw std::runtime_error("Cannot read MP3");
        validate_frames(bytes);
        drmp3 mp3;
        if (!drmp3_init_memory(&mp3, bytes.data(), bytes.size(), nullptr))
            throw std::runtime_error("Invalid MP3: " + path);
        const auto cleanup = [&mp3](void *) { drmp3_uninit(&mp3); };
        std::unique_ptr<void, decltype(cleanup)> guard(&mp3, cleanup);
        if ((mp3.channels != 1 && mp3.channels != 2) || mp3.sampleRate < 8000 ||
            mp3.sampleRate > 192000)
            throw std::runtime_error("Unsupported MP3 format");
        const auto declared = mp3.totalPCMFrameCount;
        RhythmPCM pcm;
        pcm.channels = mp3.channels;
        pcm.sample_rate = mp3.sampleRate;
        const std::uint64_t limit = std::min<std::uint64_t>(std::uint64_t(pcm.sample_rate) * 600,
                                                            128 * 1024 * 1024 / (pcm.channels * 2));
        if (declared != DRMP3_UINT64_MAX && declared > limit)
            throw std::runtime_error("MP3 exceeds decoded limit");
        std::int16_t block[8192];
        for (;;)
        {
            auto frames = drmp3_read_pcm_frames_s16(&mp3, 4096, block);
            if (!frames)
                break;
            if (pcm.samples.size() / pcm.channels + frames > limit)
                throw std::runtime_error("MP3 exceeds decoded limit");
            pcm.samples.insert(pcm.samples.end(), block, block + frames * pcm.channels);
        }
        if (pcm.duration() <= .26 ||
            (declared != DRMP3_UINT64_MAX && pcm.samples.size() / pcm.channels != declared))
            throw std::runtime_error("Empty/truncated MP3");
        out = std::move(pcm);
        error.clear();
        return true;
    }
    catch (const std::exception &e)
    {
        error = e.what();
        return false;
    }
}
} // namespace al
} // namespace gb
