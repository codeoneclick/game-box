# Building Placement Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox syntax for tracking.

**Goal:** Add three authored buildings, previews, valid/invalid placement feedback and persistent editor commands.

**Architecture:** Extend the existing tree-placement/editor flow. Logical building instances live in TerrainEditorState; immutable static resources render through a scene-owned helper. A CPU surface index validates the entire rotated footprint on one terrace. GPU work stays on the game loop.

**Tech Stack:** C++14, native gbUI, Blender/FBX, existing Vulkan/GLSL and Metal assets.

**Spec:** `docs/superpowers/specs/2026-10-14-building-placement-design.md` (approved, including future rotation around local Y).

## Global Constraints

- Preserve unrelated changes and source bbmodels; canonical assets in gbBundle.
- Scale 1/16 m per authored unit, clearance 0.5 m, Y-up / +Z-facing.
- Continuous float yaw around the exported model's own base-center pivot.
- Entire oriented construction area on one above-water terrace; no overlapping buildings.
- Model and preview files named `forester_hut`, `stonecutters_hut`, `woodcutter_hut`.
- No renderer core or generated configuration changes. Register new C++ and assets in applicable VS/Xcode targets.

## Review Focus

- Group/element rotations and different embedded textures with the same source name.
- Concave terrace or internal hole despite four valid footprint corners.
- Arbitrary yaw, self-axis rotation and footprint overlap after rotation.
- Pending terrain rebuild, load generation, and edits near a protected building foundation.
- Legacy tool value 3 means retired Climate, not new Buildings; old saves must retain that meaning.

## Interfaces and ownership

`building_catalog.h` defines `BuildingType { Forester, Stonecutter, Woodcutter }`,
`BuildingDefinition { const char* name; const char* mesh_filename; const char* texture_filename; const char* preview_filename; float half_width; float half_depth; }`, and `building_definition(BuildingType)`.
Exported mesh is already scaled and grounded; half sizes cover the transformed full model, without the extra 0.5m margin.

`building_placement.h/.cpp` defines logical `EditorBuilding` with `id`, `type`,
`position`, `yaw` (radians), `terrace`; `BuildingSurfaceSupport` indexes actual
top triangles and verifies full oriented rectangular coverage at a height.

TerrainEditorState exposes `selected_building`, `building_yaw`,
`building_surface` (shared immutable support), `building_surface_revision`,
`can_place_building(float x,float z) const`, `place_building(float x,float z,bool erase=false)`,
and `rotate_building(float delta_radians)`. Buildings is tool value 4;
value 3 remains reserved for legacy Climate. Renderer/input query these APIs.

The surface index receives complete CPU terrain parts on the mesh worker;
main publishes it only after corresponding queued GPU batches are installed.
Placement is blocked while support revision differs from state terrain revision.

## Task 1: Assets and catalog

Files: create `gbScripts/export-buildings.py`, `gbDemo/sources/building_catalog.h`, three FBX/PNG preview assets, unique embedded texture assets and export report.

- [x] Inspect transforms/UV convention; write export integrity checks including rotated groups and source hashes.
- [x] Export static meshes with authored faces/UVs and hierarchy rotations, centered XZ and grounded Y.
- [x] Render orthographic transparent previews of actual models; calculate exact rotated bounds and generated catalog.
- [x] Verify exported triangles, units, orientation, texture identity, previews and unchanged source hashes.

## Task 2: Logical placement, support, commands and saves

Files: `building_placement.*`, `terrain_editor_state.*`, `gbScripts/check_building_placement.cpp`, extend editor CPU checks.

- [x] Add failing coverage tests: flat support, edge, internal hole, second height, arbitrary yaw, overlapping rectangles.
- [x] Implement chunk-bucket triangle clipping against rotated footprint and same-height total coverage.
- [x] Add building deltas, stable IDs, undo/redo, footprint vegetation clearing and foundation protection including contour halo.
- [x] Serialize GBLV v3 and read v1/v2 with reserved Climate mapping; test exact roundtrips and invalid data atomically.
- [x] Run CPU checks and verify no building changes occur on rejected placement.

## Task 3: Native UI

Files: `terrain_editor_ui.*`, three preview sprite/material XMLs.

- [x] Add fourth visible tool Buildings (explicit mapping to value 4), with actual house thumbnail icon.
- [x] Add three native preview cards and current selection; hide other tool panels when Buildings is active.
- [x] Add lower status/rotation hints without restoring removed Climate/Brush Preview controls.
- [x] Check native persistent selection, pointer bounds and visibility switching.

## Task 4: Scene/input/render/navigation integration

Files: `editor_building_renderer.*`, `terrain_editor_input.*`, `island_demo_scene.*`, resource XML/shaders, build metadata.

- [x] Wire cursor position, R rotation, single-click placement and Shift removal; camera/UI gestures cannot place buildings.
- [x] Load three shared immutable meshes/materials and synchronize instances, preview and colored footprint aura.
- [x] Produce/publish support indexes with revision checks after terrain GPU upload; block stale placement.
- [x] Exclude building footprints from resident navigation and refresh on placement/removal/load.
- [x] Add all C++/assets to VS and Xcode, including GLSL/Metal variants.

## Task 5: Verification and review

- [x] Run CPU export, placement, history, serialization and existing editor checks.
- [x] Build affected Windows targets; if running user's Debug exe locks PDB, use isolated preview output without closing their app.
- [x] Vulkan smoke all three cards, valid/invalid placement, rotation, occupied areas, removal and Undo/Redo; concave coverage and Save/Load checked on CPU.
- [x] Capture screenshots; verify normal exit and no validation errors. Release placement navigation rebuild ran on worker (~57 ms), main-loop install ~3.4 ms.
- [ ] Native Save/Load automation: Windows showed "Location is not available"; no runtime save verification claimed.
- [x] Request code review, resolve important findings, document controls and report unverified backends.
