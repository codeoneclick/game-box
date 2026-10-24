#include "rhythm_choreography.h"
#include <algorithm>
#include <array>
#include <stdexcept>
namespace game
{
bool generate_rhythm_choreography(const RhythmTrack &source,
                                  const RhythmChoreographySettings &settings, RhythmTrack &out,
                                  std::string &error)
{
    try
    {
        unsigned enabled = settings.enabled_actions & 31;
        if (settings.difficulty == RhythmDifficulty::Easy)
            enabled &= 7;
        if (!enabled)
            throw std::invalid_argument("Enable at least one action for this difficulty");
        if (static_cast<unsigned>(settings.difficulty) > 2)
            throw std::invalid_argument("Invalid difficulty");
        if (!source.accents.empty())
        {
            const auto accents = resolve_rhythm_accents(source);
            auto result = source;
            result.events.clear();
            result.generated_choreography = true;
            const bool easy = settings.difficulty == RhythmDifficulty::Easy;
            const bool hard = settings.difficulty == RhythmDifficulty::Hard;
            const double minimum_strength = easy ? .60 : hard ? .20 : .35;
            std::vector<double> recent;
            unsigned phrase = 0, step = 0;
            for (size_t i = 0; i < accents.size(); ++i)
            {
                auto event = accents[i];
                if (event.time < 1.5 || event.strength < minimum_strength ||
                    !(enabled & (1u << static_cast<unsigned>(event.action))))
                    continue;
                const bool tap = event.action == RhythmAction::Tap;
                const double spacing = easy ? .50 : hard ? (tap ? .20 : .42) : (tap ? .30 : .65);
                const double previous =
                    result.events.empty() ? -10 : result.events.back().time + source.offset;
                const bool previous_tap =
                    result.events.empty() || result.events.back().action == RhythmAction::Tap;
                const double recovery =
                    previous_tap ? spacing : std::max(spacing, hard ? .35 : .50);
                if (event.time - previous < recovery - 1e-9)
                    continue;
                // Reserve the next recovery interval for a materially stronger
                // accent rather than filling it with a preceding weak transient.
                bool stronger = false;
                for (size_t j = i + 1;
                     j < accents.size() && accents[j].time - event.time < (hard ? .35 : .65); ++j)
                    if (accents[j].strength > event.strength + .15 &&
                        (enabled & (1u << static_cast<unsigned>(accents[j].action))))
                        stronger = true;
                if (stronger)
                    continue;
                recent.erase(std::remove_if(recent.begin(), recent.end(),
                                            [&](double t) { return event.time - t > 1.5; }),
                             recent.end());
                if (recent.size() >= static_cast<size_t>(hard ? 5 : 3))
                    continue;
                if (event.time - previous > .9)
                {
                    ++phrase;
                    step = 0;
                }
                event.phrase_id = phrase;
                event.phrase_step = step++;
                recent.push_back(event.time);
                event.time -= source.offset;
                result.events.push_back(event);
            }
            if (result.events.empty())
                throw std::invalid_argument("No playable audio accents for these settings");
            resolve_rhythm_events(result);
            out = std::move(result);
            error.clear();
            return true;
        }
        const auto beats = resolve_rhythm_beats(source);
        auto result = source;
        result.events.clear();
        result.generated_choreography = true;
        using A = RhythmAction;
        const std::array<std::vector<A>, 7> phrases = {
            {{A::Tap, A::Tap, A::Tap, A::SwipeUp},
             {A::SwipeLeft, A::SwipeRight, A::SwipeLeft, A::SwipeRight},
             {A::Tap, A::Tap, A::SwipeUp, A::SwipeUp},
             {A::Tap, A::Tap, A::Tap, A::SwipeDown},
             {A::SwipeLeft, A::Tap, A::SwipeRight, A::Tap},
             {A::SwipeLeft, A::SwipeRight, A::SwipeUp, A::SwipeDown},
             {A::Tap, A::SwipeRight, A::Tap, A::SwipeLeft, A::Tap, A::SwipeRight, A::Tap,
              A::SwipeDown}}};
        // Beat offsets within a phrase: short same-action series,
        // deliberate rests, and more space before a change of direction.
        const std::array<std::array<size_t, 4>, 6> offsets = {{{{0, 1, 2, 7}},
                                                               {{0, 2, 4, 6}},
                                                               {{0, 1, 4, 6}},
                                                               {{0, 1, 2, 7}},
                                                               {{0, 2, 4, 6}},
                                                               {{0, 2, 4, 6}}}};
        unsigned hash = settings.seed;
        for (unsigned char c : source.id)
            hash = (hash ^ c) * 16777619u;
        // Let the player hear four musical pulses before the first command.
        size_t beat = beats.size() > 4 ? 4 : 0;
        unsigned phrase = 0;
        while (beat < beats.size())
        {
            const bool easy = settings.difficulty == RhythmDifficulty::Easy;
            const bool hard = settings.difficulty == RhythmDifficulty::Hard;
            const bool fast = hard && phrase % 2 == 1;
            const size_t stride = easy ? 2 : 1;
            unsigned pattern = fast ? 6 : (hash + phrase) % 6;
            if (phrase == 0)
                pattern = 0; // A recognizable, readable introduction.
            if (easy)
            {
                const unsigned simple[] = {0, 1, 4};
                pattern = simple[(hash + phrase) % 3];
            }
            const auto &actions = phrases[pattern];
            for (size_t step = 0; step < actions.size(); ++step)
            {
                size_t index = beat + (fast   ? step / 2
                                       : hard ? step
                                              : offsets[pattern][step] * stride);
                if (index >= beats.size())
                    break;
                double time = beats[index];
                if (fast && step % 2)
                {
                    if (index + 1 >= beats.size())
                        break;
                    time = (time + beats[index + 1]) * .5;
                }
                if (time >= source.duration)
                    continue;
                A action = actions[step];
                if (!(enabled & (1u << static_cast<unsigned>(action))))
                {
                    unsigned fallback = 0;
                    while (!(enabled & (1u << fallback)))
                        ++fallback;
                    action = static_cast<A>(fallback);
                }
                result.events.push_back(RhythmEvent(time - source.offset, action, index, phrase,
                                                    static_cast<unsigned>(step)));
            }
            // Leave a trailing rest so an Up/Down at step 7 is not immediately
            // followed by another directional command at a phrase boundary.
            beat += hard ? 4 : 12 * stride;
            ++phrase;
        }
        while (!result.events.empty() &&
               source.duration - result.events.back().time + result.events.front().time <
                   .060 - 1e-9)
            result.events.pop_back();
        if (result.events.empty())
            throw std::invalid_argument("Track has no playable choreography");
        resolve_rhythm_events(result);
        out = std::move(result);
        error.clear();
        return true;
    }
    catch (const std::exception &e)
    {
        error = e.what();
        return false;
    }
}
} // namespace game
