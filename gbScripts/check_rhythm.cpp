#include "fake_leaderboard.h"
#include "rhythm_key_action.h"
#include "rhythm_session.h"
#include "rhythm_track.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
using namespace game;
int main()
{
    auto t = default_rhythm_track();
    assert(std::abs(rhythm_beat_at(t, 0).time - .46875) < 1e-9);
    assert(std::abs(rhythm_beat_at(t, 63).time - 30) < 1e-9);
    assert(std::abs(rhythm_beat_at(t, 64).time - 30.46875) < 1e-9);
    t.duration = 2;
    t.beats = {1};
    for (auto c : {std::make_pair(.040, 100), std::make_pair(.080, 70), std::make_pair(.130, 30),
                   std::make_pair(.131, 0)})
        for (int sign : {-1, 1})
        {
            RhythmSession s(t);
            s.enqueue_tap(1 + sign * c.first);
            s.update(1.2);
            assert(s.score().score == c.second);
        }
    RhythmSession s(t);
    s.enqueue_tap(.99);
    s.update(1.5);
    assert(s.score().score == 100 && s.score().combo == 1);
    s.enqueue_tap(1.01);
    s.update(1.5);
    assert(s.score().score == 100);
    s.update(3.2);
    assert(s.score().combo == 0 && s.score().judgements == 2);
    s.restart();
    assert(s.score().score == 0 && s.score().energy == .35);
    s.enqueue_tap(1);
    s.enqueue_tap(1.01);
    s.update(1.02);
    assert(s.score().judgements == 1);
    assert(s.score().accuracy() == 100);
    s.set_energy(9);
    assert(s.score().energy == 1);
    s.set_energy(-1);
    assert(s.score().energy == 0);
    assert(!s.set_windows({.09, .08, .13}));
    s.restart();
    s.update(10);
    assert(s.beat_events().size() == 5);
    s.update(10);
    assert(s.beat_events().empty());
    s.enqueue_tap(NAN);
    s.update(NAN);
    assert(s.score().score == 0);
    FakeLeaderboard bots;
    bots.on_beat(0);
    auto ranks = bots.ranking(10000);
    assert(ranks.size() == 11 && ranks[0].local);
    auto top = bots.highest_bot_score();
    bots.on_beat(0);
    assert(top == bots.highest_bot_score());
    bots.restart();
    assert(bots.highest_bot_score() == 0 && bots.ranking(0)[0].local);
    std::string error;
    RhythmTrack loaded;
    assert(load_rhythm_track("../../../gbBundle/Configurations/rhythm_ship/rhythm_ship_track.json",
                             loaded, error));
    assert(loaded.bpm == 128);
    assert(rhythm_key_action(49, true) == RhythmKeyAction::tap);
    assert(rhythm_key_action(15, true) == RhythmKeyAction::restart);
    assert(rhythm_key_action(122, true) == RhythmKeyAction::debug);
    assert(rhythm_key_action(32, false) == RhythmKeyAction::tap);
    assert(rhythm_key_action(82, false) == RhythmKeyAction::restart);
    assert(rhythm_key_action(112, false) == RhythmKeyAction::debug);
    t.beats = {0.2, .7, 1.5};
    RhythmSession explicit_session(t);
    assert(std::abs(explicit_session.beat_position(.2)) < 1e-9);
    assert(std::abs(explicit_session.beat_position(.45) - .5) < 1e-9);
    assert(std::abs(explicit_session.beat_position(2.2) - 3) < 1e-9);
    const std::vector<std::string> invalid = {"{\"bpm\":0}",
                                              "{\"bpm\":\"fast\"}",
                                              "{\"duration\":-1}",
                                              "{\"beats\":[1,1]}",
                                              "{\"beats\":[2,1]}",
                                              "{\"beats\":[]}",
                                              "{\"beats\":[1,31]}",
                                              "{\"offset\":null}",
                                              "not json",
                                              "[]",
                                              "42",
                                              "\"x\""};
    for (const auto &json : invalid)
    {
        std::ofstream("invalid-track.json") << json;
        loaded.id = "preserve";
        assert(!load_rhythm_track("invalid-track.json", loaded, error));
        assert(!error.empty() && loaded.id == "preserve");
    }
    t.beats = {0, .5, 1, 1.5};
    RhythmSession zero(t);
    zero.enqueue_tap(0);
    zero.update(0);
    assert(zero.score().score == 100);
    zero.update(.63);
    assert(zero.score().judgements == 1); // inclusive Good deadline
    zero.update(.631);
    assert(zero.score().judgements == 2);
    auto old = zero.score().judgements;
    zero.update(.1);
    assert(zero.score().judgements == old);
    zero.debug_lead(12345);
    assert(zero.score().accuracy() == 50);
    zero.restart();
    assert(zero.score().score == 0 && zero.score().accuracy() == 0);
    std::cout << "Rhythm checks passed\n";
}
