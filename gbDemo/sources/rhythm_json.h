#pragma once
#include <fstream>
#include <jsoncpp-1.7.2/json/json.h>
#include <sstream>
namespace game
{
// JsonCpp 1.7 failIfExtra alone accepts some invalid trailing tokens. Like the
// graphics settings loader, parse inside an envelope to validate the whole file.
inline bool read_rhythm_json(const std::string &path, Json::Value &out, std::string &error)
{
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f || f.tellg() <= 0 || f.tellg() > 4 * 1024 * 1024)
    {
        error = "Missing/oversized rhythm JSON: " + path;
        return false;
    }
    std::string text(static_cast<size_t>(f.tellg()), '\0');
    f.seekg(0);
    f.read(&text[0], text.size());
    if (!f)
    {
        error = "Cannot read rhythm JSON: " + path;
        return false;
    }
    if (text.compare(0, 3, "\xEF\xBB\xBF") == 0)
        text.erase(0, 3);
    std::istringstream wrapped("[" + text + "]");
    Json::Value envelope;
    Json::CharReaderBuilder reader;
    reader["collectComments"] = false;
    reader["allowComments"] = false;
    reader["failIfExtra"] = true;
    reader["rejectDupKeys"] = true;
    if (!Json::parseFromStream(reader, wrapped, &envelope, &error) || !envelope.isArray() ||
        envelope.size() != 1)
    {
        if (error.empty())
            error = "Expected one rhythm JSON document";
        return false;
    }
    out = envelope[0];
    error.clear();
    return true;
}
} // namespace game
