#include "rhythm_choreography.h"
#include "rhythm_crowd_choreography.h"
#include "rhythm_dance_pose.h"
#include "rhythm_gesture.h"
#include "rhythm_prompt_model.h"
#include "rhythm_session.h"
#include "rhythm_stage_state.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
using namespace game;
int main()
{
    {
        RhythmTrack parsed;
        std::string error;
        std::ofstream("typed-map.json")
            << R"({"duration":8,"bpm":120,"offset":0.1,"choreography":"phrases","events":[{"time":1,"action":"Tap"},{"time":1.5,"action":"SwipeLeft"},{"time":2,"action":"SwipeRight"},{"time":2.25,"action":"SwipeUp"},{"time":2.5,"action":"SwipeDown"}]})";
        assert(load_rhythm_track("typed-map.json", parsed, error));
        assert(!parsed.generated_choreography && parsed.events.size() == 5);
        assert(std::abs(resolve_rhythm_events(parsed)[0].time - 1.1) < 1e-9);
        parsed.id = "preserve";
        for (const auto *invalid :
             {R"({"events":[{"time":1,"action":"Hold"}]})",
              R"({"events":[{"time":1,"action":"tap"}]})",
              R"({"events":[{"time":1,"action":"Tap"},{"time":1.01,"action":"SwipeUp"}]})",
              R"({"duration":8,"events":[{"time":8,"action":"Tap"}]})", R"({"events":[]})"})
        {
            std::ofstream("typed-map.json") << invalid;
            assert(!load_rhythm_track("typed-map.json", parsed, error) && parsed.id == "preserve");
        }
    }
    {
        RhythmTrack t;
        t.duration = 4;
        t.beats = {.5, 1};
        t.events = {RhythmEvent(1, RhythmAction::Tap), RhythmEvent(1.18, RhythmAction::SwipeRight)};
        RhythmSession s(t);
        assert(s.upcoming(1.09, 1)[0].event.action == RhythmAction::Tap);
        assert(s.upcoming(1.10, 1)[0].event.action == RhythmAction::SwipeRight);
    }
    {
        RhythmGesture g;
        RhythmInput input;
        g.begin(100, 100, 0, 1);
        assert(!g.move(120, 100, .1, 1, input));
        assert(g.move(125, 100, .2, 1, input) && input.action == RhythmAction::SwipeRight &&
               input.time == .2);
        assert(!g.end(140, 100, .3, 1, input));
        g.begin(0, 0, 0, 1);
        assert(!g.move(30, 30, .1, 1, input));
        assert(!g.end(30, 30, .2, 1, input));
        g.begin(0, 0, 0, 1);
        assert(g.move(0, -30, .1, 1, input) && input.action == RhythmAction::SwipeUp);
        g.begin(0, 0, 0, 1);
        assert(!g.end(0, 0, .2, 2, input));
        g.begin(0, 0, 0, 1);
        assert(g.end(3, 2, .2, 1, input) && input.action == RhythmAction::Tap);
        g.begin(0, 0, 0, 1);
        g.cancel();
        assert(!g.end(0, 0, .1, 1, input));
    }
    RhythmTrack source;
    source.duration = 32;
    source.bpm = 120;
    RhythmChoreographySettings settings;
    RhythmTrack easy, normal, hard, again;
    std::string error;
    settings.difficulty = RhythmDifficulty::Easy;
    assert(generate_rhythm_choreography(source, settings, easy, error));
    settings.difficulty = RhythmDifficulty::Normal;
    assert(generate_rhythm_choreography(source, settings, normal, error));
    settings.difficulty = RhythmDifficulty::Hard;
    assert(generate_rhythm_choreography(source, settings, hard, error));
    assert(generate_rhythm_choreography(source, settings, again, error));
    assert(easy.events.size() < normal.events.size() && normal.events.size() < hard.events.size());
    assert(hard.events.size() == again.events.size());
    for (size_t i = 0; i < hard.events.size(); ++i)
        assert(hard.events[i].time == again.events[i].time &&
               hard.events[i].action == again.events[i].action);
    for (auto e : easy.events)
        assert(e.action != RhythmAction::SwipeUp && e.action != RhythmAction::SwipeDown);
    assert(resolve_rhythm_beats(source) == resolve_rhythm_beats(hard));
    unsigned found = 0;
    for (auto e : normal.events)
        found |= 1u << static_cast<unsigned>(e.action);
    assert(found == 31);
    settings.enabled_actions = 0;
    again.id = "preserve";
    assert(!generate_rhythm_choreography(source, settings, again, error) && again.id == "preserve");
    for (unsigned i = 0; i < 5; ++i)
    {
        auto pose = rhythm_dance_pose(static_cast<RhythmAction>(i), .15, 1);
        auto end = rhythm_dance_pose(static_cast<RhythmAction>(i), 1, 1);
        assert(end.x == 0 && end.y == 0 && end.scale_y == 1 && end.lean == 0);
        if (i == 1)
            assert(pose.x < 0);
        if (i == 2)
            assert(pose.x > 0);
        if (i == 3)
            assert(pose.y > 0);
        if (i == 4)
            assert(pose.scale_y < 1);
    }
    RhythmSession choreography(normal);
    RhythmCrowdChoreography a, b;
    a.restart(42);
    b.restart(42);
    a.update(8, choreography, 40);
    unsigned points_a = 0;
    for (const auto &o : a.outcomes())
        points_a += o.points();
    unsigned points_b = 0;
    for (int i = 0; i <= 80; ++i)
    {
        b.update(i * .1, choreography, 40);
        for (const auto &o : b.outcomes())
            points_b += o.points();
    }
    assert(points_a == points_b && a.sync() >= 0 && a.sync() <= 1 && a.sync() == b.sync());
    a.update(8, choreography, 40);
    assert(a.outcomes().empty());
    auto p1 = rhythm_prompt_state(choreography, 1, 1.25, 2);
    auto p2 = rhythm_prompt_state(choreography, 1.05, 1.25, 8);
    assert(p1.visible && p2.progress >= p1.progress && p2.preview.size() <= 3);
    auto hidden = rhythm_prompt_state(choreography, -1, 1.25, 2);
    assert(!hidden.visible);
    auto left = rhythm_stage_state(RhythmAction::SwipeLeft, .1, .1, .5, .8, 1, true);
    auto right = rhythm_stage_state(RhythmAction::SwipeRight, .1, .1, .5, .8, 1, true);
    assert(left.sweep < 0 && right.sweep > 0);
    auto off = rhythm_stage_state(RhythmAction::SwipeRight, .1, .1, .5, .8, 1, false);
    assert(off.sweep == 0);
    assert(rhythm_stage_state(RhythmAction::Tap, .1, 10, .5, .8, 1, true).speaker > 0);
    assert(rhythm_stage_state(RhythmAction::Tap, 1, 10, .5, .8, 1, true).speaker == 0);
    RhythmStagePulses pulses;
    for (int i = 0; i < 10; ++i)
        pulses.trigger(i * .5);
    assert(!pulses.sample(0, 20).active);
    std::cout << "Directional rhythm checks passed\n";
}
