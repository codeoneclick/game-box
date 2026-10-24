#pragma once
#include <cstdint>
#include <limits>
#include <string>
namespace game
{
enum class RhythmAction
{
    Tap,
    SwipeLeft,
    SwipeRight,
    SwipeUp,
    SwipeDown
};
inline const char *rhythm_action_name(RhythmAction a)
{
    static const char *names[] = {"Tap", "SwipeLeft", "SwipeRight", "SwipeUp", "SwipeDown"};
    const unsigned i = static_cast<unsigned>(a);
    return i < 5 ? names[i] : "Invalid";
}
inline bool parse_rhythm_action(const std::string &name, RhythmAction &out)
{
    for (unsigned i = 0; i < 5; ++i)
        if (name == rhythm_action_name(static_cast<RhythmAction>(i)))
        {
            out = static_cast<RhythmAction>(i);
            return true;
        }
    return false;
}
struct RhythmInput
{
    double time;
    RhythmAction action;
    RhythmInput(double t = 0, RhythmAction a = RhythmAction::Tap) : time(t), action(a)
    {
    }
};
struct RhythmEvent
{
    double time;
    RhythmAction action;
    std::uint64_t beat_index;
    std::uint32_t phrase_id;
    unsigned phrase_step;
    double strength;
    RhythmEvent(double t = 0, RhythmAction a = RhythmAction::Tap, std::uint64_t b = 0,
                std::uint32_t p = 0, unsigned step = 0, double s = 1)
        : time(t), action(a), beat_index(b), phrase_id(p), phrase_step(step), strength(s)
    {
    }
};
struct RhythmScheduledEvent
{
    std::uint64_t id;
    RhythmEvent event;
    bool resolved;
    RhythmScheduledEvent(std::uint64_t i = 0, RhythmEvent e = RhythmEvent(), bool r = false)
        : id(i), event(e), resolved(r)
    {
    }
};
} // namespace game
