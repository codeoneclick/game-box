#pragma once
#include "input_context.h"
#include "rhythm_audio_transport.h"
#include "rhythm_session.h"
namespace game
{
class RhythmShipInput : public gb::input_context_listener
{
    std::shared_ptr<RhythmSession> session_;
    std::shared_ptr<gb::al::RhythmAudioTransport> audio_;
    std::set<i32> held_;
    std::function<bool(glm::ivec2)> captured_;
    std::function<void()> restart_, debug_;

  public:
    RhythmShipInput(std::shared_ptr<RhythmSession>, std::shared_ptr<gb::al::RhythmAudioTransport>,
                    std::function<bool(glm::ivec2)>, std::function<void()>, std::function<void()>);
    void on_gr_pressed(const glm::ivec2 &, const glm::ivec2 &, gb::e_input_source, ui32) override;
    void on_gr_released(const glm::ivec2 &, const glm::ivec2 &, gb::e_input_source, ui32) override
    {
    }
    void on_gr_moved(const glm::ivec2 &, const glm::ivec2 &, const glm::ivec2 &, ui32) override {}
    void on_gr_dragged(const glm::ivec2 &, const glm::ivec2 &, const glm::ivec2 &,
                       gb::e_input_source, ui32) override
    {
    }
    void on_key_down(i32) override;
    void on_key_up(i32 k) override { held_.erase(k); }
    void on_virtual_keyboard_input(const std::string &) override {}
    void on_virtual_keyboard_backspace() override {}
    void on_virtual_keyboard_hidden() override {}
};
} // namespace game
