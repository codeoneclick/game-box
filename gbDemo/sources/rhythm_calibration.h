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
    // The spacing exceeds twice the supported delay, avoiding half-second
    // phase wrapping on Bluetooth output while retaining all 24 measurements.
    static constexpr double pulse_interval = 1.5;
    static constexpr double maximum_offset = .6;
    static constexpr int count_in_pulses = 4;
    static constexpr int tap_count = 24;
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
