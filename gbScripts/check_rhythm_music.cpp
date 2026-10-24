#include "rhythm_calibration.h"
#include "rhythm_ship_playback.h"
#include "rhythm_track_catalog.h"
#include "rhythm_track_loader.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
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
        RhythmTrack choreography;
        assert(generate_rhythm_choreography(e.track, RhythmChoreographySettings(), choreography,
                                            error));
        const auto acoustic = resolve_rhythm_accents(e.track);
        for (const auto &action : resolve_rhythm_events(choreography))
        {
            if (!acoustic.empty())
            {
                auto accent =
                    std::lower_bound(acoustic.begin(), acoustic.end(), action.time - 1e-8,
                                     [](const RhythmEvent &e, double t) { return e.time < t; });
                assert(accent != acoustic.end() && std::abs(accent->time - action.time) < 1e-7 &&
                       accent->action == action.action);
            }
            else
            {
                auto beat = std::lower_bound(beats.begin(), beats.end(), action.time - 1e-8);
                assert(beat != beats.end() && std::abs(*beat - action.time) < 1e-7);
            }
        }
        std::array<unsigned, 5> counts{};
        std::ofstream chart("playable-" + e.track.id + ".json");
        chart << "[";
        for (size_t i = 0; i < choreography.events.size(); ++i)
        {
            const auto &action = choreography.events[i];
            ++counts[static_cast<unsigned>(action.action)];
            if (i)
                chart << ",";
            chart << "{\"time\":" << std::setprecision(10) << action.time << ",\"action\":\""
                  << rhythm_action_name(action.action) << "\"}";
        }
        chart << "]";
        std::cout << "Playable " << e.track.id << " events=" << choreography.events.size()
                  << " Tap/L/R/U/D=" << counts[0] << "/" << counts[1] << "/" << counts[2] << "/"
                  << counts[3] << "/" << counts[4] << std::endl;
        if (!e.track.audio_file.empty())
        {
            assert(!acoustic.empty() && beats.back() < e.track.duration);
            RhythmTrack prepared;
            assert(prepare_rhythm_track(e, e.track.duration, prepared, error));
            assert(!prepare_rhythm_track(e, e.track.duration / 2, prepared, error));
        }
    }
    std::ofstream("bad-catalog.json") << "[]";
    assert(!load_rhythm_catalog("bad-catalog.json", catalog, error) && catalog.size() == 10);
    RhythmCalibration calibration;
    for (double bias : {.080, -.060, .300, .600, -.600})
    {
        calibration.begin();
        calibration.tap(.5); // count-in, ignored
        for (int i = 0; i < 24; ++i)
        {
            calibration.tap((4 + i) * 1.5 + bias);
            calibration.tap((4 + i) * 1.5 + bias); // duplicate ignored
        }
        auto result = calibration.finish();
        assert(result.accepted && std::abs(result.offset - bias) < 1e-8);
    }
    calibration.begin();
    for (int i = 0; i < 24; ++i)
        calibration.tap((4 + i) * 1.5 + (i % 2 ? .15 : -.15));
    assert(!calibration.finish().accepted);
    calibration.begin();
    for (int i = 0; i < 10; ++i)
        calibration.tap((4 + i) * 1.5 + .08);
    assert(!calibration.finish().accepted);
    calibration.begin();
    for (int i = 0; i < 24; ++i)
        calibration.tap((4 + i) * 1.5 + (i == 5 ? .24 : .08));
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
        const auto first = resolve_rhythm_events(session->track()).front();
        const double calibrated_first = first.time + .08;
        playback.input(RhythmInput(calibrated_first, first.action));
        session->update(playback.raw_to_song_time(calibrated_first));
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
            playback.tap((4 + i) * 1.5 - .06);
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
    {
        auto restricted = std::vector<RhythmTrackEntry>{catalog.back(), catalog.front()};
        restricted[1].track.events.clear();
        restricted[1].track.accents = {RhythmEvent(2, RhythmAction::SwipeDown)};
        restricted[1].track.generated_choreography = true;
        auto session = std::make_shared<RhythmSession>(default_rhythm_track());
        auto audio = std::make_shared<gb::al::RhythmAudioTransport>();
        auto bots = std::make_shared<FakeLeaderboard>();
        RhythmShipPlayback playback(
            session, audio, bots, restricted,
            [](const std::string &name) { return "../../../gbResources/" + name; },
            "restricted-calibration.json");
        playback.select(0);
        auto settings = RhythmChoreographySettings();
        settings.enabled_actions = 1;
        assert(playback.apply_choreography(settings));
        playback.select(1);
        for (int i = 0;
             i < 200 && playback.status().find("No playable audio accents") == std::string::npos;
             ++i)
        {
            playback.update();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        assert(!playback.scoring_active());
        settings.enabled_actions = 1u << static_cast<unsigned>(RhythmAction::SwipeDown);
        assert(playback.apply_choreography(settings));
        assert(playback.scoring_active() && session->track().id == restricted[1].track.id);
    }
    std::cout << "Music catalog, calibration and loader checks passed\n";
}
