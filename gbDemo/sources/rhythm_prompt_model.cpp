#include "rhythm_prompt_model.h"
#include <algorithm>
#include <cmath>
namespace game
{
RhythmPromptState rhythm_prompt_state(const RhythmSession &session, double t, double lead,
                                      int count)
{
    RhythmPromptState out;
    if (!std::isfinite(t) || t < 0)
        return out;
    lead = std::isfinite(lead) ? std::max(.25, std::min(3., lead)) : 2.0;
    count = std::max(0, std::min(3, count));
    const auto future = session.upcoming(t, static_cast<size_t>(count + 1));
    if (!future.empty())
    {
        out.current = future[0];
        const double dt = out.current.event.time - t;
        out.visible = dt <= lead;
        double start = std::max(0., out.current.event.time - lead);
        if (out.current.id)
        {
            const double previous = session.scheduled_event(out.current.id - 1).event.time;
            start = std::max(start, (previous + out.current.event.time) * .5);
        }
        const double approach = std::max(.001, out.current.event.time - start);
        out.progress = std::max(0., std::min(1., 1 - dt / approach));
        const double age = -dt;
        if (age >= 0 && age < .12)
            out.beat_pulse = std::exp(-age * 25);
        for (size_t i = 1; i < future.size(); ++i)
            out.preview.push_back(future[i]);
        out.phrase = "PHRASE " + std::to_string(out.current.event.phrase_id + 1) + "  /  " +
                     std::to_string(out.current.event.phrase_step + 1);
    }
    RhythmResolution last;
    if (session.last_resolution(last))
    {
        const double age = t - last.presentation_time;
        if (age >= 0 && age < .45)
        {
            out.feedback = last.judgement;
            out.feedback_alpha = 1 - age / .45;
        }
    }
    out.combo = session.score().combo;
    return out;
}
} // namespace game
