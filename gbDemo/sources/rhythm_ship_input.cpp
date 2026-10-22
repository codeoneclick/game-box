#include "rhythm_ship_input.h"
#include "rhythm_key_action.h"
namespace game
{
RhythmShipInput::RhythmShipInput(std::shared_ptr<RhythmSession> s,
                                 std::shared_ptr<gb::al::RhythmAudioTransport> a,
                                 std::function<bool(glm::ivec2)> c, std::function<void()> r,
                                 std::function<void()> d)
    : session_(s), audio_(a), captured_(c), restart_(r), debug_(d)
{
}
void RhythmShipInput::on_gr_pressed(const glm::ivec2 &p, const glm::ivec2 &, gb::e_input_source src,
                                    ui32 i)
{
    if (src == gb::e_input_source_mouse_left && i == 0 && !captured_(p) &&
        audio_->mode() != gb::al::RhythmAudioMode::initializing)
        session_->enqueue_tap(audio_->seconds());
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
    if (action == RhythmKeyAction::tap && audio_->mode() != gb::al::RhythmAudioMode::initializing)
        session_->enqueue_tap(audio_->seconds());
    else if (action == RhythmKeyAction::restart)
        restart_();
    else if (action == RhythmKeyAction::debug)
        debug_();
}
} // namespace game
