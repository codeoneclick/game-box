#include "rhythm_prompt_view.h"
#include "ces_transformation_2d_component.h"
#include "game_object_2d.h"
#include "label.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include <cmath>
namespace game
{
struct RhythmPromptView::implementation
{
    gb::scene_fabricator_shared_ptr fab;
    gb::game_object_2d_shared_ptr root = gb::ces_entity::construct<gb::game_object_2d>();
    gb::game_object_2d_shared_ptr ring;
    struct Icon
    {
        gb::game_object_2d_shared_ptr root;
        gb::game_object_2d_shared_ptr tap;
        std::vector<gb::sprite_shared_ptr> tap_fill;
        std::array<gb::sprite_shared_ptr, 3> arrow;
    };
    std::array<Icon, 4> icons;
    gb::label_shared_ptr feedback, phrase, combo;
    gb::sprite_shared_ptr bar(const gb::ces_entity_shared_ptr &parent, glm::vec2 p, glm::vec2 size,
                              float rotation, glm::u8vec4 color)
    {
        // Flat color preserves thin glyphs at preview/ring scale. The editor's
        // rounded-panel shader has a pixel inset that erases these small bars.
        auto s = fab->create_sprite("button_background.xml");
        s->size = size;
        s->pivot = glm::vec2(.5f);
        s->position = p;
        // This engine's vendored GLM rotate() takes degrees; layout uses radians.
        s->rotation = glm::degrees(rotation);
        s->color = color;
        parent->add_child(s);
        return s;
    }
    gb::label_shared_ptr label(glm::vec2 p, const std::string &text, float size)
    {
        auto l = fab->create_label_2d("button_label.xml");
        l->font_name = "Vera.ttf";
        l->font_size = size;
        l->text = text;
        l->position = p;
        l->font_color = glm::u8vec4(245, 250, 255, 255);
        root->add_child(l);
        return l;
    }
    void build()
    {
        ring = gb::ces_entity::construct<gb::game_object_2d>();
        root->add_child(ring);
        for (int i = 0; i < 32; ++i)
        {
            const float a = i * 6.283185307f / 32;
            bar(ring, glm::vec2(std::cos(a), std::sin(a)) * 50.f, glm::vec2(10, 6), a + 1.5707963f,
                glm::u8vec4(70, 238, 255, 220));
        }
        for (size_t i = 0; i < icons.size(); ++i)
        {
            auto &icon = icons[i];
            icon.root = gb::ces_entity::construct<gb::game_object_2d>();
            root->add_child(icon.root);
            if (i)
            {
                icon.root->position = glm::vec2(-32.f + static_cast<float>(i - 1) * 32.f, -70);
                icon.root->scale = glm::vec2(.45f);
            }
            const auto color = i ? glm::u8vec4(160, 180, 230, 200) : glm::u8vec4(255, 210, 72, 255);
            icon.tap = gb::ces_entity::construct<gb::game_object_2d>();
            icon.root->add_child(icon.tap);
            // Filled circle from horizontal strips; reuses the native flat material.
            for (int row = 0; row < 20; ++row)
            {
                const float y = row - 9.5f;
                const float width = 2 * std::sqrt(100.f - y * y);
                icon.tap_fill.push_back(
                    bar(icon.tap, glm::vec2(0, y), glm::vec2(width, 1), 0, color));
            }
            icon.arrow[0] = bar(icon.root, glm::vec2(0, 2), glm::vec2(5, 27), 0, color);
            icon.arrow[1] = bar(icon.root, glm::vec2(-5, -9), glm::vec2(5, 17), .785398f, color);
            icon.arrow[2] = bar(icon.root, glm::vec2(5, -9), glm::vec2(5, 17), -.785398f, color);
        }
        phrase = label(glm::vec2(-65, 85), "", 10);
        feedback = label(glm::vec2(-45, 28), "", 16);
        combo = label(glm::vec2(-40, 106), "", 12);
        float depth = -.3f;
        std::function<void(const gb::ces_entity_shared_ptr &)> prepare =
            [&](const gb::ces_entity_shared_ptr &n) {
                if (auto tr = n->get_component<gb::ces_transformation_2d_component>())
                {
                    tr->set_is_in_camera_space(false);
                    tr->set_z_order(depth);
                    depth += .0004f;
                }
                n->enumerate_children(prepare);
            };
        prepare(root);
    }
    void icon(Icon &i, RhythmAction action, bool visible)
    {
        i.root->visible = visible;
        i.root->visible_in_next_frame = visible;
        if (!visible)
            return;
        const bool tap = action == RhythmAction::Tap;
        i.tap->visible = tap;
        i.tap->visible_in_next_frame = tap;
        for (auto s : i.arrow)
        {
            s->visible = !tap;
            s->visible_in_next_frame = !tap;
        }
        i.root->rotation = action == RhythmAction::SwipeLeft    ? -90.f
                           : action == RhythmAction::SwipeRight ? 90.f
                           : action == RhythmAction::SwipeDown  ? 180.f
                                                                : 0;
    }
};
RhythmPromptView::RhythmPromptView(const gb::scene_fabricator_shared_ptr &fab)
    : impl_(std::make_shared<implementation>())
{
    impl_->fab = fab;
    impl_->build();
}
gb::ces_entity_shared_ptr RhythmPromptView::root() const
{
    return impl_->root;
}
void RhythmPromptView::update(const RhythmPromptState &state, glm::vec2 anchor, glm::ivec2 viewport,
                              bool enabled)
{
    auto &p = *impl_;
    const bool show = enabled && state.visible && std::isfinite(anchor.x) &&
                      std::isfinite(anchor.y) && anchor.x >= -50 && anchor.y >= 0 &&
                      anchor.x <= viewport.x + 50 && anchor.y <= viewport.y;
    p.root->visible = show;
    p.root->visible_in_next_frame = show;
    if (!show)
        return;
    p.root->position =
        glm::clamp(anchor, glm::vec2(90, 150), glm::vec2(viewport.x - 90, viewport.y - 230));
    const float radius = static_cast<float>(1.25 - state.progress * .95);
    p.ring->scale = glm::vec2(radius);
    p.icon(p.icons[0], state.current.event.action, true);
    const auto current_color = glm::u8vec4(255, 210, 72, state.current.resolved ? 65 : 255);
    for (auto strip : p.icons[0].tap_fill)
        strip->color = current_color;
    for (auto arrow : p.icons[0].arrow)
        arrow->color = current_color;
    p.icons[0].root->scale = glm::vec2(.85f + .25f * static_cast<float>(state.progress));
    for (size_t i = 1; i < 4; ++i)
        p.icon(p.icons[i],
               i <= state.preview.size() ? state.preview[i - 1].event.action : RhythmAction::Tap,
               i <= state.preview.size());
    const std::string message =
        state.feedback_alpha > 0 ? rhythm_judgement_name(state.feedback) : "";
    if (static_cast<std::string>(p.feedback->text) != message)
        p.feedback->text = message;
    p.feedback->font_color =
        state.feedback == RhythmJudgement::wrong || state.feedback == RhythmJudgement::miss
            ? glm::u8vec4(255, 100, 130, static_cast<ui8>(255 * state.feedback_alpha))
            : glm::u8vec4(100, 255, 225, static_cast<ui8>(255 * state.feedback_alpha));
    if (static_cast<std::string>(p.phrase->text) != state.phrase)
        p.phrase->text = state.phrase;
    const auto combo = "COMBO x" + std::to_string(state.combo);
    if (static_cast<std::string>(p.combo->text) != combo)
        p.combo->text = combo;
}
} // namespace game
