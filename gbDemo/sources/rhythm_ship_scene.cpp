#include "rhythm_ship_scene.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "ces_action_component.h"
#include "ces_dance_crowd_system.h"
#include "ces_ocean_system.h"
#include "ces_render_system.h"
#include "ces_sun_shadow_system.h"
#include "common.h"
#include "game_transition.h"
#include "graphics_context.h"
#include "graphics_settings.h"
#include "ocean_shader_uniforms.h"
#include "render_pipeline.h"
#include "render_technique_ws.h"
#include "rhythm_ship_environment.h"
#include "rhythm_ship_input.h"
#include "rhythm_ship_ui.h"
#include "scene_fabricator.h"
#include "ss_render_technique_custom_uniforms.h"
#include "stylized_lighting_settings.h"
#include "stylized_post_process_uniforms.h"
#include "texture.h"
#include "window_impl.h"
#include <cstdlib>
namespace game
{
struct rhythm_ship_scene::state
{
    std::shared_ptr<RhythmSession> session;
    std::shared_ptr<gb::al::RhythmAudioTransport> audio =
        std::make_shared<gb::al::RhythmAudioTransport>();
    std::shared_ptr<FakeLeaderboard> bots = std::make_shared<FakeLeaderboard>();
    std::shared_ptr<RhythmShipDebugSettings> settings = std::make_shared<RhythmShipDebugSettings>();
    std::shared_ptr<RhythmShipUI> ui;
    std::shared_ptr<ces_dance_crowd_system> crowd;
    RhythmShipEnvironment environment;
    double last_beat = -10, last_log = -10;
    bool last_leader = false;
    void restart(const RhythmTrack &t)
    {
        RhythmTrack copy = t;
        *session = RhythmSession(copy);
        bots->restart();
        last_beat = -10;
        audio->start({copy.duration, resolve_rhythm_beats(copy), copy.audio_file});
        audio->set_volume(settings->volume);
        std::cout << "Rhythm restart bpm=" << copy.bpm << " " << audio->diagnostic() << std::endl;
    }
};
rhythm_ship_scene::~rhythm_ship_scene()
{
    if (auto c = context_.lock())
        c->remove_listener(input_);
    if (state_)
        state_->audio->stop();
}
void rhythm_ship_scene::create()
{
    gb::scene_graph::create();
    auto s = state_ = std::make_shared<state>();
    RhythmTrack track;
    std::string error;
    bool loaded = false;
    for (const auto &path :
         {bundlepath() + "rhythm_ship_track.json",
          std::string("../gbBundle/Configurations/rhythm_ship/rhythm_ship_track.json"),
          std::string("rhythm_ship_track.json")})
        if (load_rhythm_track(path, track, error))
        {
            loaded = true;
            break;
        }
    if (!loaded)
        std::cerr << "Rhythm config fallback: " << error << std::endl;
    s->session = std::make_shared<RhythmSession>(track);
    if (const char *n = std::getenv("GB_RHYTHM_CROWD_COUNT"))
        s->settings->crowd_count = glm::clamp(std::atoi(n), 30, 50);
    const auto resolution = get_transition()->get_window()->get_resolution_size_in_points();
    set_camera_2d(std::make_shared<gb::camera_2d>(resolution.x, resolution.y));
    auto camera = std::make_shared<gb::camera_3d>(
        50.f, .5f, 6000.f, glm::ivec4(0, 0, resolution.x, resolution.y), true);
    set_camera_3d(camera);
    camera->set_look_at(glm::vec3(0, 3, -5));
    camera->set_rotation(110);
    camera->set_distance_to_look_at(glm::vec3(78, 65, 78));
    camera->get_mat_v();
    auto technique = add_component<gb::ces_render_technique_uniforms_component>();
    auto saved_post=stylized_post_settings_from_environment();
    auto saved_lighting=lighting_settings_from_environment();
    std::string graphics_error;
    if(!load_graphics_settings(graphics_settings_path(),saved_post,saved_lighting,graphics_error))
        std::cout<<"Rhythm graphics defaults: "<<graphics_error<<std::endl;
    stylized_lighting_settings()=lighting_settings_from_environment(&saved_lighting);
    auto post = std::make_shared<StylizedPostProcessSettings>(stylized_post_settings_from_environment(&saved_post));
    auto final = technique->construct_uniforms<stylized_post_process_uniforms>(
        gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "final");
    auto output = technique->construct_uniforms<stylized_post_process_uniforms>(
        gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "demo");
    auto compose = technique->construct_uniforms<ocean_field_debug_uniforms>(
        gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "compose");
    compose->set(glm::vec4(0, 0, float(post->implementationStage), 0), "field_debug");
    auto ao = technique->construct_uniforms<ssao_shader_uniforms>(
        gb::ces_render_technique_uniforms_component::e_shader_uniform_type_fragment, "ss.ao");
    ao->set(glm::vec4(3, .06f, post->enableSSAO ? post->ssaoIntensity : 0, 1), "ao_params");
    std::array<glm::ivec2, 2> sizes = {{glm::ivec2(0), glm::ivec2(0)}};
    for (const auto &t : get_transition()
                             ->get_system<gb::ces_render_system>()
                             ->get_render_pipeline()
                             ->get_ws_techniques())
    {
        auto tex = t->get_color_attachment_texture();
        if (tex)
            for (int i = 0; i < 2; ++i)
                if (tex->get_guid() ==
                    std::string(i == 0 ? "ws.sun.near" : "ws.sun.far") + ".attachment_01")
                    sizes[i] = glm::ivec2(tex->get_width(), tex->get_height());
    }
    auto shadows = std::make_shared<ces_sun_shadow_system>(get_fabricator(), sizes);
    shadows->initialize(shared_from_this());
    shadows->set_world_dimensions(2048);
    get_transition()->add_system(shadows);
    s->environment =
        create_rhythm_ship_environment(get_fabricator(), shared_from_this(), get_transition());
    s->environment.ocean->set_optics_intensity(glm::vec3(
        post->enableWaterReflection?post->waterReflectionIntensity:0.f,
        post->enableWaterRefraction?post->waterRefractionIntensity:0.f,
        post->enableWaterCaustics?post->waterCausticsIntensity:0.f));
    populate_rhythm_ship_crowd(get_fabricator(), shared_from_this());
    s->crowd = std::make_shared<ces_dance_crowd_system>(s->session, s->audio);
    s->crowd->set_active_count(s->settings->crowd_count);
    get_transition()->add_system(s->crowd);
    std::weak_ptr<state> weak = s;
    s->ui = std::make_shared<RhythmShipUI>(get_fabricator(), s->session, s->bots, s->audio,
                                           s->settings, resolution,
                                           [weak](const RhythmTrack &t)
                                           {
                                               if (auto p = weak.lock())
                                                   p->restart(t);
                                           });
    add_child(s->ui->root());
    if (const char *debug = std::getenv("GB_RHYTHM_DEBUG"))
        if (std::string(debug) == "1")
            s->ui->toggle_debug();
    std::weak_ptr<RhythmShipUI> ui = s->ui;
    input_ = std::make_shared<RhythmShipInput>(
        s->session, s->audio,
        [ui](glm::ivec2 p)
        {
            auto view = ui.lock();
            return view && view->contains(p);
        },
        [weak]
        {
            if (auto p = weak.lock())
                p->restart(p->session->track());
        },
        [ui]
        {
            if (auto p = ui.lock())
                p->toggle_debug();
        });
    context_ = get_transition()->get_input_context();
    get_transition()->get_input_context()->add_listener(input_);
    bool manual_gamma = !gb::graphics_context::get_current_context()->is_output_srgb();
    auto action = gb::ces_entity::construct<gb::ces_entity>();
    action->add_component<gb::ces_action_component>()->set_update_callback(
        [s, camera, post, final, output, manual_gamma,
         resolution](const gb::ces_entity_shared_ptr &, float)
        {
            double t = s->audio->seconds();
            camera->set_rotation(110 + std::sin(float(t) * .09f) * 1.1f);
            camera->get_mat_v();
            final->update(*post, camera->get_position(), camera->get_look_at(), camera->get_mat_v(),
                          camera->get_mat_p(), -.3f, manual_gamma);
            output->update(*post, camera->get_position(), camera->get_look_at(),
                           camera->get_mat_v(), camera->get_mat_p(), -.3f, manual_gamma);
            if (s->audio->mode() != gb::al::RhythmAudioMode::initializing)
            {
                s->session->update(t);
                for (const auto &b : s->session->beat_events())
                {
                    s->bots->on_beat(b.id);
                    s->last_beat = b.time;
                }
            }
            if (s->ui->consume_force_beat())
                s->last_beat = t;
            s->crowd->set_active_count(s->settings->crowd_count);
            auto ranks = s->bots->ranking(s->session->score().score);
            const bool leader = ranks[0].local && ranks[0].score > 0;
            if (leader != s->last_leader)
            {
                if (std::getenv("GB_RHYTHM_DIAGNOSTICS"))
                    std::cout << "Rhythm leader=" << leader
                              << " score=" << s->session->score().score << std::endl;
                s->last_leader = leader;
            }
            update_rhythm_ship_effects(s->environment, t, s->last_beat, s->session->score().energy,
                                       s->settings->stage_intensity,
                                       ranks[0].local && ranks[0].score > 0);
            s->ui->update();
            const auto player_clip =
                camera->get_mat_p() * camera->get_mat_v() * glm::vec4(0, 7.3f, 9, 1);
            if (player_clip.w > 0)
                s->ui->set_player_position(
                    glm::vec2((player_clip.x / player_clip.w * .5f + .5f) * resolution.x,
                              (.5f - player_clip.y / player_clip.w * .5f) * resolution.y));
            if (std::getenv("GB_RHYTHM_DIAGNOSTICS") && t - s->last_log >= 1)
            {
                s->last_log = t;
                std::cout << "Rhythm time=" << t << " score=" << s->session->score().score
                          << " combo=" << s->session->score().combo
                          << " leader=" << (ranks[0].local && ranks[0].score > 0)
                          << " energy=" << s->session->score().energy
                          << " crowd=" << s->settings->crowd_count << " " << s->audio->diagnostic()
                          << std::endl;
            }
        });
    add_child(action);
    s->restart(track);
    std::cout << "Rhythm Ship initialized crowd=" << s->settings->crowd_count
              << " ocean=existing sky=existing" << std::endl;
}
} // namespace game
