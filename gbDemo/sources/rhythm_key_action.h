#pragma once
namespace game
{
enum class RhythmKeyAction
{
    none,
    tap,
    restart,
    debug
};
inline RhythmKeyAction rhythm_key_action(int key, bool apple)
{
    if (key == (apple ? 49 : 32))
        return RhythmKeyAction::tap;
    if (key == (apple ? 15 : 82) || (!apple && key == 114))
        return RhythmKeyAction::restart;
    if (key == (apple ? 122 : 112))
        return RhythmKeyAction::debug;
    return RhythmKeyAction::none;
}
} // namespace game
