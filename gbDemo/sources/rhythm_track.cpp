#include "rhythm_track.h"
#include "rhythm_json.h"
#include <cmath>
#include <fstream>
#include <jsoncpp-1.7.2/json/json.h>
#include <stdexcept>
namespace game
{
RhythmTrack default_rhythm_track()
{
    return {};
}
std::vector<double> resolve_rhythm_beats(const RhythmTrack &t)
{
    if (!std::isfinite(t.bpm) || t.bpm <= 0 || t.bpm > 1000 || !std::isfinite(t.duration) ||
        t.duration <= .26 || t.duration > 3600 || !std::isfinite(t.offset) ||
        std::abs(t.offset) > t.duration)
        throw std::invalid_argument("Invalid rhythm track timing");
    std::vector<double> result;
    if (t.beats.empty())
    {
        double step = 60 / t.bpm;
        for (double b = t.offset + step; b <= t.duration + 1e-9; b += step)
            if (b > 0)
                result.push_back(std::min(b, t.duration));
    }
    else
        for (double b : t.beats)
        {
            b += t.offset;
            if (!std::isfinite(b) || b < 0 || b >= t.duration ||
                (!result.empty() && b <= result.back()))
                throw std::invalid_argument("Invalid explicit beat array");
            result.push_back(b);
        }
    if (result.empty())
        throw std::invalid_argument("Empty beat timeline");
    double gap = t.duration - result.back() + result.front();
    for (size_t i = 1; i < result.size(); ++i)
        gap = std::min(gap, result[i] - result[i - 1]);
    if (gap <= .26)
        throw std::invalid_argument("Beat spacing must exceed 260 ms");
    return result;
}
RhythmBeat rhythm_beat_at(const RhythmTrack &t, std::uint64_t id)
{
    const auto b = resolve_rhythm_beats(t);
    return {id, (id / b.size()) * t.duration + b[id % b.size()]};
}
std::vector<RhythmEvent> resolve_rhythm_events(const RhythmTrack &t)
{
    const auto beats = resolve_rhythm_beats(t);
    std::vector<RhythmEvent> result;
    if (t.events.empty())
    {
        for (size_t i = 0; i < beats.size(); ++i)
            result.push_back(RhythmEvent(beats[i], RhythmAction::Tap, i));
        return result;
    }
    if (t.events.size() > 60000)
        throw std::invalid_argument("Too many rhythm events");
    for (auto e : t.events)
    {
        e.time += t.offset;
        if (!std::isfinite(e.time) || e.time < 0 || e.time >= t.duration ||
            static_cast<unsigned>(e.action) >= 5 ||
            (!result.empty() && e.time - result.back().time < .060 - 1e-9))
            throw std::invalid_argument("Invalid/too dense rhythm events");
        result.push_back(e);
    }
    if (t.duration - result.back().time + result.front().time < .060 - 1e-9)
        throw std::invalid_argument("Dense event loop seam");
    return result;
}
bool load_rhythm_track(const std::string &path, RhythmTrack &out, std::string &error)
{
    try
    {
        Json::Value v;
        if (!read_rhythm_json(path, v, error))
            throw std::invalid_argument("Cannot parse rhythm track: " + path);
        if (!v.isObject())
            throw std::invalid_argument("Rhythm track root must be an object");
        RhythmTrack t;
        for (const auto *key : {"bpm", "offset", "duration"})
            if (v.isMember(key) && !v[key].isNumeric())
                throw std::invalid_argument(std::string("Expected number: ") + key);
        if (v.isMember("track"))
        {
            if (!v["track"].isString())
                throw std::invalid_argument("Expected track string");
            t.id = v["track"].asString();
        }
        if (v.isMember("audio_file"))
        {
            if (!v["audio_file"].isString())
                throw std::invalid_argument("Expected audio_file string");
            t.audio_file = v["audio_file"].asString();
        }
        t.bpm = v.get("bpm", 128).asDouble();
        t.offset = v.get("offset", 0).asDouble();
        t.duration = v.get("duration", 30).asDouble();
        if (v.isMember("beats"))
        {
            if (!v["beats"].isArray() || v["beats"].empty())
                throw std::invalid_argument("Expected nonempty beat array");
            for (const auto &b : v["beats"])
            {
                if (!b.isNumeric())
                    throw std::invalid_argument("Expected beat number");
                t.beats.push_back(b.asDouble());
            }
        }
        if (v.isMember("choreography"))
        {
            if (!v["choreography"].isString() || (v["choreography"].asString() != "phrases" &&
                                                  v["choreography"].asString() != "legacy"))
                throw std::invalid_argument("Invalid choreography mode");
            t.generated_choreography = v["choreography"].asString() == "phrases";
        }
        if (v.isMember("events"))
        {
            if (!v["events"].isArray() || v["events"].empty() || v["events"].size() > 60000)
                throw std::invalid_argument("Invalid event list");
            for (const auto &e : v["events"])
            {
                RhythmAction a;
                if (!e.isObject() || !e["time"].isNumeric() || !e["action"].isString() ||
                    !parse_rhythm_action(e["action"].asString(), a))
                    throw std::invalid_argument("Invalid event");
                t.events.push_back(RhythmEvent(e["time"].asDouble(), a));
            }
            t.generated_choreography = false; // Explicit authored events are authoritative.
        }
        resolve_rhythm_beats(t);
        resolve_rhythm_events(t);
        out = t;
        error.clear();
        return true;
    }
    catch (const std::exception &e)
    {
        error = e.what();
        return false;
    }
}
} // namespace game
