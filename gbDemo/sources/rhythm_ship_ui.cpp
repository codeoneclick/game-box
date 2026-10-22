#include "rhythm_ship_ui.h"
#include "button.h"
#include "ces_transformation_2d_component.h"
#include "game_object_2d.h"
#include "label.h"
#include "panel.h"
#include "scene_fabricator.h"
#include "slider.h"
#include "ui_fabricator.h"
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
    std::function<void(const RhythmTrack &)> restart;
    gb::ces_entity_shared_ptr root = gb::ces_entity::construct<gb::game_object_2d>();
    gb::ui::panel_shared_ptr panel;
    std::vector<gb::label_shared_ptr> labels, values;
    gb::label_shared_ptr you;
    gb::label_shared_ptr beat;
    std::vector<gb::ui::slider_shared_ptr> sliders;
    glm::ivec2 size;
    bool visible = false, force = false;
    double refresh = -1;
    gb::label_shared_ptr label(const gb::ces_entity_shared_ptr &parent, std::array<float, 2> p,
                               const std::string &text, float size = 18)
    {
        auto l = fab->create_label_2d("button_label.xml");
        l->font_name = "Vera.ttf";
        l->font_size = size;
        l->font_color = glm::u8vec4(246, 245, 236, 255);
        l->text = text;
        l->position = glm::vec2(p[0], p[1]);
        parent->add_child(l);
        return l;
    }
    void build()
    {
        gb::ui::ui_fabricator controls(fab);
        auto header = controls.create_panel(glm::vec2(290, 220), "RHYTHM SHIP");
        header->position = glm::vec2(16, 14);
        header->set_background_color(glm::u8vec4(10, 20, 42, 235));
        root->add_child(header);
        header->set_header_color(glm::u8vec4(19, 38, 65, 255));
        label(header, {14, 7}, "RHYTHM SHIP", 16);
        labels.push_back(label(header, {14, 38}, "SCORE 0", 23));
        labels.push_back(label(header, {14, 70}, "COMBO x0"));
        labels.push_back(label(header, {14, 98}, "ACCURACY 0%"));
        labels.push_back(label(header, {14, 126}, "ENERGY"));
        labels.push_back(label(header, {14, 153}, "READY", 24));
        beat = label(header, {245, 157}, "O", 24);
        beat->font_color = glm::u8vec4(255, 220, 75, 255);
        labels.push_back(
            label(root, {18, size.y - 35.f}, "SPACE / CLICK: TAP   R: RESTART   F1: DEBUG", 15));
        auto leaders = controls.create_panel(glm::vec2(250, 235), "TOP DECK / LIVE RANK");
        leaders->position = glm::vec2(size.x - 266.f, 14);
        leaders->set_background_color(glm::u8vec4(10, 20, 42, 235));
        root->add_child(leaders);
        leaders->set_header_color(glm::u8vec4(19, 38, 65, 255));
        label(leaders, {12, 7}, "TOP DECK / LIVE RANK", 15);
        for (int i = 0; i < 6; ++i)
            labels.push_back(label(leaders, {12.f, 40.f + i * 28}, "", 16));
        labels.push_back(label(root, {size.x * .5f - 180.f, 18}, "", 19));
        labels.push_back(label(root, {18, size.y - 60.f}, "", 13));
        panel = controls.create_panel(glm::vec2(385, 390), "DEVELOPMENT / F1");
        panel->position = glm::vec2(16, 245);
        panel->set_background_color(glm::u8vec4(7, 17, 36, 248));
        root->add_child(panel);
        panel->set_header_color(glm::u8vec4(19, 38, 65, 255));
        label(panel, {12, 7}, "DEVELOPMENT / F1", 15);
        you = label(root, {0, 0}, "YOU", 16);
        you->font_color = glm::u8vec4(255, 220, 75, 255);
        const char *names[] = {"BPM",     "VOLUME", "OFFSET", "PERFECT ms", "GREAT ms",
                               "GOOD ms", "ENERGY", "CROWD",  "LIGHTS"};
        std::weak_ptr<implementation> weak = shared_from_this();
        for (int i = 0; i < 9; ++i)
        {
            values.push_back(label(panel, {12.f, 38.f + i * 29}, names[i], 14));
            auto s = controls.create_slider(glm::vec2(160, 18));
            s->position = glm::vec2(207, 39 + i * 29);
            panel->add_child(s);
            sliders.push_back(s);
            if (i == 0 && !session->track().beats.empty())
                s->disable(true);
            s->set_offset(i == 0   ? (session->track().bpm - 40) / 190
                          : i == 1 ? settings->volume
                          : i == 2 ? .5f
                          : i == 3 ? .2f
                          : i == 4 ? .4f
                          : i == 5 ? .65f
                          : i == 6 ? .35f
                          : i == 7 ? .5f
                                   : 1.f / 3);
            s->set_callback(
                [weak, i](float v)
                {
                    if (auto p = weak.lock())
                        p->edit(i, v);
                });
        }
        const char *buttons[] = {"Restart Track", "Force Beat", "Energy 100%", "Become Rank #1"};
        for (int i = 0; i < 4; ++i)
        {
            auto b = controls.create_button(glm::vec2(174, 28),
                                            [weak, i](const gb::ces_entity_shared_ptr &)
                                            {
                                                if (auto p = weak.lock())
                                                {
                                                    if (i == 0)
                                                        p->restart(p->session->track());
                                                    if (i == 1)
                                                        p->force = true;
                                                    if (i == 2)
                                                        p->session->set_energy(1);
                                                    if (i == 3)
                                                        p->session->debug_lead(
                                                            p->bots->highest_bot_score() + 100);
                                                }
                                            });
            b->position = glm::vec2(12 + (i % 2) * 187, 306 + (i / 2) * 34);
            b->enumerate_children(
                [](const gb::ces_entity_shared_ptr &node)
                {
                    if (auto text = std::dynamic_pointer_cast<gb::label>(node))
                        text->font_name = "Vera.ttf";
                });
            b->set_text_color(glm::u8vec4(246, 245, 236, 255));
            b->set_foreground_color(glm::u8vec4(26, 48, 74, 255));
            b->set_text(buttons[i]);
            b->set_font_size(13);
            panel->add_child(b);
        }
        float depth = -.8f;
        std::function<void(const gb::ces_entity_shared_ptr &)> prepare =
            [&](const gb::ces_entity_shared_ptr &n)
        {
            if (auto tr = n->get_component<gb::ces_transformation_2d_component>())
            {
                tr->set_is_in_camera_space(false);
                tr->set_z_order(depth);
                depth += .0005f;
            }
            n->enumerate_children(prepare);
        };
        prepare(root);
        panel->visible = false;
        panel->visible_in_next_frame = false;
    }
    void edit(int i, float v)
    {
        v = glm::clamp(v, 0.f, 1.f);
        if (i == 0 || i == 2)
        {
            auto track = session->track();
            if (i == 0)
            {
                if (!track.beats.empty())
                    return;
                track.bpm = 40 + std::round(v * 190);
            }
            else
                track.offset = (v - .5f) * 2;
            try
            {
                resolve_rhythm_beats(track);
                restart(track);
            }
            catch (const std::exception &)
            {
            }
        }
        if (i == 1)
        {
            settings->volume = v;
            audio->set_volume(v);
        }
        if (i >= 3 && i <= 5)
        {
            auto w = session->windows();
            double value = .005 + v * .195;
            if (i == 3)
                w.perfect = value;
            if (i == 4)
                w.great = value;
            if (i == 5)
                w.good = value;
            session->set_windows(w);
        }
        if (i == 6)
            session->set_energy(v);
        if (i == 7)
            settings->crowd_count = 30 + int(std::round(v * 20));
        if (i == 8)
            settings->stage_intensity = v * 3;
    }
};
RhythmShipUI::RhythmShipUI(const gb::scene_fabricator_shared_ptr &f,
                           std::shared_ptr<RhythmSession> s, std::shared_ptr<FakeLeaderboard> b,
                           std::shared_ptr<gb::al::RhythmAudioTransport> a,
                           std::shared_ptr<RhythmShipDebugSettings> d, glm::ivec2 size,
                           std::function<void(const RhythmTrack &)> restart)
    : impl_(std::make_shared<implementation>())
{
    auto &p = *impl_;
    p.fab = f;
    p.session = s;
    p.bots = b;
    p.audio = a;
    p.settings = d;
    p.size = size;
    p.restart = restart;
    p.build();
}
gb::ces_entity_shared_ptr RhythmShipUI::root() const { return impl_->root; }
void RhythmShipUI::toggle_debug()
{
    auto &p = *impl_;
    p.visible = !p.visible;
    p.panel->visible = p.visible;
    p.panel->visible_in_next_frame = p.visible;
}
void RhythmShipUI::set_player_position(glm::vec2 position)
{
    impl_->you->position = position - glm::vec2(16, 0);
}
bool RhythmShipUI::contains(glm::ivec2 q) const
{
    auto &p = *impl_;
    return (q.x >= 16 && q.x <= 306 && q.y >= 14 && q.y <= 234) ||
           (q.x >= p.size.x - 266 && q.y <= 249) ||
           (p.visible && q.x >= 16 && q.x <= 401 && q.y >= 245 && q.y <= 635);
}
bool RhythmShipUI::consume_force_beat()
{
    bool f = impl_->force;
    impl_->force = false;
    return f;
}
void RhythmShipUI::update()
{
    auto &p = *impl_;
    double t = p.audio->seconds();
    if (t >= p.refresh && t - p.refresh < .1)
        return;
    p.refresh = t;
    const auto beat_position = p.session->beat_position(t);
    const float beat_phase = static_cast<float>(beat_position - std::floor(beat_position));
    p.beat->scale = glm::vec2(1 + .3f * std::exp(-beat_phase * 12));
    const auto &s = p.session->score();
    auto set = [](const gb::label_shared_ptr &l, const std::string &x)
    {
        if (static_cast<std::string>(l->text) != x)
            l->text = x;
    };
    set(p.labels[0], "SCORE  " + std::to_string(s.score));
    set(p.labels[1],
        "COMBO x" + std::to_string(s.combo) + "   BEST " + std::to_string(s.max_combo));
    set(p.labels[2], "ACCURACY " + std::to_string(int(s.accuracy())) + "%   BPM " +
                         std::to_string(int(p.session->track().bpm)));
    int energy = int(s.energy * 100), bars = energy / 10;
    set(p.labels[3], "ENERGY [" + std::string(bars, '|') + std::string(10 - bars, '.') + "] " +
                         std::to_string(energy) + "%");
    set(p.labels[4],
        t - p.session->last_judgement_time() < .75
            ? rhythm_judgement_name(p.session->last_judgement())
            : (p.session->beat_position(t) - std::floor(p.session->beat_position(t)) < .18
                   ? "*  TAP  *"
                   : "   TAP"));
    auto ranks = p.bots->ranking(s.score);
    int rank = 1;
    for (size_t i = 0; i < ranks.size(); ++i)
        if (ranks[i].local)
            rank = int(i + 1);
    for (int i = 0; i < 5; ++i)
        set(p.labels[6 + i],
            std::to_string(i + 1) + ". " + ranks[i].name + "  " + std::to_string(ranks[i].score));
    set(p.labels[11], "YOUR RANK  #" + std::to_string(rank));
    set(p.labels[12], rank == 1 && s.score > 0 ? "STAR OF THE DANCE FLOOR" : "");
    set(p.labels[13], p.audio->diagnostic());
    const auto windows = p.session->windows();
    double vals[] = {p.session->track().bpm,
                     p.settings->volume,
                     p.session->track().offset,
                     windows.perfect * 1000,
                     windows.great * 1000,
                     windows.good * 1000,
                     s.energy * 100,
                     double(p.settings->crowd_count),
                     p.settings->stage_intensity};
    const char *names[] = {"BPM",     "Volume",   "Offset s", "Perfect ms", "Great ms",
                           "Good ms", "Energy %", "Crowd",    "Lights"};
    const double offsets[] = {(vals[0] - 40) / 190,
                              vals[1],
                              (vals[2] + 1) / 2,
                              (windows.perfect - .005) / .195,
                              (windows.great - .005) / .195,
                              (windows.good - .005) / .195,
                              s.energy,
                              (vals[7] - 30) / 20,
                              vals[8] / 3};
    for (int i = 0; i < 9; ++i)
    {
        p.sliders[i]->set_offset(static_cast<float>(offsets[i]));
        std::ostringstream out;
        out << names[i] << " " << std::fixed
            << std::setprecision(i == 1 || i == 2 || i == 8 ? 2 : 0) << vals[i];
        set(p.values[i], out.str());
    }
}
} // namespace game
