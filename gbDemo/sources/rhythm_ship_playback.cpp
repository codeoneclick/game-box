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
    auto candidate = track;
    if (candidate.generated_choreography)
    {
        candidate.events.clear();
        std::string error;
        RhythmTrack generated;
        if (!generate_rhythm_choreography(candidate, choreography_, generated, error))
        {
            notice_ = error;
            return;
        }
        source_track_ = candidate;
        candidate = std::move(generated);
    }
    else
        source_track_ = candidate;
    const auto windows = session_->windows();
    RhythmSession prepared(candidate);
    prepared.set_windows(windows);
    audio_->stop();
    *session_ = std::move(prepared);
    bots_->restart();
    bool started =
        pcm ? audio_->start_pcm(pcm, "MP3 / " + std::to_string(pcm->sample_rate) + " Hz / " +
                                         std::to_string(pcm->channels) + " ch / sample clock")
            : audio_->start({track.duration, resolve_rhythm_beats(track), ""});
    loading_ = false;
    ++epoch_;
    has_input_ = false;
    if (!started)
        status_ = "Audio start failed: " + audio_->diagnostic();
    std::cout << "Rhythm restart bpm=" << track.bpm << " id=" << track.id
              << " duration=" << track.duration << " " << audio_->diagnostic() << std::endl;
}
void RhythmShipPlayback::select(size_t index)
{
    ++epoch_;
    has_input_ = false;
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
            auto fallback = default_rhythm_track();
            fallback.generated_choreography = true;
            publish(fallback, nullptr);
        }
    }
    if (calibrating_ && audio_->seconds() > 14)
        finish_calibration();
}
void RhythmShipPlayback::tap(double raw)
{
    input(RhythmInput(raw, RhythmAction::Tap));
}
void RhythmShipPlayback::input(RhythmInput value)
{
    const double raw = value.time;
    if (calibrating_)
    {
        if (value.action != RhythmAction::Tap)
            return;
        calibration_.tap(raw);
        if (calibration_.progress() == 24)
            finish_calibration();
        return;
    }
    if (scoring_active())
    {
        last_input_ = value;
        has_input_ = true;
        value.time -= offset_;
        session_->enqueue_input(value);
    }
}
bool RhythmShipPlayback::apply_choreography(const RhythmChoreographySettings &settings,
                                            bool force_authored)
{
    auto source = loading_ ? catalog_[selected_].track : source_track_;
    if (source.id.empty())
        source = catalog_[selected_].track;
    if (!source.generated_choreography && !source.events.empty() && !force_authored)
    {
        notice_ = "Authored events: use Generate to replace";
        return false;
    }
    source.events.clear();
    source.generated_choreography = true;
    RhythmTrack generated;
    std::string error;
    if (!generate_rhythm_choreography(source, settings, generated, error))
    {
        notice_ = error;
        return false;
    }
    choreography_ = settings;
    notice_.clear();
    if (force_authored)
        catalog_[selected_].track = source;
    if (loading_ || calibrating_)
        return true;
    publish(source, active_pcm_);
    return true;
}
void RhythmShipPlayback::begin_calibration()
{
    ++epoch_;
    has_input_ = false;
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
