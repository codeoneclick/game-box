#pragma once
#include "fake_leaderboard.h"
#include "rhythm_audio_transport.h"
#include "rhythm_calibration.h"
#include "rhythm_choreography.h"
#include "rhythm_session.h"
#include "rhythm_track_catalog.h"
#include "rhythm_track_loader.h"
namespace game
{
class RhythmShipPlayback
{
    std::shared_ptr<RhythmSession> session_;
    std::shared_ptr<gb::al::RhythmAudioTransport> audio_;
    std::shared_ptr<FakeLeaderboard> bots_;
    std::vector<RhythmTrackEntry> catalog_;
    std::function<std::string(const std::string &)> resolve_;
    RhythmTrackLoader loader_;
    std::shared_ptr<const gb::al::RhythmPCM> active_pcm_;
    std::string pcm_id_, preferences_, status_, notice_;
    size_t selected_ = 0;
    std::uint64_t generation_ = 0;
    bool loading_ = false, calibrating_ = false, chart_ready_ = false;
    double offset_ = 0;
    RhythmCalibration calibration_;
    RhythmTrack source_track_;
    RhythmChoreographySettings choreography_;
    std::uint64_t epoch_ = 0;
    RhythmInput last_input_;
    bool has_input_ = false;
    void publish(const RhythmTrack &, std::shared_ptr<const gb::al::RhythmPCM>);
    void finish_calibration();

  public:
    RhythmShipPlayback(std::shared_ptr<RhythmSession>,
                       std::shared_ptr<gb::al::RhythmAudioTransport>,
                       std::shared_ptr<FakeLeaderboard>, std::vector<RhythmTrackEntry>,
                       std::function<std::string(const std::string &)>, std::string preferences);
    void select(size_t);
    void next(int direction);
    void restart(const RhythmTrack &);
    void update();
    void tap(double);
    void input(RhythmInput);
    bool last_input(RhythmInput &out) const
    {
        if (!has_input_)
            return false;
        out = last_input_;
        return true;
    }
    bool apply_choreography(const RhythmChoreographySettings &, bool force_authored = false);
    const RhythmChoreographySettings &choreography_settings() const
    {
        return choreography_;
    }
    std::uint64_t epoch() const
    {
        return epoch_;
    }
    bool scoring_active() const
    {
        return chart_ready_ && !loading_ && !calibrating_;
    }
    bool calibrating() const
    {
        return calibrating_;
    }
    void begin_calibration();
    void cancel_calibration();
    bool set_calibration_offset(double seconds);
    double calibration_offset() const
    {
        return offset_;
    }
    // Input timestamps and presentation share this conversion. Negative song time
    // is retained for input rejection during the initial output-latency interval.
    double raw_to_song_time(double raw) const
    {
        return raw - offset_;
    }
    double song_seconds() const;
    std::string status() const;
};
} // namespace game
