# Island Resident Implementation Plan

> Executed natively using superpowers:executing-plans after user approval. Checked items below record completion and the actual verification scope.

**Goal:** Add an original male low-poly resident with idle/walk animations, autonomous safe shoreline walks and one shared shirt/trouser color to the F5 island demo.

**Architecture:** Blender exports a rigged FBX and matching clips; the existing fabricator, skeleton and animation mixer load them. Demo-owned CPU navigation/behavior controls movement; a scene-specific ECS system selects animation before evaluation and supplies evaluated bones to a resident material before rendering. The material writes the ocean's existing three opaque-scene attachments.

**Tech Stack:** Installed `C:/Program Files/Blender Foundation/Blender 5.2/blender.exe` (headless Python), existing ufbx import, C++/MSBuild, Vulkan/shaderc, GLSL/Metal, standalone CPU smoke checks and Vulkan captures.

**Spec:** `docs/superpowers/specs/2026-10-09-island-resident-design.md` (approved).

## Global Constraints

- One male character, flat shading, elongated proportions, approximately 1,000–3,000 triangles, target height 1.9 metres.
- Shirt and trousers receive the same clothing color simultaneously; boots, skin, hair and belt retain fixed colors.
- Looping in-place idle and walk; approximately 1.2 m/s locomotion, idle stops of 2–5 seconds.
- First-version movement stays on the lowest connected dry terrace at Y=0.5 m, with a safe edge margin. No upper-wall crossing, swimming, IK or crowd simulation.
- At most four bone influences per vertex, comfortably below 64 bones; verify normalized weights and matched clip skeletons.
- Put runtime source assets in gbBundle and application behavior in gbDemo; preserve previous ocean/camera changes and F5 behavior.
- No generated/vendor edits, new renderer framework or engine-wide navigation framework. Explicitly report unavailable Apple/OpenGL/server verification.
- Use the approved user's current F5 workspace; preserve existing working changes. No unrelated cleanup or implicit staging of prior changes.

## Review Focus

1. FBX export may change axes, units or bind transforms: Task 1 verifies scale/feet origin and Task 4 verifies actual GPU pose against CPU skinning.
2. Packed skin weights occupy the old vertex color channel and PT4B omits normals: Task 4 uses UV mask/palette and derivative flat normals, with normalized shader weights.
3. Logical terrain cells differ from smoothed rendered contours: Task 2 samples actual shore triangles and checks complete segments with footprint/edge margin.
4. Large dt or unreachable destinations could move the resident off land: Task 3 clamps processed time and tests fallback to idle with no invalid movement.
5. Equal system orders, first-frame pose and teardown can produce stale buffers/listeners: Tasks 4–6 test phase ordering, identity initialization, removed entities and recreated setup.

## Verified integration facts

- `scene_fabricator::create_shape_3d()` creates the configured mesh/material and calls `add_animation_sequences_3d()` when animation entries exist.
- FBX animation filenames support `file.fbx#ActionName`; use matching names in exported actions and XML. The importer samples FBX frame rate.
- `shape_3d::play_animation(name, true)` and the existing mixer provide loop selection/blending. Do not reset the clip each frame.
- Animation runs at order 3, render at 7. All systems' `on_feed_start` run before any `on_feed`.
- PT4B provides position, packed UV, bone IDs and normalized byte weights. It has no normals. Never use weights as clothing vertex colors.
- Custom uniforms are already supported in the renderer: Metal uses indexed buffers; Vulkan converts GLSL uniforms into custom descriptor blocks at bindings 11/12. Use a 64-matrix vertex wrapper at Metal index 2 and a separate fragment wrapper at index 0.
- Do not pass the resident through `ces_ocean_system::add_receiver`: it creates/replaces the same uniform component. Its own material writes compatible scene data; it stays above water.
- New files need membership in both demo MSBuild files and all three existing Xcode targets. The Blender script runs in Blender's bundled Python, not the system Python 2.7.

### Task 1: Original rigged asset and animation exports

**Files:** Create `gbScripts/create-island-resident.py`, `gbResources/island_resident/island_resident.blend`, `gbBundle/Meshes/island_resident.fbx`, `gbBundle/Meshes/island_resident_idle.fbx`, `gbBundle/Meshes/island_resident_walk.fbx`, `gbBundle/Textures/island_resident_palette.png`, `gbBundle/Textures/island_resident_clothing_mask.png`, and `gbScripts/check-island-resident-assets.py`.

