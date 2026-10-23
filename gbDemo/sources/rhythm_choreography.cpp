#include "rhythm_choreography.h"
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
        const auto beats = resolve_rhythm_beats(source);
        auto result = source;
        result.events.clear();
        result.generated_choreography = true;
        using A = RhythmAction;
        const std::array<std::vector<A>, 7> phrases = {
            {{A::Tap, A::Tap, A::SwipeRight, A::Tap},
             {A::SwipeLeft, A::SwipeRight, A::SwipeLeft, A::SwipeRight},
             {A::Tap, A::Tap, A::SwipeUp, A::SwipeUp},
             {A::Tap, A::Tap, A::Tap, A::SwipeDown},
             {A::SwipeLeft, A::Tap, A::SwipeRight, A::Tap},
             {A::SwipeLeft, A::SwipeRight, A::SwipeUp, A::SwipeDown},
             {A::Tap, A::SwipeRight, A::Tap, A::SwipeLeft, A::Tap, A::SwipeRight, A::Tap,
              A::SwipeDown}}};
        unsigned hash = settings.seed;
        for (unsigned char c : source.id)
            hash = (hash ^ c) * 16777619u;
        size_t beat = 0;
        unsigned phrase = 0;
        while (beat < beats.size())
        {
            const bool easy = settings.difficulty == RhythmDifficulty::Easy;
            const bool fast =
                !easy &&
                (settings.difficulty == RhythmDifficulty::Hard ? phrase % 2 == 1 : phrase % 4 == 3);
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
                size_t index = beat + (fast ? step / 2 : easy ? (step / 2) * 3 + step % 2 : step);
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
            beat += easy ? 6 : 4;
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
