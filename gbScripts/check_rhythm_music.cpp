#include "rhythm_calibration.h"
#include "rhythm_ship_playback.h"
#include "rhythm_track_catalog.h"
#include "rhythm_track_loader.h"
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>
using namespace game;
int main()
{
    {
        RhythmTrackEntry entry;
        entry.track.duration = 10;
        entry.track.beats = {.5, 1};
        entry.track.events = {RhythmEvent(9.99, RhythmAction::Tap)};
        RhythmTrack prepared;
        prepared.id = "preserve";
        std::string why;
        assert(!prepare_rhythm_track(entry, 9.98, prepared, why) && prepared.id == "preserve");
    }
    std::vector<RhythmTrackEntry> catalog;
    std::string error;
    assert(load_rhythm_catalog(
        "../../../gbBundle/Configurations/rhythm_ship/rhythm_ship_catalog.json", catalog, error));
    assert(catalog.size() == 10 && catalog[0].track.id == "active_sport");
    for (auto &e : catalog)
    {
        auto beats = resolve_rhythm_beats(e.track);
        assert(!beats.empty());
        if (!e.track.audio_file.empty())
        {
            assert(beats.front() < 60 / e.track.bpm && beats.back() < e.track.duration);
            RhythmTrack prepared;
            assert(prepare_rhythm_track(e, e.track.duration, prepared, error));
            assert(!prepare_rhythm_track(e, e.track.duration / 2, prepared, error));
        }
    }
    std::ofstream("bad-catalog.json") << "[]";
    assert(!load_rhythm_catalog("bad-catalog.json", catalog, error) && catalog.size() == 10);
    RhythmCalibration calibration;
    for (double bias : {.080, -.060})
    {
        calibration.begin();
        calibration.tap(.5); // count-in, ignored
        for (int i = 0; i < 24; ++i)
        {
            calibration.tap((4 + i) * .5 + bias);
            calibration.tap((4 + i) * .5 + bias); // duplicate ignored
        }
        auto result = calibration.finish();
        assert(result.accepted && std::abs(result.offset - bias) < 1e-8);
    }
    calibration.begin();
    for (int i = 0; i < 24; ++i)
        calibration.tap((4 + i) * .5 + (i % 2 ? .15 : -.15));
    assert(!calibration.finish().accepted);
    calibration.begin();
    for (int i = 0; i < 10; ++i)
        calibration.tap((4 + i) * .5 + .08);
    assert(!calibration.finish().accepted);
    calibration.begin();
    for (int i = 0; i < 24; ++i)
        calibration.tap((4 + i) * .5 + (i == 5 ? .24 : .08));
    assert(calibration.finish().accepted && std::abs(calibration.finish().offset - .08) < 1e-8);
    double offset = .03;
    assert(save_rhythm_calibration("calibration-check.json", .08, error));
    assert(load_rhythm_calibration("calibration-check.json", offset, error) && offset == .08);
    for (auto invalid : {"[]", "{\"offset_ms\":999}", "{\"offset_ms\":\"NaN\"}",
                         "{\"offset_ms\":80} NaN", "{\"offset_ms\":80} {}"})
    {
        std::ofstream("calibration-check.json") << invalid;
        offset = .03;
        assert(!load_rhythm_calibration("calibration-check.json", offset, error) && offset == .03);
    }
    {
        RhythmTrackLoader loader;
        auto first = loader.request("missing1.mp3");
        auto latest = loader.request("missing2.mp3");
        assert(latest > first);
        RhythmLoadResult result;
        bool ready = false;
        for (int i = 0; i < 100 && !ready; ++i)
        {
            ready = loader.poll(result);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        assert(ready && result.generation == latest && !result.error.empty());
    }
    {
        _putenv_s("GB_RHYTHM_SILENT", "1");
        auto test_session = std::make_shared<RhythmSession>(default_rhythm_track());
        auto test_audio = std::make_shared<gb::al::RhythmAudioTransport>();
        auto test_bots = std::make_shared<FakeLeaderboard>();
        RhythmShipPlayback fallback(
            test_session, test_audio, test_bots, catalog,
            [](const std::string &) { return "missing.mp3"; }, "notice-calibration.json");
        fallback.select(0);
        for (int i = 0; i < 100 && !fallback.scoring_active(); ++i)
        {
            fallback.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        assert(fallback.status().find("Cannot load") != std::string::npos);
        fallback.select(9);
        assert(fallback.status().find("Cannot load") == std::string::npos);
        std::remove("playback-calibration.json");
        auto session = std::make_shared<RhythmSession>(default_rhythm_track());
        auto audio = std::make_shared<gb::al::RhythmAudioTransport>();
        auto bots = std::make_shared<FakeLeaderboard>();
        RhythmShipPlayback playback(
            session, audio, bots, catalog,
            [](const std::string &name) { return "../../../gbResources/" + name; },
            "playback-calibration.json");
        playback.select(0);
        playback.tap(.017);
        assert(!playback.scoring_active() && session->score().judgements == 0);
        for (int i = 0; i < 100 && !playback.scoring_active(); ++i)
        {
            playback.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        assert(playback.scoring_active() && session->track().id == "active_sport");
        assert(playback.set_calibration_offset(.08));
        playback.tap(.091);
        session->update(.091, .08);
        assert(session->score().score == 100);
        const auto epoch = playback.epoch();
        auto invalid_settings = playback.choreography_settings();
        invalid_settings.enabled_actions = 0;
        assert(!playback.apply_choreography(invalid_settings) && playback.epoch() == epoch &&
               session->score().score == 100);
        auto hard = playback.choreography_settings();
        hard.difficulty = RhythmDifficulty::Hard;
        assert(playback.apply_choreography(hard) && playback.epoch() > epoch &&
               session->score().score == 0);
        playback.restart(session->track());
        assert(session->score().score == 0);
        playback.select(1);
        playback.select(2);
        playback.select(0); // Active cache wins over stale decode.
        for (int i = 0; i < 100 && !playback.scoring_active(); ++i)
        {
            playback.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        assert(session->track().id == "active_sport");
        playback.begin_calibration();
        assert(!playback.scoring_active());
        for (int i = 0; i < 24; ++i)
            playback.tap((4 + i) * .5 - .06);
        assert(playback.scoring_active() && playback.calibration_offset() < 0 &&
               session->score().score == 0);
        playback.select(2);
        playback.begin_calibration();
        playback.cancel_calibration();
        for (int i = 0; i < 100 && !playback.scoring_active(); ++i)
        {
            playback.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        assert(session->track().id == "gym_motivation");
    }
    {
        auto authored = catalog;
        authored[0].track.generated_choreography = false;
        authored[0].track.events = {RhythmEvent(.5, RhythmAction::SwipeLeft)};
        auto session = std::make_shared<RhythmSession>(default_rhythm_track());
        auto audio = std::make_shared<gb::al::RhythmAudioTransport>();
        auto bots = std::make_shared<FakeLeaderboard>();
        RhythmShipPlayback playback(
            session, audio, bots, authored,
            [](const std::string &name) { return "../../../gbResources/" + name; },
            "playback-calibration.json");
        playback.select(0);
        assert(playback.apply_choreography(RhythmChoreographySettings(), true));
        for (int i = 0; i < 100 && !playback.scoring_active(); ++i)
        {
            playback.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        assert(session->track().generated_choreography && session->track().events.size() > 1);
    }
    std::cout << "Music catalog, calibration and loader checks passed\n";
}
