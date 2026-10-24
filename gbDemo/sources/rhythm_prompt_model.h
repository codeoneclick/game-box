#pragma once
#include "rhythm_session.h"
namespace game
{
struct RhythmPromptState
{
    bool visible = false;
    double progress = 0, feedback_alpha = 0;
    double beat_pulse = 0;
    RhythmScheduledEvent current;
    std::vector<RhythmScheduledEvent> preview;
    RhythmJudgement feedback = RhythmJudgement::none;
    std::uint64_t combo = 0;
    std::string phrase;
};
RhythmPromptState rhythm_prompt_state(const RhythmSession &, double raw_time, double lead_seconds,
                                      int preview_count);
} // namespace game
