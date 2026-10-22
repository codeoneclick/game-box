# Execution ledger — docs/superpowers/plans/2026-10-22-rhythm-ship.md

User approved design, plan and recommended native execution. Baseline Windows Debug build passed.
Ruling: work in the existing diskoteka feature checkout — user selected execution here; preserve the two pre-existing design/plan documentation changes and existing build outputs. No branch merge/push.
Ruling: use this portable ledger instead of Bash-only Superpowers bookkeeping scripts on Windows; same task evidence is recorded here.
Pre-flight: track -> session/audio agree on absolute seconds and loop endpoint; session -> crowd/HUD agree on shared state; input -> session queues timestamps; session events -> bots/effects consumed once per update; scene -> audio teardown owns stop/listener removal.
Tasks 1/2 RED: check-rhythm.ps1 failed because new domain headers/sources did not exist, as expected.

Tasks 1/2: domain implemented; check-rhythm.ps1 passed, including thresholds, late delivery, expiry, duplicates, restart, looping, malformed JSON, zero-time explicit beats, energy and bots.
Task 3 RED: audio checker failed for missing transport; GREEN: check-rhythm-audio.ps1 passed forced silent mode, sample placement and real played-sample clock.
Ruling: audio backend implementation stays in one guarded C++ file instead of three trivial platform translation units; public API remains platform independent. Apple execution is unavailable here.
Ruling: polling audio integration check allows 3 seconds to start instead of assuming sound starts within 100 ms; observed hardware startup latency exceeded 100 ms. Primary time remains the played-sample counter.
Tasks 4-7: scene, crowd, input, HUD and native debug sliders implemented; Debug build passed. Vulkan 50-character PCM runtime check passed with keyboard scoring, multiframe captures, restart and normal exit. Separate silent/debug run passed native button leader override and energy controls; captured leader banner/crown.
Ruling: share eight atlas quad meshes by costume rather than one per-entity atlas uniform; all 51 entities share atlas/material shader resources. This avoids per-character GPU updates.
Ruling: use sliders, without editable text fields; keyboard capture is therefore unnecessary. Validated numeric values are shown beside each slider.
Ruling: preserve empty configured passes via a hidden material registry entity. Legacy ces_render_system indexes an unregistered pass as -1 and crashed on empty grass/shadow/debug passes; stack captured at ces_render_system.cpp:180. No core change needed.
Ruling: integrate scene and crowd in one initial compile increment because they share environment helpers; domain/audio checks remained separate before integration. All targets are recompiled before final validation.
Task 8: asset/project checker passed (13 canonical assets, all three Apple demo target memberships); Release build and final review in progress.

Final review: independent read-only gpt-6-astra review found seven Important issues, no Critical issues. One consolidated fix pass:
- JSON roots []/42/string: observed vendored JsonCpp assertion RED, added object guard, focused suite GREEN.
- macOS key mapping and offset/explicit-map phase: added pure action mapping and session beat_position, new checks RED for missing interfaces then GREEN; crowd/cue now consume the resolved timeline.
- Apple compiler flags: asset checker observed missing per-source exceptions/RTTI overrides RED; registration now applies narrow -fexceptions/-frtti overrides and checker GREEN. C++11-compatible window constructors retained for older targets.
- Apple async startup/cache/failure/stalled-loop defects: replaced legacy async player use with synchronous owned OpenAL PCM source/buffer on the existing engine context; no callbacks, temp files or audio caches. Static checks prohibit the legacy path; shared unwrapping arithmetic checks cover 65-second stalls and monotonic output. Native Apple build/runtime remains UNKNOWN.
Ruling: Apple uses the existing engine OpenAL context directly through the gbSound adapter rather than legacy play2d/get_current_time — inspected legacy APIs recycle pending sources, retain caches and publish state before ID insertion; synchronous PCM removes these hazards without editing engine audio classes. Cost: native Apple validation still required; custom music decoding remains future work on both platforms.
Final: reusable sub-sample PCM duration was regraded Important because it could divide by zero; observed rejection assertion RED, reject zero-sample durations, audio suite GREEN.
Final: authored-map BPM control disabled; high energy now adds strips, fragment glow and brief beat flashes. Dedicated beat pulse remains visible independently of the judgement label.
Final: all seven Important findings addressed; no deferred minor findings. Review's declined items (networking/music pipeline/movement/buoyancy/skeletal animation, slider text focus, empty-pass workaround, unavailable native platforms and server) retain the approved scope/recorded limitations.
Final runtime rerun passed Debug silent/debug controls and Release PCM with 50 dancers, validation, ranking gain/loss, score/reset and normal shutdown. One script failure was a one-Hz diagnostic missing a brief leader interval; transition diagnostics now record every leader change and the same behavior passes.
Ruling: commit the integrated vertical slice together after verification instead of independent intermediate task commits — source membership and scene helpers were integrated together; avoids commits referring to files absent from their tree. Cost: less granular history, preserved standalone checks and this ledger supply the task evidence.

Final verification after all code changes:
- Windows debug-client|x64 build: passed.
- Windows release-client|x64 build: passed.
- check-rhythm.ps1: passed.
- check-rhythm-audio.ps1: passed, real PCM played-sample clock measured and forced silent mode exercised.
- check-rhythm-ship-assets.py with Blender 5.2 Python 3: passed, 13 assets and all applicable MSVC/Xcode memberships/compiler overrides.
- check-rhythm-ship-runtime.ps1 -CrowdCount 50 -Silent -DebugPanel: passed, Vulkan validation, keyboard score, native debug actions, rank changes, restart, normal shutdown.
- check-rhythm-ship-runtime.ps1 -Configuration release-client -CrowdCount 50 -DebugPanel: passed, real PCM clock and same Vulkan/UI lifecycle checks.
- Final captures visually inspected: 50 characters, DJ, YOU/ring/crown, leaderboard/banner, native labelled buttons, energy, beat indicator, ocean and complete stage/deck.
- Ocean/sky GLSL, Metal ocean source and their existing material assets: unchanged relative to base 7da03dd1. Saved graphics and water presentation preferences are now loaded just as in the old scene.
- Additional old-island check: initialized residents/presentation, but capture timed out at 45 seconds and shutdown was forced by the existing runner. Retained island runtime is UNKNOWN; this does not establish a pre-existing cause. No original island or core source was changed.
- Apple/Metal/OpenGL/server: not built or exercised. Native Apple fixes have source/portable arithmetic/membership checks only; audible output not independently heard/recorded.
- git diff --check: passed. All 57 task files inspected through status; no generated staged resources, vendored dependencies or core engine sources edited.

Tasks 1-8: implementation complete, subject to the explicit platform/old-island verification limits above. Branch remains diskoteka; no merge or push requested.
