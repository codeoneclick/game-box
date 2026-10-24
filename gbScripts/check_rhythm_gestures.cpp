#include "rhythm_choreography.h"
#include "rhythm_crowd_choreography.h"
#include "rhythm_dance_pose.h"
#include "rhythm_gesture.h"
#include "rhythm_prompt_model.h"
#include "rhythm_session.h"
#include "rhythm_stage_state.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
using namespace game;
int main()
{
    {
        RhythmTrack acoustic;
        acoustic.duration = 12;
        std::string error;
        std::ofstream("accent-map.json")
            << R"({"duration":12,"bpm":120,"choreography":"accents","accents":[{"time":1.83,"action":"Tap","strength":0.8},{"time":2.26,"action":"Tap","strength":0.85},{"time":2.69,"action":"Tap","strength":0.9},{"time":4.81,"action":"SwipeDown","strength":1},{"time":5.93,"action":"SwipeRight","strength":0.9}]})";
        assert(load_rhythm_track("accent-map.json", acoustic, error));
        RhythmTrack chart;
        assert(generate_rhythm_choreography(acoustic, RhythmChoreographySettings(), chart, error));
        assert(chart.events.size() == 5);
        assert(chart.events.front().time == 1.83 && chart.events[3].time == 4.81);
        assert(chart.events[0].action == RhythmAction::Tap &&
               chart.events[3].action == RhythmAction::SwipeDown);
        auto settings = RhythmChoreographySettings();
        settings.seed = 999;
        RhythmTrack again;
        assert(generate_rhythm_choreography(acoustic, settings, again, error));
        assert(again.events[0].time == chart.events[0].time &&
               again.events[3].action == chart.events[3].action);
        std::ofstream("accent-map.json")
            << R"({"duration":12,"choreography":"accents","accents":[{"time":1,"action":"Tap","strength":2}]})";
        assert(!load_rhythm_track("accent-map.json", again, error));
    }
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
    // Keep preparation before the first musical phrase.
    assert(normal.events.front().time >= 2);
    // A musical phrase can have a short Tap series followed by a real rest.
    assert(normal.events[0].action == RhythmAction::Tap &&
           normal.events[1].action == RhythmAction::Tap &&
           normal.events[2].action == RhythmAction::Tap);
    assert(std::abs(normal.events[1].time - normal.events[0].time - .5) < 1e-9);
    assert(std::abs(normal.events[2].time - normal.events[1].time - .5) < 1e-9);
    assert(normal.events[3].action == RhythmAction::SwipeUp &&
           normal.events[3].time - normal.events[2].time >= 2);
    {
        RhythmTrack fast;
        fast.duration = 16;
        fast.bpm = 160;
        RhythmTrack relaxed;
        RhythmChoreographySettings defaults;
        assert(generate_rhythm_choreography(fast, defaults, relaxed, error));
        RhythmSession s(relaxed);
        const auto first = relaxed.events[2];
        const double second = relaxed.events[3].time;
        const double midpoint = (first.time + second) * .5;
        // Expiry tolerance must not shift command changes off the musical midpoint.
        assert(s.upcoming(first.time + .31, 1)[0].id == 2);
        assert(s.upcoming(midpoint, 1)[0].id == 2);
        assert(s.upcoming(midpoint + .0001, 1)[0].id == 3);
        const auto cue = rhythm_prompt_state(s, midpoint + .0001, 2, 2);
        assert(cue.progress < .001);
        const auto hit = rhythm_prompt_state(s, second, 2, 2);
        assert(std::abs(hit.progress - 1) < 1e-9);
        assert(hit.beat_pulse == 1);
        assert(rhythm_prompt_state(s, second - .01, 2, 2).beat_pulse == 0);
        assert(rhythm_prompt_state(s, second + .15, 2, 2).beat_pulse == 0);
        assert(second - first.time >= 1.8);
        assert(s.upcoming(first.time + .29, 1)[0].id == 2);
        s.enqueue_input(RhythmInput(first.time + .29, first.action));
        s.update(first.time + .29);
        assert(s.last_judgement() == RhythmJudgement::good);
        assert(RhythmPresentationSettings().prompt_lead >= 2);
        assert(s.set_windows(RhythmWindows(.1, .2, .4)));
        assert(!s.set_windows(RhythmWindows(.1, .2, .501)));
    }
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
        assert(end.x == 0 && end.y == 0 && end.z == 0 && end.scale_y == 1 && end.lean == 0);
        if (i == 1)
            assert(pose.x < 0);
        if (i == 2)
            assert(pose.x > 0);
        if (i == 3 || i == 4)
            assert(pose.scale_y == 1 && pose.y < .2f);
        if (i == 3)
            assert(pose.z < 0 && pose.x == 0);
        if (i == 4)
            assert(pose.z > 0 && pose.x == 0);
        const auto returned = rhythm_dance_pose(static_cast<RhythmAction>(i), .55, 1);
        assert(returned.x == 0 && returned.y == 0 && returned.z == 0);
    }
    // The dance accent follows the input promptly instead of peaking 275ms later.
    assert(rhythm_dance_pose(RhythmAction::Tap, .06, 1).y >
           rhythm_dance_pose(RhythmAction::Tap, .20, 1).y);
    assert(rhythm_dance_pose(RhythmAction::Tap, .25, 1).y == 0);
    assert(rhythm_dance_pose(RhythmAction::SwipeRight, .10, 1).x > 1.4f);
    RhythmSession choreography(normal);
    {
        RhythmTrack delayed;
        delayed.duration = 4;
        delayed.beats = {1, 2};
        delayed.events = {RhythmEvent(1, RhythmAction::SwipeLeft),
                          RhythmEvent(2, RhythmAction::SwipeRight)};
        RhythmSession s(delayed);
        RhythmCrowdChoreography crowd;
        crowd.restart(42);
        crowd.update(.999, s, 50);
        assert(crowd.outcomes().empty());
        crowd.update(1.34, s, 50);
        assert(crowd.outcomes().size() == 49);
        std::array<double, 49> delays{};
        for (const auto &o : crowd.outcomes())
        {
            delays[o.bot] = o.state.start - 1;
            assert(delays[o.bot] >= 0 && delays[o.bot] <= .33);
        }
        crowd.update(2.34, s, 50);
        assert(crowd.outcomes().size() == 49);
        for (const auto &o : crowd.outcomes())
            assert(std::abs(o.state.start - 2 - delays[o.bot]) < 1e-9);
        auto bounds = std::minmax_element(delays.begin(), delays.end());
        assert(*bounds.second - *bounds.first > .2);
    }
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
    const auto first_prompt = normal.events.front().time;
    auto p1 = rhythm_prompt_state(choreography, first_prompt - .5, 2, 2);
    auto p2 = rhythm_prompt_state(choreography, first_prompt - .45, 2, 8);
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
