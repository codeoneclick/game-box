# Party Liner Asset Implementation Plan

> **For agentic workers:** Use superpowers:executing-plans inline in the existing approved `diskoteka` checkout. One final fresh-context asset review.

**Goal:** Deliver the approved editable voxel liner and actual-model previews.
**Architecture:** Author grouped cuboids through native Blockbench free-format APIs in an isolated local instance. Save the native codec output and embedded palette texture. Render previews from the saved model, validate native reload and geometry/clearance without modifying the engine or current scene.
**Tech Stack:** Blockbench Electron/CDP, JavaScript authoring, Python/Blender preview renderer.
**Spec:** `docs/superpowers/specs/2026-10-24-party-liner-design.md` (approved by user).

## Constraints and review focus

- Three funnels, stepped black/red hull, white decks, stern dance floor, DJ stage; no runtime integration.
- Target 15k–35k triangles; report actual counts. Named editable groups, one embedded/exported atlas.
- Y up, negative Z toward stage; clear floor accommodates 30–50 characters.
- No user project overwrite; separate Blockbench user profile and debugging port.
- Preview is actual geometry, not a generated reference image. Color/emission annotations do not promise in-engine light.

## Task 1: Native model authoring

Files: create `gbScripts/author-party-liner.js`, `gbScripts/invoke-party-liner.mjs`, `gbResources/rhythm_party_liner.bbmodel`, `gbResources/rhythm_party_liner/palette.png`.

- [x] Launch dedicated Blockbench and inspect actual native APIs using established authoring example.
- [x] Build hull/deck silhouette, clear dance floor and superstructure with three funnels.
- [x] Add grouped stage, console, speakers/screen, stairs, promenade/lifeboat/rail/lamp details.
- [x] Save native codec output, texture and authoring report; reload in Blockbench and compare geometry/groups/textures.

## Task 2: Visual verification and delivery

Files: create `gbScripts/render-party-liner.py`, `gbResources/rhythm_party_liner/README.md`, previews and asset report in that directory. Scratch logs/profile remain under ignored `gbDemo/output/rhythm-check/party-liner`.

- [x] Render overview and playable-deck views from the saved model; inspect and refine proportions/details if necessary.
- [x] Check finite geometry, atlas assignments, bounds, actual triangle counts and obstacle-free dance floor. Review saved geometry and authoring for major failures.
- [x] Record scale, placement, palette/emission groups, checks and runtime integration boundary.
- [x] Run git diff --check, inspect status, commit only asset/tooling/docs changes locally. No engine build needed for source-only art assets; no backend claims.

## Execution ledger

- Initial status: only approved design doc untracked; no unrelated edits.
- Ruling: use existing diskoteka checkout, preserving the established session workflow; no runtime source changes.
- Ruling: no implementation-mirroring unit tests for reversible asset authoring; inspect native reload and actual geometry instead.

- Native authoring/reload: Blockbench 5.2.2, free format, 2,290 cubes / 27,480 triangles / 34 groups; embedded/external 128x128 palette verified. Native bounds match independent transformed bounds to <0.001.
- All three final geometry-based Blender 5.2.1 previews rendered and inspected; the native Blockbench viewport was saved and inspected too. Preview-only water at Y=-13.5; engine placement notes explicitly account for the existing ocean at Y=0.
- Fresh review found a 1.8-unit gap between stage stairs and platform. Added saved-geometry top-tread connection check; observed RED on the old asset, regenerated stairs extending inward to +/-18 and observed GREEN. Reviewer independently confirmed both 0.2-unit overlaps at matching Y=6.55; no remaining Important/Critical findings.
- Final render command and --validate-only command both exit 0. Headroom / play-area clearance, unique IDs, UVs, atlas, three funnels, source SHA and native bounds checks PASS. node --check on authoring/runner PASS.
- No runtime/core/assets staging or backend code changes; engine builds intentionally not run. Separate initial Blockbench attempts failed due restricted Desktop path; the final isolated instance launched outside that restriction without touching the user's original process/project.
- Ruling: reduce preview samples from 32 to 8 with denoising after first full overview inspection; geometry and source are unchanged, preview-only rendering is faster.
- Keep this delivery on the existing local diskoteka branch; no merge/push/publication.
