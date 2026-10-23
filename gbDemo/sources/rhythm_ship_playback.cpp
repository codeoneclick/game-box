#include "rhythm_ship_playback.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
namespace game
{
RhythmShipPlayback::RhythmShipPlayback(std::shared_ptr<RhythmSession> s,
                                       std::shared_ptr<gb::al::RhythmAudioTransport> a,
                                       std::shared_ptr<FakeLeaderboard> b,
                                       std::vector<RhythmTrackEntry> catalog,
                                       std::function<std::string(const std::string &)> resolver,
                                       std::string preferences)
    : session_(s), audio_(a), bots_(b), catalog_(std::move(catalog)), resolve_(resolver),
      preferences_(preferences)
{
    if (catalog_.empty())
    {
        RhythmTrackEntry entry;
        entry.title = "Metronome";
        entry.track = default_rhythm_track();
        catalog_.push_back(entry);
    }
    std::string error;
    load_rhythm_calibration(preferences_, offset_, error);
}
void RhythmShipPlayback::publish(const RhythmTrack &track,
                                 std::shared_ptr<const gb::al::RhythmPCM> pcm)
{
    const auto windows = session_->windows();
    audio_->stop();
    *session_ = RhythmSession(track);
    session_->set_windows(windows);
    bots_->restart();
    bool started =
        pcm ? audio_->start_pcm(pcm, "MP3 / " + std::to_string(pcm->sample_rate) + " Hz / " +
                                         std::to_string(pcm->channels) + " ch / sample clock")
            : audio_->start({track.duration, resolve_rhythm_beats(track), ""});
    loading_ = false;
    if (!started)
        status_ = "Audio start failed: " + audio_->diagnostic();
    std::cout << "Rhythm restart bpm=" << track.bpm << " id=" << track.id
              << " duration=" << track.duration << " " << audio_->diagnostic() << std::endl;
}
void RhythmShipPlayback::select(size_t index)
{
    notice_.clear();
    calibrating_ = false;
    generation_ = 0;
    selected_ = index % catalog_.size();
    const auto &entry = catalog_[selected_];
    audio_->stop();
    if (entry.track.audio_file.empty())
    {
        active_pcm_.reset();
        pcm_id_.clear();
        status_ = entry.title;
        publish(entry.track, nullptr);
        return;
    }
    if (active_pcm_ && pcm_id_ == entry.track.id)
    {
        status_ = entry.title;
        publish(entry.track, active_pcm_);
        return;
    }
    active_pcm_.reset();
    pcm_id_.clear();
    loading_ = true;
    status_ = "Loading: " + entry.title;
    generation_ = loader_.request(resolve_(entry.track.audio_file));
}
void RhythmShipPlayback::next(int direction)
{
    select((selected_ + catalog_.size() + (direction < 0 ? -1 : 1)) % catalog_.size());
}
void RhythmShipPlayback::restart(const RhythmTrack &track)
{
    if (loading_ || calibrating_)
        return;
    if (!track.audio_file.empty() && (!active_pcm_ || pcm_id_ != track.id))
    {
        select(selected_);
        return;
    }
    publish(track, active_pcm_);
}
void RhythmShipPlayback::update()
{
    RhythmLoadResult result;
    if (loader_.poll(result) && loading_ && result.generation == generation_)
    {
        RhythmTrack track;
        std::string error = result.error;
        if (result.pcm &&
            prepare_rhythm_track(catalog_[selected_], result.pcm->duration(), track, error))
        {
            active_pcm_ = result.pcm;
            pcm_id_ = track.id;
            status_ = catalog_[selected_].title;
            publish(track, active_pcm_);
        }
        else
        {
            status_ = "Metronome fallback";
            notice_ = "Cannot load selected MP3: " + error;
            std::cerr << "Rhythm fallback: " << error << std::endl;
            publish(default_rhythm_track(), nullptr);
        }
    }
    if (calibrating_ && audio_->seconds() > 14)
        finish_calibration();
}
void RhythmShipPlayback::tap(double raw)
{
    if (calibrating_)
    {
        calibration_.tap(raw);
        if (calibration_.progress() == 24)
            finish_calibration();
        return;
    }
    if (scoring_active())
        session_->enqueue_tap(raw - offset_);
}
void RhythmShipPlayback::begin_calibration()
{
    generation_ = 0;
    loading_ = false;
    calibrating_ = true;
    notice_.clear();
    calibration_.begin();
    audio_->stop();
    std::vector<double> beats;
    for (int i = 0; i < 28; ++i)
        beats.push_back(i * .5);
    audio_->start({16, beats, ""});
    status_ = "Calibration";
    if (std::getenv("GB_RHYTHM_DIAGNOSTICS"))
        std::cout << "Rhythm calibration begin" << std::endl;
}
void RhythmShipPlayback::finish_calibration()
{
    const auto result = calibration_.finish();
    if (std::getenv("GB_RHYTHM_DIAGNOSTICS"))
        std::cout << "Rhythm calibration accepted=" << result.accepted
                  << " offset_ms=" << result.offset * 1000 << std::endl;
    if (result.accepted)
    {
        if (set_calibration_offset(result.offset))
            notice_ = result.diagnostic;
    }
    else
        notice_ = result.diagnostic;
    calibrating_ = false;
    const auto message = notice_;
    select(selected_);
    notice_ = message;
}
void RhythmShipPlayback::cancel_calibration()
{
    if (calibrating_)
    {
        calibrating_ = false;
        select(selected_);
        notice_ = "Calibration cancelled";
        if (std::getenv("GB_RHYTHM_DIAGNOSTICS"))
            std::cout << "Rhythm calibration cancelled" << std::endl;
    }
}
bool RhythmShipPlayback::set_calibration_offset(double offset)
{
    std::string error;
    if (!save_rhythm_calibration(preferences_, offset, error))
    {
        notice_ = error;
        return false;
    }
    offset_ = offset;
    notice_.clear();
    if (std::getenv("GB_RHYTHM_DIAGNOSTICS"))
        std::cout << "Rhythm calibration saved offset_ms=" << offset * 1000 << std::endl;
    return true;
}
std::string RhythmShipPlayback::status() const
{
    if (calibrating_)
    {
        const double t = audio_->seconds();
        return t < 1.75 ? "CALIBRATION: listen to 4 count-in clicks"
                        : "CALIBRATION: SPACE / CLICK  " + std::to_string(calibration_.progress()) +
                              " / 24";
    }
    return status_ + (notice_.empty() ? "" : " | " + notice_);
}
} // namespace game