**Interfaces:** Script CLI `--output-root <repository> --preview-dir <directory>` passed after Blender's `--`. Produce a Y-up engine-space 1.9 m character with feet at ground origin after import. Idle action name `idle`, walk action name `walk`; FBX clips contain the same armature/bind pose as the mesh. UVs point into fixed palette patches; shirt/trouser UVs sample white palette patches and mask value 1, other parts mask value 0.

- [x] Add Blender-side asset checks for 1,000–3,000 triangles, finite coordinates, 1.9 m height, <=32 bones, positive normalized weights with <=4 influences, UVs/mask coverage and both named clips. Running the check before generation must fail for missing files.
- [x] Generate the original angular male mesh with complete joint connections, shirt, trousers, boots, hair and simple facial features. Save the editable scene. Build a humanoid rig with root, pelvis, torso/head, arm and leg chains; keep static root translation.
- [x] Author idle at 30 fps, 90-frame cycle; walk at 30 fps, 24-frame cycle. Use a 0.96 m cycle distance for matching 1.2 m/s in-place cadence. Export with bake settings verified against the installed Blender version; do not export cameras/lights or leaf bones.
- [x] Export palette/mask PNGs with nearest-sampled color patches large enough to avoid border bleed. Render front, side and back previews; inspect silhouette, boots and joint behavior in idle/walk poses.
- [x] Run Blender headlessly to generate then validate assets. Verify ufbx sees expected bone names/order, clip durations, scale and bind matrices using a focused importer probe. Correct export settings before changing any engine code.
- [x] Register runtime assets in existing Xcode resource phases and source Blender files only as authoring assets. Record output paths and generation command.

Generation command (PowerShell, repository root):

```powershell
& 'C:/Program Files/Blender Foundation/Blender 5.2/blender.exe' --background --python gbScripts/create-island-resident.py -- --output-root . --preview-dir gbDemo/output/island-check/resident-asset
& 'C:/Program Files/Blender Foundation/Blender 5.2/blender.exe' --background gbResources/island_resident/island_resident.blend --python gbScripts/check-island-resident-assets.py -- --output-root .
```

Expected: exit 0 and an explicit asset validation summary; missing exports,
invalid bone weights or absent clips fail with a nonzero exit code.

### Task 2: Safe navigation on the rendered shore

**Files:** Create `gbDemo/sources/resident_navigation.h/.cpp`, `gbScripts/check_island_resident.cpp`, `gbScripts/check-island-resident.ps1`. Modify demo project/filter/Xcode membership.

**Interfaces:** Define CPU `resident_point {float x,z;}` and `resident_navigation` storing a 1 m grid, world origin, shore Y and walkability. Expose `resident_navigation build_resident_navigation(const terrain_mesh_data&, float shore_y, float cell_size=1.f)`, `bool resident_navigation::is_safe(resident_point) const`, `bool resident_navigation::segment_is_safe(resident_point,resident_point) const`, `std::vector<resident_point> resident_navigation::find_path(resident_point,resident_point) const`, and `std::vector<resident_point> resident_navigation::reachable_points(resident_point) const`.

- [x] Add CPU smoke cases for the actual default shore, empty mesh, displaced contour, narrow strip and a ring around an impassable interior. A route must remain on the safe rendered shore; reject water, upper terraces and direct shortcuts across a bay/inner wall. Run before implementation; expect missing-symbol compile failure.
- [x] Rasterize only upward triangles at `shore_y`; preserve triangle coverage for point tests. Check footprint samples with 0.4 m body radius plus 0.8 m edge margin. A walkable center must have complete support, not merely one logical cell.
- [x] Implement four-neighbor connected-component discovery and shortest-path search; return an empty path for invalid or disconnected endpoints. Validate route segments at <=0.25 m intervals against the same footprint-safe triangle coverage. Avoid unchecked path smoothing and diagonal corner cuts.
- [x] Run the CPU check on multiple terrain seeds/resolutions. Require at least one usable connected shore region for defaults and safe failure for insufficient space. Add build metadata for the two new source files.

CPU check names: `check_rendered_shore_support`, `check_ring_path`,
`check_disconnected_path`, `check_narrow_shore`, `check_smoothed_contour_margin`.
For every returned route point require `assert(nav.is_safe(point));` and for
every consecutive pair require `assert(nav.segment_is_safe(a,b));`.
Run `./gbScripts/check-island-resident.ps1`; expected exit 0 with navigation
smoke summary after the initial missing-implementation compile failure.

### Task 3: Deterministic autonomous walk/idle behavior

**Files:** Create `gbDemo/sources/resident_behavior.h/.cpp`; extend `check_island_resident.cpp` and project membership.

