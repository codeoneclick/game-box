#include "rhythm_ship_playback.h"
#include "rhythm_crowd_choreography.h"
#include "rhythm_prompt_model.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace game;
int main()
{
    _putenv_s("GB_RHYTHM_SILENT", "1");
    std::remove("sync-preferences.json");
    RhythmTrackEntry entry;
    entry.track.duration = 10;
    entry.track.beats = {2, 4, 6, 8};
    entry.track.events = {RhythmEvent(2, RhythmAction::Tap)};
    auto session = std::make_shared<RhythmSession>(entry.track);
    auto audio = std::make_shared<gb::al::RhythmAudioTransport>();
    auto bots = std::make_shared<FakeLeaderboard>();
    RhythmShipPlayback playback(session, audio, bots, {entry},
                                [](const std::string &s) { return s; }, "sync-preferences.json");
    playback.select(0);
    assert(playback.calibration_offset() == 0);
    for (double offset : {.3, -.3, .6, -.6, 0.})
    {
        // Reconfigure after the old session advanced. A backward corrected time
        // must start a fresh epoch, including clearing score and local animation.
        session->update(5);
        auto epoch = playback.epoch();
        assert(playback.set_calibration_offset(offset));
        assert(playback.epoch() > epoch && session->score().judgements == 0);
        RhythmInput local;
        assert(!playback.last_input(local));
        assert(std::abs(playback.raw_to_song_time(2 + offset) - 2) < 1e-8);
        if (offset > 0)
        {
            assert(playback.raw_to_song_time(0) < 0);
            assert(playback.song_seconds() == 0); // initial latency interval
        }
        const double song = playback.raw_to_song_time(2 + offset);
        const auto prompt = rhythm_prompt_state(*session, song, 1, 1);
        assert(prompt.visible && std::abs(prompt.current.event.time - 2) < 1e-8 &&
               std::abs(prompt.progress - 1) < 1e-8);
        assert(rhythm_prompt_state(*session, song + 1e-9, 1, 1).beat_pulse > .999);
        playback.tap(2 + offset);
        assert(playback.last_input(local) && std::abs(local.time - 2) < 1e-8);
        RhythmCrowdChoreography crowd;
        crowd.restart(1337);
        crowd.update(song, *session, 40);
        crowd.local_input(local);
        assert(std::abs(crowd.dancer_state(0, song).start - 2) < 1e-8);
        session->update(song);
        RhythmResolution resolution;
        assert(session->last_resolution(resolution) &&
               resolution.judgement == RhythmJudgement::perfect &&
               std::abs(resolution.input_time - 2) < 1e-8 &&
               std::abs(resolution.presentation_time - 2) < 1e-8 &&
               session->score().score == 100);
        // The existing version-1 preference schema still round-trips.
        double loaded = 99;
        std::string error;
        assert(load_rhythm_calibration("sync-preferences.json", loaded, error) &&
               std::abs(loaded - offset) < 1e-8);
    }
    auto epoch = playback.epoch();
    assert(!playback.set_calibration_offset(.7) && playback.epoch() == epoch);
    assert(playback.set_calibration_offset(0) && playback.epoch() == epoch);
    for (double bias : {.3, .6, -.6})
    {
        RhythmCalibration calibration;
        calibration.begin();
        calibration.tap(4.5 + bias); // fourth count-in pulse is never sampled
        for (int i = 0; i < 24; ++i)
        {
            calibration.tap((4 + i) * 1.5 + bias);
            calibration.tap((4 + i) * 1.5 + bias); // duplicate ignored
        }
        auto result = calibration.finish();
        assert(calibration.progress() == 24 && result.accepted &&
               std::abs(result.offset - bias) < 1e-8);
    }
    // Calibration receives raw clicks even while the previous AV offset is set.
    assert(playback.set_calibration_offset(.4));
    playback.begin_calibration();
    assert(playback.calibrating() && !playback.scoring_active());
    for (int i = 0; i < 24; ++i)
        playback.tap((4 + i) * 1.5 + .3);
    assert(!playback.calibrating() && playback.scoring_active() &&
           std::abs(playback.calibration_offset() - .3) < 1e-8 &&
           session->score().judgements == 0);
    std::remove("sync-preferences.json");
    std::cout << "AV sync checks passed: raw 2.3 / offset .3 -> prompt, dance, judgement 2.0; "
                 "signed offsets, epoch resets, raw calibration and version-1 preferences\n";
}
