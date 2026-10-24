# Rhythm Ship stage prompt and anchored steps — 2026-10-24

User correction to f2adde7d: move the action prompt from the player to the front of the DJ booth; Tap must be a circle; four swipes must produce steps along the deck and return to the character's anchor; every bot uses its own 0–330 ms reaction delay.

Implemented using existing demo helpers and native sprite material; no new engine interfaces, source memberships, assets or shaders. The fixed world prompt anchor is (0, 8, -23), projected before UI update. Tap is a filled circle; arrow rotations are exact -90/90/0/180 degrees for Left/Right/Up/Down. The timing ring and previews remain with the relocated prompt. The old player-attached indicator is gone.

Step offsets use ship X/Z coordinates: Left/Right along X, Up towards the DJ (-Z), Down away (+Z). The envelope moves out over 180 ms, holds for 100 ms and returns by 550 ms. Each frame derives position from base_position plus the temporary offset; horizontal idle drift was removed. Vertical idle dance remains. A subsequent swipe replaces the temporary pose; it cannot accumulate displacement or change the anchor.

Each bot has a deterministic delay in [0, .33) seconds, fixed for the run and independent of event ID. Its outcome and movement start at event time plus that delay. Existing occasional misses/wrong actions remain. Delays beyond the Good window score as misses, reflecting rhythm skill. Shared outcomes still feed both choreography and leaderboard.

During native inspection, verified that the vendored GLM rotate() converts its angle from degrees internally. The prior prompt supplied radians, which barely rotated arrows. Converted prompt layout angles and crowd billboard yaw/lean to degrees at the transform boundary. No engine/vendored changes.

Checks:
- Gesture regression reproduced old Up/Down jump/crouch, then passed after planar steps; separate delay regression reproduced early bot outcomes, then passed with fixed nonnegative per-bot delays.
- check-rhythm-gestures.ps1: PASS, including four step directions, exact return at 550 ms, stable bounded bot delays, deterministic update partitioning, and existing gesture/prompt/stage checks.
- check-rhythm.ps1: PASS.
- Windows Debug and Release builds: PASS (stage-step-degrees-debug.log, stage-step-degrees-release.log).
- Vulkan Debug 50-character portrait runtime, scoring and normal shutdown: PASS (stage-step-degrees-runtime.log). Captures inspected for relocated circle/ring, previews and crowd layout.
- Native directional controls, cancellation, F8, stale F1 captures and focus loss: PASS (stage-step-input.log).
- check-rhythm-ship-assets.py and git diff --check: PASS.
- Independent source review and targeted degree-conversion re-review: no actionable findings.

Logs/captures are ignored under gbDemo/output/rhythm-check. Apple/Metal, dormant OpenGL and server builds/runtime remain unverified on this Windows host. Audio content/transport and ocean/skybox/stage geometry were not changed.

Final Release Vulkan 50-character runtime and Release native directional/cancellation/UI/focus-loss checks also passed (stage-step-final-release-runtime.log, stage-step-final-release-input.log), including normal shutdown.