**Interfaces:** Define `resident_state` containing position, yaw, idle timer, route/index, mode and seeded PRNG state. Expose `resident_state create_resident_state(const resident_navigation&, uint32_t seed)` and `void step_resident(const resident_navigation&, float dt, resident_state&)`. State creation chooses a safe visible shoreline spawn; empty navigation produces an explicit unavailable state and no bogus origin spawn.

- [x] Add CPU checks for alternating idle/walk states, constant bounded walking speed, waypoint completion, yaw continuity and deterministic destinations for the same seed. Test zero, negative/nonfinite dt, 0.25/2 s stalls, unavailable destinations and >100 simulated routes; all accepted positions and segments must remain safe.
- [x] Implement idle stops in [2,5] seconds, reachable destination selection, route following at 1.2 m/s and turn rate <=180 degrees/s. Cap processed dt at 0.1 s. Consume movement budget across waypoints without overshoot; use idle if a route cannot be selected or becomes invalid.
- [x] Run the standalone CPU smoke check. Keep these units free of renderer/ECS dependencies so behavior verification does not require a window.

Extend the same command with `check_idle_walk_cycle`, `check_reachable_targets`,
`check_stall_movement_budget`, `check_nonfinite_dt`, and `check_empty_navigation`.
Assert zero/negative/nonfinite dt does not advance state and a long stall moves
no farther than 0.12 m. Require both modes to occur and every route to remain safe
during the long simulated run.

### Task 4: Animated resident material and uniform adapter

**Files:** Create `gbDemo/sources/resident_shader_uniforms.h/.cpp`, `gbDemo/sources/ces_island_resident_component.h/.cpp`, `gbDemo/sources/ces_island_resident_system.h/.cpp`, `gbBundle/Shaders/shader_island_resident.vert/.frag`, `gbBundle/Shaders/metal/mtl_island_resident.h`, `gbBundle/Configurations/game_objects/island_resident.xml`, `gbBundle/Configurations/game_objects/materials/material.island_resident.xml`. Modify `mtl_library.metal` and project membership.

**Interfaces:** Component owns `resident_state` and one clothing color. System owns immutable navigation and registers a mask for resident, transform, mixer and custom uniforms. Vertex wrapper owns 64 mat4 values initialized to identity and a named `u_bones` matrix-array uniform; fragment wrapper owns view mat4, clothing vec4 and sun vec4 (96 bytes). Match GLSL declaration order to packed C++/Metal data.

- [x] Construct the configured FBX shape through the fabricator, attach wrappers and start idle only after validating skeleton/clip readiness. Reject >64 or mismatched bones with a clear diagnostic. Set rotating radius bounds covering all animated poses.
- [x] Implement `on_feed_start` to advance Task 3 state, set position/yaw and call `play_animation` only on mode changes. Register the system at order 6; in `on_feed` copy evaluated mixer transformations after animation order 3, then update color/view/sun. Do not change feeder, render or core animation contracts.
- [x] Implement resident skinning with renormalized packed weights and guarded bone IDs. Pass skinned world position and UV. In the fragment shader derive flat face normals from world-position derivatives, sample palette and clothing mask, apply one shared color, then write color/encoded normal/world-position with positive linear view depth to attachments 0/1/2. Implement matching Metal functions and GLSL compatibility in the existing conventions.
- [x] Capture fixed idle/walk poses in a close inspection view. Inspect CPU-skinned bounds/projection diagnostics alongside GPU captures to detect axis/bind/order mistakes. Numerical per-vertex GPU readback remains unperformed. Verify normals/lighting from front/back and no shaderc or descriptor validation errors. Initialize wrappers before first draw; never upload uninitialized matrices.
- [x] Build Windows and verify new XML/asset/uniform bindings. Report Metal/OpenGL as source updates, not tested targets. No generic renderer changes unless a focused failing probe proves the existing API cannot carry the required data.

### Task 5: Island population and simultaneous recoloring

**Files:** Create `gbDemo/sources/island_resident_demo.h/.cpp`; modify `island_demo_scene.h/.cpp`, runtime capture tooling and project membership.

**Interfaces:** `resident_demo_handles create_island_resident_demo(const gb::scene_fabricator_shared_ptr&, const terrain_mesh_data&, float shore_y)` returns configured shape, system and a color input listener. The scene owns/attaches these and explicitly registers/removes the listener. Color input listener weakly references the resident entity; palette updates affect only that resident's single clothing setting.

