п»ї# Audio accent choreography implementation plan

> **For agentic workers:** Use superpowers:executing-plans in the existing approved checkout; audio-clock diagnosis is an independent scratch-only parallel task.

**Goal:** Replace repeated BPM-template commands for MP3s with playable actions anchored to attacks, bursts and energy rises/drops in their decoded audio.
**Architecture:** Offline NumPy analysis emits per-track `accents` (exact time, proposed action, salience). The demo filters this material for difficulty, action mask, reaction time and burst budget without creating or moving notes. Existing explicit `events` stay authoritative; the metronome keeps its synthetic fallback. Original PCM/audio and rendering backends stay unchanged unless measured transport evidence proves a bug.
**Tech stack:** Existing C++11 demo, Python3/NumPy, pinned decoder, WinMM and native Vulkan smoke scripts.
**Spec:** User requests in the current thread: actions must follow actual musical accents, spikes/drops, with playable varied series and rests, not repeated templates or an arbitrary interval.

- [x] Independently measure output clock against loopback using scratch-only helper; record measured lag/drift or exact limitation.
- [x] Add an audio accent extractor and synthetic-waveform checks proving exact known attacks, no generated notes in silence and variation between different recordings. Inspect real Adrenaline output statistics and waveform overlays before runtime changes.
- [x] Extend existing track/event data with source accents and salience. Validate ordering, duration, bounds, count and numeric fields. RED/GREEN parsing and duration-reconciliation regressions.
- [x] Add an accent selection path to existing choreography generator. Preserve source times, use audio-derived action suggestions, prioritize stronger accents, allow short Tap clusters and leave time for directional steps; difficulty/masks must not reintroduce a template.
- [x] Regenerate all nine canonical music maps from unchanged PCM; keep analysis provenance and per-event evidence. Existing metronome and authored JSON compatibility remain.
- [x] Verify PCM-to-action correspondence independently of a BPM grid, difficulty/restart/calibration, Debug/Release builds and native Adrenaline gameplay. Review, document actual evidence and remaining listening uncertainty, commit locally.

No new C++ sources or core ECS/render contracts. No publication/merge/push. Do not declare audible synchrony based solely on unit-grid membership or build success.

Evidence: docs/rhythm-ship-audio-analysis.md and the 2026-10-24 entry in docs/rhythm-stage-steps.md. Physical Bluetooth listening remains unverified; required device-specific calibration is documented.
