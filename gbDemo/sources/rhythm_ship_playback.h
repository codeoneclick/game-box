#pragma once
#include "fake_leaderboard.h"
#include "rhythm_audio_transport.h"
#include "rhythm_calibration.h"
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
    bool loading_ = false, calibrating_ = false;
    double offset_ = 0;
    RhythmCalibration calibration_;
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
    bool scoring_active() const
    {
        return !loading_ && !calibrating_;
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
    std::string status() const;
};
} // namespace game