- [x] Populate one resident on a safe visible foreground shore point, attach the component, register its system and add it to the scene. If assets/navigation are invalid, log the specific failure and preserve the rest of the ocean demo.
- [x] Bind `C` to cycle muted blue, green, red and ochre clothing colors. One assignment updates both clothing regions; ignore repeated keydown until keyup. Keep boots, belt, hair and skin fixed. Preserve the existing orbit/pan/zoom listener.
- [x] Add an inspection mode `GB_RESIDENT_INSPECT=1` and verification-only fixed state switches for idle/walk poses and color. Freeze autonomous movement only in diagnostic mode; normal F5 runs autonomous behavior.
- [x] Capture every clothing palette entry in the same pose; compare shirt/trouser pixels and unchanged boot/skin/hair regions. Exercise C while walking, then camera orbit, RMB pan and wheel zoom. Render data must compose correctly with water.
- [x] Exercise resident detach/recreation and scene setup teardown in a focused runtime probe; removed listeners/queries must not retain the old resident or change its color. Document actual results instead of marking unperformed probes green.

### Task 6: Final verification, documentation and review

**Files:** Update `ISLAND_DEMO.md`, relevant architecture notes, asset generation instructions and `gbScripts/check-island-resident-runtime.ps1`.

- [x] Run `./gbScripts/check-island-resident.ps1`, `./gbScripts/check-island-terrain.ps1` and the standard Windows `build-gbDemo.ps1 -Configuration debug-client -Platform x64` on final code. Inspect return codes and shader diagnostics.
- [x] Run multiframe Vulkan smoke checks and an extended autonomous run spanning multiple stops/routes. Assert changing bone poses and safe shoreline positions, capture close idle/walk/color views, and exercise controls and teardown.
- [x] Inspect source/resource membership, new XML and `git diff --check`; preserve pre-existing changes. Keep known shutdown-only command-pool validation messages distinct from new rendering errors.
- [x] Document Blender regeneration, model/clip paths, `C`, shared color setting, behavior limits, diagnostics and every unverified backend. Record evidence in ignored check output; only claim checks actually performed.
- [x] Obtain one independent whole-change review, address concrete findings and repeat affected checks. Keep native execution as used for the ocean task. Do not create per-task delegation or silently stage/commit earlier ocean work.

## Self-review and execution handoff

Model, rig, clips, shared recoloring, navigation, autonomous behavior, input,
ocean pass compatibility and lifecycle each have an owning task and verification.
The plan uses existing fabricator/mixer/custom uniform APIs; PT4B's missing normals
and terrain smoothing are explicitly handled. New proposed interfaces are named
above and belong to demo code, not assumed engine APIs.

Execution completed natively after user approval, followed by one independent
whole-change review and verification of its asset-preflight finding.

## Execution evidence (2026-10-09)

- Blender generation/asset check: 2,012 triangles, 17 authored bones, packed authoring images, 1.9 m height. Runtime has 18 bones including the armature parent.
- CPU navigation/behavior smoke: 3,000 simulated seconds, 144 stops; safe paths, deterministic state, bounded motion/turns and seeds 1337/42/2026 at 512/256 resolutions. A focused corner test failed before fixing the per-frame turn budget.
- FBX probe: failed with 0.01 animation/bind scale ratio before `FBX_SCALE_ALL`; all bone ratios now1 under the two actual importer conversion modes. Runtime preflight verifies missing/corrupt data and clip ranges before fabricator assertions.
- Windows debug-client x64 build passed, with C++ unwind enabled only on the four demo files that throw/catch resident setup errors. No feeder, render-system or core animation changes.
- Vulkan fixed idle/walk/color captures passed; real C messages (including duplicate keydown) cycle four colors and preserve boot/skin/hair pixels. Shirt/trouser pixels agree in idle and walk.
- 80 s autonomous run: three walk phases/two idle stops, all reported positions safe. Close back capture and orbit/RMB pan/wheel flows exercised; release/cancel and upper wheel bound screenshots stayed fixed.
- Temporary lifecycle probe detached and destroyed the original resident (weak reference expired), recreated a fresh resident using the existing navigation system, then verified one C cycle. Probe source was restored before final build.
- Corrupt staged FBX probe captured ocean6/80/160 and logged the expected resident-only setup failure. The generic ocean smoke tool labels any 'failed' log as an error; the resident fallback itself remained alive and drew the ocean. Staged asset restored in finally.
- Independent reviewer finding (invalid FBX could reach asserting loaders) fixed and reviewed again; no remaining important finding.
- XML/project parsing and git diff --check passed. Existing unrelated changes preserved; no staging or commits.
- Metal/Xcode, OpenGL and server builds, full transition recreation and numerical per-vertex GPU readback remain UNKNOWN/unperformed. Pre-existing shutdown-only pending Vulkan command-pool validation remains.
