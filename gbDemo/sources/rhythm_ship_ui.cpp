#include "rhythm_ship_ui.h"
#include "button.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_2d_component.h"
#include "game_object_2d.h"
#include "label.h"
#include "panel.h"
#include "render_statistics.h"
#include "rhythm_prompt_view.h"
#include "scene_fabricator.h"
#include "slider.h"
#include "ui_fabricator.h"
#include <chrono>
#include <iomanip>
#include <sstream>
namespace game
{
struct RhythmShipUI::implementation : std::enable_shared_from_this<implementation>
{
    gb::scene_fabricator_shared_ptr fab;
    std::shared_ptr<RhythmSession> session;
    std::shared_ptr<FakeLeaderboard> bots;
    std::shared_ptr<gb::al::RhythmAudioTransport> audio;
    std::shared_ptr<RhythmShipDebugSettings> settings;
    std::shared_ptr<RhythmShipPlayback> playback;
    std::function<void(const RhythmTrack &)> restart;
    gb::ces_entity_shared_ptr root = gb::ces_entity::construct<gb::game_object_2d>();
    gb::ui::panel_shared_ptr debug, ranks;
    std::array<gb::ces_entity_shared_ptr, 4> pages;
    std::shared_ptr<RhythmPromptView> prompt;
    gb::label_shared_ptr score, summary, rank, title, stats, diagnostic, debug_info, calibration,
        rank_rows;
    gb::ui::button_shared_ptr difficulty;
    std::array<gb::ui::button_shared_ptr, 5> actions;
    std::array<gb::ui::button_shared_ptr, 3> visual_toggles;
    struct Binding
    {
        gb::label_shared_ptr text;
        gb::ui::slider_shared_ptr slider;
        std::string name;
        double low, high;
        int precision;
        std::function<double(implementation &)> get;
    };
    std::vector<Binding> bindings;
    glm::ivec2 size;
    glm::vec2 anchor = glm::vec2(-1000);
    bool visible = false, blocked = false, ranks_visible = false, force = false;
    int page = 0;
    double sync = 0;
    std::uint64_t visibility_epoch = 0;
    std::shared_ptr<std::uint64_t> arm_capture(const gb::ces_entity_shared_ptr &control)
    {
        auto ticket = std::make_shared<std::uint64_t>(~std::uint64_t(0));
        std::weak_ptr<implementation> weak = shared_from_this();
        control->get_component<gb::ces_bound_touch_component>()->add_callback(
            gb::e_input_state_pressed,
            [weak, ticket](const gb::ces_entity_shared_ptr &e, const glm::vec2 &,
                           gb::e_input_source, gb::e_input_state) {
                if (auto p = weak.lock())
                    if (!p->blocked && e->get_is_visible())
                        *ticket = p->visibility_epoch;
            });
        return ticket;
    }
    std::chrono::steady_clock::time_point refreshed;
    gb::label_shared_ptr label(const gb::ces_entity_shared_ptr &parent, float x, float y,
                               const std::string &text, float font = 14)
    {
        auto l = fab->create_label_2d("button_label.xml");
        l->font_name = "Vera.ttf";
        l->font_size = font;
        l->font_color = glm::u8vec4(243, 248, 255, 255);
        l->text = text;
        l->position = glm::vec2(x, y);
        parent->add_child(l);
        return l;
    }
    gb::ui::button_shared_ptr button(const gb::ces_entity_shared_ptr &parent, float x, float y,
                                     float w, const std::string &text,
                                     std::function<void(implementation &)> action)
    {
        gb::ui::ui_fabricator controls(fab);
        std::weak_ptr<implementation> weak = shared_from_this();
        auto b = controls.create_button(glm::vec2(w, 30), nullptr);
        auto ticket = arm_capture(b);
        b->set_on_pressed_callback([weak, action, ticket](const gb::ces_entity_shared_ptr &e) {
            if (auto p = weak.lock())
                if (!p->blocked && e->get_is_visible() && *ticket == p->visibility_epoch)
                    action(*p);
        });
        b->position = glm::vec2(x, y);
        b->enumerate_children([](const gb::ces_entity_shared_ptr &e) {
            if (auto l = std::dynamic_pointer_cast<gb::label>(e))
                l->font_name = "Vera.ttf";
        });
        b->set_text(text);
        b->set_font_size(12);
        b->set_text_color(glm::u8vec4(245, 248, 255, 255));
        b->set_foreground_color(glm::u8vec4(25, 55, 78, 255));
        parent->add_child(b);
        return b;
    }
    void slider(const gb::ces_entity_shared_ptr &parent, float y, const std::string &name,
                double low, double high, std::function<double(implementation &)> getter,
                std::function<void(implementation &, double)> setter, int precision = 0)
    {
        gb::ui::ui_fabricator controls(fab);
        std::weak_ptr<implementation> weak = shared_from_this();
        Binding binding;
        binding.text = label(parent, 16, y, name, 13);
        binding.slider = controls.create_slider(glm::vec2(290, 18));
        binding.slider->position = glm::vec2(325, y + 3);
        binding.name = name;
        binding.low = low;
        binding.high = high;
        binding.precision = precision;
        binding.get = getter;
        binding.slider->set_offset(static_cast<float>((getter(*this) - low) / (high - low)));
        auto ticket = arm_capture(binding.slider);
        std::weak_ptr<gb::ui::slider> control = binding.slider;
        binding.slider->set_callback([weak, control, ticket, setter, low, high](float offset) {
            auto c = control.lock();
            if (auto p = weak.lock())
                if (c && c->get_is_visible() && !p->blocked && *ticket == p->visibility_epoch)
                    setter(*p, low + glm::clamp(offset, 0.f, 1.f) * (high - low));
        });
        parent->add_child(binding.slider);
        bindings.push_back(binding);
    }
    void visibility()
    {
        ++visibility_epoch;
        debug->visible = visible && !blocked;
        debug->visible_in_next_frame = visible && !blocked;
        for (int i = 0; i < 4; ++i)
        {
            pages[i]->visible = visible && !blocked && i == page;
            pages[i]->visible_in_next_frame = visible && !blocked && i == page;
        }
        ranks->visible = ranks_visible && !blocked;
        ranks->visible_in_next_frame = ranks_visible && !blocked;
    }
    void build()
    {
        gb::ui::ui_fabricator controls(fab);
        auto top = controls.create_panel(glm::vec2(size.x - 32.f, 82), "");
        top->position = glm::vec2(16, 14);
        top->set_background_color(glm::u8vec4(7, 23, 42, 185));
        top->set_header_color(glm::u8vec4(7, 23, 42, 185));
        root->add_child(top);
        score = label(top, 12, 10, "SCORE 0  /  COMBO x0", 18);
        summary = label(top, 12, 44, "", 12);
        rank = label(top, 365, 12, "", 13);
        button(top, size.x - 145.f, 43, 96, "Ranks", [](implementation &p) {
            p.ranks_visible = !p.ranks_visible;
            p.visibility();
        });
        ranks = controls.create_panel(glm::vec2(250, 230), "");
        ranks->position = glm::vec2(size.x - 266.f, 108);
        ranks->set_background_color(glm::u8vec4(7, 23, 42, 230));
        root->add_child(ranks);
        rank_rows = label(ranks, 12, 12, "", 14);
        auto bottom = controls.create_panel(glm::vec2(size.x - 32.f, 133), "");
        bottom->position = glm::vec2(16, size.y - 147.f);
        bottom->set_background_color(glm::u8vec4(7, 23, 42, 205));
        bottom->set_header_color(glm::u8vec4(7, 23, 42, 205));
        root->add_child(bottom);
        title = label(bottom, 10, 8, "Loading", 12);
        button(bottom, 10, 32, 64, "Prev", [](implementation &p) { p.playback->next(-1); });
        button(bottom, 82, 32, 64, "Next", [](implementation &p) { p.playback->next(1); });
        button(bottom, 154, 32, 84, "Restart",
               [](implementation &p) { p.restart(p.session->track()); });
        button(bottom, 246, 32, 110, "F1 Settings", [](implementation &p) {
            p.visible = !p.visible;
            p.visibility();
        });
        stats = label(bottom, 10, 74, "", 12);
        diagnostic = label(bottom, 10, 97, "", 10);
        label(root, 18, size.y - 17.f, "SPACE: TAP  /  ARROWS OR MOUSE DRAG: MOVE  /  F8: GRAPHICS",
              10);
        debug = controls.create_panel(glm::vec2(size.x - 32.f, 650), "");
        debug->position = glm::vec2(16, 110);
        debug->set_background_color(glm::u8vec4(7, 20, 38, 248));
        root->add_child(debug);
        label(debug, 14, 8, "RHYTHM DEVELOPMENT / F1", 14);
        const char *tabs[] = {"Gameplay", "Timing", "Visuals", "Calibration"};
        for (int i = 0; i < 4; ++i)
        {
            button(debug, 14 + i * 157.f, 38, 147, tabs[i], [i](implementation &p) {
                p.page = i;
                p.visibility();
            });
            auto group = gb::ces_entity::construct<gb::game_object_2d>();
            group->position = glm::vec2(0, 90);
            debug->add_child(group);
            pages[i] = group;
        }
        auto game = pages[0];
        difficulty = button(game, 16, 6, 195, "Difficulty: Normal", [](implementation &p) {
            auto s = p.playback->choreography_settings();
            s.difficulty =
                static_cast<RhythmDifficulty>((static_cast<unsigned>(s.difficulty) + 1) % 3);
            p.playback->apply_choreography(s);
        });
        button(game, 230, 6, 180, "Generate Beatmap", [](implementation &p) {
            p.playback->apply_choreography(p.playback->choreography_settings(), true);
        });
        const char *names[] = {"TAP", "LEFT", "RIGHT", "UP", "DOWN"};
        for (unsigned i = 0; i < 5; ++i)
        {
            actions[i] = button(game, 16 + i * 124.f, 55, 115, names[i], [i](implementation &p) {
                auto s = p.playback->choreography_settings();
                s.enabled_actions ^= 1u << i;
                p.playback->apply_choreography(s);
            });
            button(game, 16 + i * 124.f, 97, 115, std::string("Test ") + names[i],
                   [i](implementation &p) {
                       p.playback->input(
                           RhythmInput(p.audio->seconds(), static_cast<RhythmAction>(i)));
                   });
        }
        label(game, 16, 141,
              "Test buttons judge at current audio time. Easy uses TAP / LEFT / RIGHT.", 11);
        button(game, 16, 171, 145, "Force Beat", [](implementation &p) { p.force = true; });
        button(game, 174, 171, 145, "Energy 100%",
               [](implementation &p) { p.session->set_energy(1); });
        button(game, 332, 171, 180, "Become Rank #1",
               [](implementation &p) { p.session->debug_lead(p.bots->highest_bot_score() + 100); });
        slider(
            game, 225, "Swipe threshold px", 8, 96,
            [](implementation &p) { return p.settings->presentation.swipe_threshold; },
            [](implementation &p, double v) {
                p.settings->presentation.swipe_threshold = static_cast<float>(v);
            });
        slider(
            game, 272, "Crowd", 30, 50, [](implementation &p) { return p.settings->crowd_count; },
            [](implementation &p, double v) {
                p.settings->crowd_count = static_cast<int>(std::round(v));
            });
        slider(
            game, 319, "Energy %", 0, 100,
            [](implementation &p) { return p.session->score().energy * 100; },
            [](implementation &p, double v) { p.session->set_energy(v / 100); });
        auto timing = pages[1];
        for (int i = 0; i < 3; ++i)
        {
            const char *names[] = {"Perfect ms", "Great ms", "Good ms"};
            slider(
                timing, 20 + i * 52.f, names[i], 5, 250,
                [i](implementation &p) {
                    auto w = p.session->windows();
                    return (i == 0 ? w.perfect : i == 1 ? w.great : w.good) * 1000;
                },
                [i](implementation &p, double v) {
                    auto w = p.session->windows();
                    if (i == 0)
                        w.perfect = v / 1000;
                    if (i == 1)
                        w.great = v / 1000;
                    if (i == 2)
                        w.good = v / 1000;
                    p.session->set_windows(w);
                });
        }
        slider(
            timing, 192, "Authored offset seconds", -1, 1,
            [](implementation &p) { return p.session->track().offset; },
            [](implementation &p, double v) {
                auto t = p.session->track();
                t.offset = v;
                try
                {
                    resolve_rhythm_events(t);
                    p.restart(t);
                }
                catch (const std::exception &)
                {
                }
            },
            3);
        slider(
            timing, 246, "BPM (generated pulse only)", 40, 230,
            [](implementation &p) { return p.session->track().bpm; },
            [](implementation &p, double v) {
                auto t = p.session->track();
                if (!t.beats.empty())
                    return;
                t.bpm = std::round(v);
                try
                {
                    resolve_rhythm_events(t);
                    p.restart(t);
                }
                catch (const std::exception &)
                {
                }
            });
        label(timing, 16, 305, "Wrong direction resolves only the nearest event. Ties go earlier.",
              11);
        auto visuals = pages[2];
        const char *toggle_names[] = {"Beat indicator", "Crowd choreography", "Directional stage"};
        for (int i = 0; i < 3; ++i)
            visual_toggles[i] =
                button(visuals, 16 + i * 207.f, 12, 196, toggle_names[i], [i](implementation &p) {
                    auto &s = p.settings->presentation;
                    if (i == 0)
                        s.beat_visualization = !s.beat_visualization;
                    if (i == 1)
                        s.crowd_choreography = !s.crowd_choreography;
                    if (i == 2)
                        s.stage_directional = !s.stage_directional;
                });
        slider(
            visuals, 76, "Music volume", 0, 1, [](implementation &p) { return p.settings->volume; },
            [](implementation &p, double v) {
                p.settings->volume = static_cast<float>(v);
                p.audio->set_volume(p.settings->volume);
            },
            2);
        slider(
            visuals, 128, "Stage intensity", 0, 3,
            [](implementation &p) { return p.settings->stage_intensity; },
            [](implementation &p, double v) {
                p.settings->stage_intensity = static_cast<float>(v);
            },
            2);
        slider(
            visuals, 180, "Prompt lead seconds", .25, 3,
            [](implementation &p) { return p.settings->presentation.prompt_lead; },
            [](implementation &p, double v) { p.settings->presentation.prompt_lead = v; }, 2);
        slider(
            visuals, 232, "Preview actions", 0, 3,
            [](implementation &p) { return p.settings->presentation.preview_count; },
            [](implementation &p, double v) {
                p.settings->presentation.preview_count = static_cast<int>(std::round(v));
            });
        auto cal = pages[3];
        label(cal, 16, 10, "Listen to four count-in clicks, then TAP 24 times with SPACE or click.",
              12);
        button(cal, 16, 53, 160, "Calibrate",
               [](implementation &p) { p.playback->begin_calibration(); });
        button(cal, 192, 53, 140, "Cancel",
               [](implementation &p) { p.playback->cancel_calibration(); });
        button(cal, 348, 53, 150, "Reset offset", [](implementation &p) {
            if (!p.playback->calibrating())
                p.playback->set_calibration_offset(0);
        });
        calibration = label(cal, 16, 105, "", 13);
        slider(
            cal, 157, "Personal offset ms", -250, 250,
            [](implementation &p) { return p.playback->calibration_offset() * 1000; },
            [](implementation &p, double v) {
                if (!p.playback->calibrating())
                    p.playback->set_calibration_offset(v / 1000);
            });
        label(cal, 16, 216,
              "Positive = late taps. Personal offset never shifts the music or visuals.", 11);
        debug_info = label(debug, 14, 590, "", 11);
        prompt = std::make_shared<RhythmPromptView>(fab);
        root->add_child(prompt->root());
        float depth = -.85f;
        std::function<void(const gb::ces_entity_shared_ptr &)> prepare =
            [&](const gb::ces_entity_shared_ptr &e) {
                if (auto tr = e->get_component<gb::ces_transformation_2d_component>())
                {
                    tr->set_is_in_camera_space(false);
                    tr->set_z_order(depth);
                    depth += .00035f;
                }
                e->enumerate_children(prepare);
            };
        prepare(root);
        visibility();
    }
};
RhythmShipUI::RhythmShipUI(const gb::scene_fabricator_shared_ptr &f,
                           std::shared_ptr<RhythmSession> s, std::shared_ptr<FakeLeaderboard> b,
                           std::shared_ptr<gb::al::RhythmAudioTransport> a,
                           std::shared_ptr<RhythmShipDebugSettings> settings, glm::ivec2 size,
                           std::function<void(const RhythmTrack &)> restart,
                           std::shared_ptr<RhythmShipPlayback> playback)
    : impl_(std::make_shared<implementation>())
{
    auto &p = *impl_;
    p.fab = f;
    p.session = s;
    p.bots = b;
    p.audio = a;
    p.settings = settings;
    p.size = size;
    p.restart = restart;
    p.playback = playback;
    p.build();
}
gb::ces_entity_shared_ptr RhythmShipUI::root() const
{
    return impl_->root;
}
void RhythmShipUI::toggle_debug()
{
    auto &p = *impl_;
    if (p.blocked)
        return;
    p.visible = !p.visible;
    p.visibility();
}
void RhythmShipUI::set_overlay_blocked(bool blocked)
{
    auto &p = *impl_;
    if (p.blocked == blocked)
        return;
    p.blocked = blocked;
    p.root->visible = !blocked;
    p.root->visible_in_next_frame = !blocked;
    p.visibility();
}
void RhythmShipUI::set_prompt_position(glm::vec2 position)
{
    impl_->anchor = position;
}
void RhythmShipUI::set_crowd_sync(double value)
{
    impl_->sync = value;
}
bool RhythmShipUI::contains(glm::ivec2 q) const
{
    auto &p = *impl_;
    return (q.y <= 100) || q.y >= p.size.y - 147 ||
           (p.visible && q.x >= 16 && q.x <= p.size.x - 16 && q.y >= 110 && q.y <= 760) ||
           (p.ranks_visible && q.x >= p.size.x - 266 && q.y >= 108 && q.y <= 338);
}
bool RhythmShipUI::consume_force_beat()
{
    bool v = impl_->force;
    impl_->force = false;
    return v;
}
void RhythmShipUI::update()
{
    auto &p = *impl_;
    const double t = p.audio->seconds();
    auto state = rhythm_prompt_state(*p.session, t, p.settings->presentation.prompt_lead,
                                     p.settings->presentation.preview_count);
    p.prompt->update(state, p.anchor, p.size,
                     !p.blocked && !p.visible && p.playback->scoring_active() &&
                         p.settings->presentation.beat_visualization);
    const auto now = std::chrono::steady_clock::now();
    if (now - p.refreshed < std::chrono::milliseconds(100))
        return;
    p.refreshed = now;
    auto set = [](const gb::label_shared_ptr &l, const std::string &text) {
        if (static_cast<std::string>(l->text) != text)
            l->text = text;
    };
    const auto &s = p.session->score();
    set(p.score, "SCORE " + std::to_string(s.score) + "  /  COMBO x" + std::to_string(s.combo));
    set(p.summary, "ACCURACY " + std::to_string(int(s.accuracy())) + "%   ENERGY " +
                       std::to_string(int(s.energy * 100)) + "%   SYNC " +
                       std::to_string(int(p.sync * 100)) + "%");
    auto ranks = p.bots->ranking(s.score);
    size_t rank = 1;
    std::string rows = "TOP DECK\n\n";
    for (size_t i = 0; i < ranks.size(); ++i)
    {
        if (ranks[i].local)
            rank = i + 1;
        if (i < 6)
            rows += std::to_string(i + 1) + ". " + ranks[i].name + " " +
                    std::to_string(ranks[i].score) + "\n";
    }
    set(p.rank, "RANK #" + std::to_string(rank));
    set(p.rank_rows, rows);
    set(p.title, p.playback->status().substr(0, 88));
    set(p.diagnostic, p.audio->diagnostic() + " | SPACE / ARROWS / SWIPE");
    const auto stat = gb::render_statistics::last_frame();
    std::ostringstream statistics;
    statistics << "FPS: " << std::fixed << std::setprecision(1) << stat.fps
               << "   Polygons/frame: " << stat.triangles << "   Draws: " << stat.draw_calls;
    set(p.stats, statistics.str());
    if (std::getenv("GB_HUD_DEBUG"))
        std::cout << "HUD fps=" << stat.fps << " triangles=" << stat.triangles
                  << " draws=" << stat.draw_calls << std::endl;
    for (auto &binding : p.bindings)
    {
        const double value = binding.get(p);
        std::ostringstream text;
        text << binding.name << " " << std::fixed << std::setprecision(binding.precision) << value;
        set(binding.text, text.str());
        binding.slider->set_offset(
            static_cast<float>((value - binding.low) / (binding.high - binding.low)));
    }
    const auto &choreography = p.playback->choreography_settings();
    const char *difficulties[] = {"Easy", "Normal", "Hard"};
    const std::string difficulty =
        "Difficulty: " + std::string(difficulties[static_cast<unsigned>(choreography.difficulty)]);
    if (p.difficulty->get_text() != difficulty)
        p.difficulty->set_text(difficulty);
    const char *action_names[] = {"TAP", "LEFT", "RIGHT", "UP", "DOWN"};
    for (unsigned i = 0; i < 5; ++i)
    {
        const std::string text = std::string(action_names[i]) +
                                 ((choreography.enabled_actions & (1u << i)) ? " ON" : " OFF");
        if (p.actions[i]->get_text() != text)
            p.actions[i]->set_text(text);
    }
    const bool flags[] = {p.settings->presentation.beat_visualization,
                          p.settings->presentation.crowd_choreography,
                          p.settings->presentation.stage_directional};
    const char *flag_names[] = {"Beat", "Crowd moves", "Stage moves"};
    for (int i = 0; i < 3; ++i)
    {
        const std::string text = std::string(flag_names[i]) + (flags[i] ? " ON" : " OFF");
        if (p.visual_toggles[i]->get_text() != text)
            p.visual_toggles[i]->set_text(text);
    }
    set(p.calibration, p.playback->status().substr(0, 84) + "\nSaved: " +
                           std::to_string(int(p.playback->calibration_offset() * 1000)) + " ms");
    RhythmResolution result;
    std::string details = "";
    if (p.session->last_resolution(result))
        details = "Expected " + std::string(rhythm_action_name(result.event.action)) + " / input " +
                  (result.has_input ? rhythm_action_name(result.input) : "none") + " / " +
                  std::to_string(int(result.error * 1000)) + " ms / " +
                  rhythm_judgement_name(result.judgement);
    set(p.debug_info, details);
}
} // namespace game
