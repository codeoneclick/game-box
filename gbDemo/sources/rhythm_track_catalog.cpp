#include "rhythm_track_catalog.h"
#include "rhythm_json.h"
#include <cmath>
#include <fstream>
#include <jsoncpp-1.7.2/json/json.h>
#include <set>
#include <stdexcept>
namespace game
{
namespace
{
bool basename(const std::string &s)
{
    return !s.empty() && s.find_first_of("/\\:") == std::string::npos && s != "." && s != "..";
}
} // namespace
bool load_rhythm_catalog(const std::string &path, std::vector<RhythmTrackEntry> &out,
                         std::string &error)
{
    try
    {
        Json::Value root;
        if (!read_rhythm_json(path, root, error) || !root.isObject() || !root["tracks"].isArray() ||
            root["tracks"].empty() || root["tracks"].size() > 32)
            throw std::runtime_error("Invalid music catalog: " + path);
        auto split = path.find_last_of("/\\");
        const std::string directory = split == std::string::npos ? "" : path.substr(0, split + 1);
        std::vector<RhythmTrackEntry> entries;
        std::set<std::string> ids;
        for (const auto &item : root["tracks"])
        {
            if (!item.isObject() || !item["title"].isString() || !item["map"].isString())
                throw std::runtime_error("Invalid catalog entry");
            RhythmTrackEntry e;
            e.title = item["title"].asString();
            e.map_file = item["map"].asString();
            if (e.title.empty() || e.title.size() > 100 || !basename(e.map_file) ||
                !load_rhythm_track(directory + e.map_file, e.track, error) ||
                (!e.track.audio_file.empty() && !basename(e.track.audio_file)) ||
                !ids.insert(e.track.id).second)
                throw std::runtime_error("Invalid/duplicate track: " + e.map_file + " " + error);
            entries.push_back(e);
        }
        out = std::move(entries);
        error.clear();
        return true;
    }
    catch (const std::exception &e)
    {
        error = e.what();
        return false;
    }
}
bool prepare_rhythm_track(const RhythmTrackEntry &e, double duration, RhythmTrack &out,
                          std::string &error)
{
    try
    {
        if (!std::isfinite(duration) || std::abs(duration - e.track.duration) > .025)
            throw std::runtime_error("Decoded duration differs from authored map");
        auto t = e.track;
        t.duration = duration;
        resolve_rhythm_beats(t);
        resolve_rhythm_events(t);
        resolve_rhythm_accents(t);
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
