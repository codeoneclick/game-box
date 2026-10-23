#pragma once
#include <string>
#include <vector>
namespace game
{
struct RhythmCalibrationResult
{
    bool accepted = false;
    double offset = 0, jitter = 0;
    std::string diagnostic;
};
class RhythmCalibration
{
    std::vector<double> errors_;
    std::vector<int> pulses_;

  public:
    void begin()
    {
        errors_.clear();
        pulses_.clear();
    }
    void tap(double);
    RhythmCalibrationResult finish() const;
    int progress() const
    {
        return static_cast<int>(errors_.size());
    }
};
bool load_rhythm_calibration(const std::string &, double &, std::string &);
bool save_rhythm_calibration(const std::string &, double, std::string &);
} // namespace game
