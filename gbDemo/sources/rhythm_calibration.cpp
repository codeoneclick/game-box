#include "rhythm_calibration.h"
#include "rhythm_json.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <jsoncpp-1.7.2/json/json.h>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif
namespace game
{
namespace
{
double median(std::vector<double> v)
{
    std::sort(v.begin(), v.end());
    return v.empty() ? 0 : (v[(v.size() - 1) / 2] + v[v.size() / 2]) * .5;
}
bool valid(double v)
{
    return std::isfinite(v) && std::abs(v) <= .25;
}
} // namespace
void RhythmCalibration::tap(double t)
{
    if (!std::isfinite(t) || t < 1.75 || t > 13.75)
        return;
    const int pulse = static_cast<int>(std::floor(t / .5 + .5));
    if (pulse < 4 || pulse >= 28 ||
        std::find(pulses_.begin(), pulses_.end(), pulse) != pulses_.end())
        return;
    const double error = t - pulse * .5;
    if (!valid(error))
        return;
    pulses_.push_back(pulse);
    errors_.push_back(error);
}
RhythmCalibrationResult RhythmCalibration::finish() const
{
    RhythmCalibrationResult r;
    r.diagnostic = "Incomplete: tap all 24 pulses";
    if (errors_.size() < 24)
        return r;
    const double center = median(errors_);
    std::vector<double> deviations;
    for (double x : errors_)
        deviations.push_back(std::abs(x - center));
    r.jitter = median(deviations);
    if (r.jitter > .030)
    {
        r.diagnostic = "Timing varies too much: retry";
        return r;
    }
    std::vector<double> inliers;
    for (double x : errors_)
        if (std::abs(x - center) <= std::max(.015, 3 * r.jitter))
            inliers.push_back(x);
    if (inliers.size() < 20)
    {
        r.diagnostic = "Too few consistent taps: retry";
        return r;
    }
    r.offset = median(inliers);
    r.accepted = valid(r.offset);
    r.diagnostic = r.accepted ? "Calibration saved" : "Offset outside range";
    return r;
}
bool load_rhythm_calibration(const std::string &path, double &out, std::string &error)
{
    Json::Value v;
    if (!read_rhythm_json(path, v, error) || !v.isObject() || !v["offset_ms"].isNumeric() ||
        !valid(v["offset_ms"].asDouble() / 1000))
    {
        error = "Invalid/missing audio calibration";
        return false;
    }
    out = v["offset_ms"].asDouble() / 1000;
    error.clear();
    return true;
}
bool save_rhythm_calibration(const std::string &path, double offset, std::string &error)
{
    if (!valid(offset))
    {
        error = "Invalid calibration offset";
        return false;
    }
    Json::Value v;
    v["version"] = 1;
    v["offset_ms"] = offset * 1000;
    // A failed write never overwrites the previous preferences.
    const auto temporary = path + ".tmp";
    {
        std::ofstream f(temporary, std::ios::binary | std::ios::trunc);
        f << v;
        f.flush();
        f.close();
        if (f.fail())
        {
            std::remove(temporary.c_str());
            error = "Cannot save audio calibration";
            return false;
        }
    }
#ifdef _WIN32
    if (!MoveFileExA(temporary.c_str(), path.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        std::remove(temporary.c_str());
        error = "Cannot publish calibration";
        return false;
    }
#else
    if (std::rename(temporary.c_str(), path.c_str()) != 0)
    {
        error = "Cannot publish calibration";
        return false;
    }
#endif
    error.clear();
    return true;
}
} // namespace game
