#pragma once
namespace game
{
enum class RhythmKeyAction
{
    none,
    tap,
    restart,
    debug,
    graphics,
    left,
    right,
    up,
    down
};
inline RhythmKeyAction rhythm_key_action(int key, bool apple)
{
    if (key == (apple ? 49 : 32))
        return RhythmKeyAction::tap;
    if (key == (apple ? 15 : 82) || (!apple && key == 114))
        return RhythmKeyAction::restart;
    if (key == (apple ? 122 : 112))
        return RhythmKeyAction::debug;
    if (key == (apple ? 100 : 119))
        return RhythmKeyAction::graphics;
    if (key == (apple ? 123 : 37))
        return RhythmKeyAction::left;
    if (key == (apple ? 124 : 39))
        return RhythmKeyAction::right;
    if (key == (apple ? 126 : 38))
        return RhythmKeyAction::up;
    if (key == (apple ? 125 : 40))
        return RhythmKeyAction::down;
    return RhythmKeyAction::none;
}
} // namespace game
