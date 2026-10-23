#include "rhythm_ship_input.h"
#include "rhythm_key_action.h"
namespace game
{
RhythmShipInput::RhythmShipInput(std::shared_ptr<RhythmSession> s,
                                 std::shared_ptr<gb::al::RhythmAudioTransport> a,
                                 std::function<bool(glm::ivec2)> c, std::function<void()> r,
                                 std::function<void()> d, std::function<void()> g,
                                 std::function<void(glm::ivec2, float)> scroll,
                                 std::function<void(RhythmInput)> tap,
                                 std::function<std::uint64_t()> epoch,
                                 std::function<bool()> enabled, std::function<float()> threshold)
    : session_(s), audio_(a), captured_(c), restart_(r), debug_(d), graphics_(g), scroll_(scroll),
      tap_(tap), epoch_(epoch), enabled_(enabled), threshold_(threshold)
{
}
bool RhythmShipInput::allowed() const
{
    return audio_->mode() != gb::al::RhythmAudioMode::initializing && (!enabled_ || enabled_());
}
void RhythmShipInput::emit(const RhythmInput &input)
{
    if (tap_)
        tap_(input);
    else
        session_->enqueue_input(input);
    if (std::getenv("GB_RHYTHM_DIAGNOSTICS"))
        std::cout << "Rhythm input=" << rhythm_action_name(input.action) << " raw=" << input.time
                  << std::endl;
}
void RhythmShipInput::on_gr_pressed(const glm::ivec2 &p, const glm::ivec2 &, gb::e_input_source src,
                                    ui32 i)
{
    if (src == gb::e_input_source_mouse_left && i == 0 && !captured_(p) && allowed())
    {
        gesture_.threshold = threshold_ ? threshold_() : 24;
        gesture_.begin(p.x, p.y, audio_->seconds(), epoch_ ? epoch_() : 0);
    }
    else
        gesture_.cancel();
}
void RhythmShipInput::on_gr_dragged(const glm::ivec2 &p, const glm::ivec2 &area, const glm::ivec2 &,
                                    gb::e_input_source src, ui32 i)
{
    if (src != gb::e_input_source_mouse_left || i != 0)
        return;
    if (!allowed() || captured_(p) || p.x < 0 || p.y < 0 || p.x >= area.x || p.y >= area.y)
    {
        gesture_.cancel();
        return;
    }
    RhythmInput input;
    if (gesture_.move(p.x, p.y, audio_->seconds(), epoch_ ? epoch_() : 0, input))
        emit(input);
}
void RhythmShipInput::on_gr_released(const glm::ivec2 &p, const glm::ivec2 &area,
                                     gb::e_input_source src, ui32 i)
{
    if (src != gb::e_input_source_mouse_left || i != 0)
        return;
    bool valid = allowed() && !captured_(p) && p.x >= 0 && p.y >= 0 && p.x < area.x && p.y < area.y;
    if (!valid)
    {
        gesture_.cancel();
        return;
    }
    RhythmInput input;
    if (gesture_.end(p.x, p.y, audio_->seconds(), epoch_ ? epoch_() : 0, input))
        emit(input);
}
void RhythmShipInput::on_key_down(i32 k)
{
    if (!held_.insert(k).second)
        return;
#if defined(__OSX__)
    const auto action = rhythm_key_action(k, true);
#else
    const auto action = rhythm_key_action(k, false);
#endif
    if ((action == RhythmKeyAction::tap || action == RhythmKeyAction::left ||
         action == RhythmKeyAction::right || action == RhythmKeyAction::up ||
         action == RhythmKeyAction::down) &&
        allowed())
    {
        auto a = action == RhythmKeyAction::left    ? RhythmAction::SwipeLeft
                 : action == RhythmKeyAction::right ? RhythmAction::SwipeRight
                 : action == RhythmKeyAction::up    ? RhythmAction::SwipeUp
                 : action == RhythmKeyAction::down  ? RhythmAction::SwipeDown
                                                    : RhythmAction::Tap;
        emit(RhythmInput(audio_->seconds(), a));
    }
    else if (action == RhythmKeyAction::restart)
    {
        gesture_.cancel();
        restart_();
    }
    else if (action == RhythmKeyAction::debug)
    {
        gesture_.cancel();
        debug_();
    }
    else if (action == RhythmKeyAction::graphics && graphics_)
    {
        gesture_.cancel();
        graphics_();
    }
}
} // namespace game
