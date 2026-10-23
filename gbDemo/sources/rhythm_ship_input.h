#pragma once
#include "input_context.h"
#include "rhythm_audio_transport.h"
#include "rhythm_gesture.h"
#include "rhythm_session.h"
namespace game
{
class RhythmShipInput : public gb::input_context_listener
{
    std::shared_ptr<RhythmSession> session_;
    std::shared_ptr<gb::al::RhythmAudioTransport> audio_;
    std::set<i32> held_;
    std::function<bool(glm::ivec2)> captured_;
    std::function<void()> restart_, debug_, graphics_;
    std::function<void(glm::ivec2, float)> scroll_;
    std::function<void(RhythmInput)> tap_;
    std::function<std::uint64_t()> epoch_;
    std::function<bool()> enabled_;
    std::function<float()> threshold_;
    RhythmGesture gesture_;
    bool allowed() const;
    void emit(const RhythmInput &);

  public:
    RhythmShipInput(std::shared_ptr<RhythmSession>, std::shared_ptr<gb::al::RhythmAudioTransport>,
                    std::function<bool(glm::ivec2)>, std::function<void()>, std::function<void()>,
                    std::function<void()> graphics = {},
                    std::function<void(glm::ivec2, float)> scroll = {},
                    std::function<void(RhythmInput)> tap = {},
                    std::function<std::uint64_t()> epoch = {}, std::function<bool()> enabled = {},
                    std::function<float()> threshold = {});
    void cancel_gesture()
    {
        gesture_.cancel();
    }
    void on_gr_cancelled(gb::e_input_source, ui32) override
    {
        gesture_.cancel();
    }
    void on_input_focus_lost() override
    {
        gesture_.cancel();
        held_.clear();
    }
    void on_gr_scrolled(const glm::ivec2 &p, const glm::ivec2 &, f32 delta) override
    {
        if (scroll_)
            scroll_(p, delta);
    }
    void on_gr_pressed(const glm::ivec2 &, const glm::ivec2 &, gb::e_input_source, ui32) override;
    void on_gr_released(const glm::ivec2 &, const glm::ivec2 &, gb::e_input_source, ui32) override;
    void on_gr_moved(const glm::ivec2 &, const glm::ivec2 &, const glm::ivec2 &, ui32) override
    {
    }
    void on_gr_dragged(const glm::ivec2 &, const glm::ivec2 &, const glm::ivec2 &,
                       gb::e_input_source, ui32) override;
    void on_key_down(i32) override;
    void on_key_up(i32 k) override
    {
        held_.erase(k);
    }
    void on_virtual_keyboard_input(const std::string &) override
    {
    }
    void on_virtual_keyboard_backspace() override
    {
    }
    void on_virtual_keyboard_hidden() override
    {
    }
};
} // namespace game
