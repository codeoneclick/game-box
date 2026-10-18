# Graphics preferences

The F8 panel's **Save** button writes `gbResources/GraphicsSettings.json`.
Preferences load when the island scene starts. There is no automatic save on exit.
F8 opens the render inspector: select a pass on the left, then use its On/Off
control, attachment selector and existing settings on the right. Each column has
independent wheel scrolling and a draggable scrollbar when content overflows.
The preview samples the live GPU texture; downstream outputs can appear one frame
later. The inspector uses a separate UI layer and stays accessible when scene UI
or final scene output is disabled. Final-off displays the inspector over black.
Disabled geometry passes clear their outputs, and disabled color post effects
copy their input instead of displaying a stale frame.
The `Triangles` label beside On/Off counts triangle submissions for that pass in
the last completed frame, including instances, batches and fullscreen copies.
Skipped or clear-only passes show zero. A disabled color effect can still show
two triangles for its bypass copy. These are submitted triangles, before GPU
clipping or occlusion, rather than a count of visible pixels or unique meshes.
The island pipeline consumes SSAO directly; it has no `ss.ao.blur` or
`ws.blob.shadow` pass.
The saved values include every F8 toggle, slider, lighting color, view mode and
the time of day and skybox rotation selected in the main toolbar. Terrain Save/Load is independent.

Select **ws.ocean** for **Reflection**, **Refraction** and **Caustics**. Each has
an independent On/Off button and a 0-3x intensity slider. They update existing
shader uniforms live, without rebuilding water or changing its wave physics.
Reflection includes the skybox and screen-space scene hits. Refraction controls
underwater distortion; switching it off preserves clear water/bottom visibility.
Caustics illuminate submerged terrain/rocks in ws.base and are visible through
the ocean. They are shader effects within existing passes, not extra buffers or
separate triangle submissions. The ws.ocean preview shows their combined result.
Save stores `enableWaterReflection`, `enableWaterRefraction`, `enableWaterCaustics`
and `waterReflectionIntensity`, `waterRefractionIntensity`, `waterCausticsIntensity`
in `postProcess`. All default to enabled / 1x. Older version 1 files remain valid.

The scene resolves the repository's `gbResources` directory by searching upward
from the executable directory and then the working directory. It captures this
path once for loading and saving, so saving updates the source file immediately
without rebuilding or relying on the staged Windows asset copy.

Startup order is platform defaults, JSON, then explicit `GB_POST_*`, `GB_SSAO*`
and lighting environment overrides. Explicit presets replace their corresponding
settings group. Missing fields retain their current values (platform defaults at
startup); unknown fields are ignored.
Invalid types, malformed JSON and unsupported versions reject the whole file,
leave defaults intact and produce a startup log message. Numeric values are
clamped to the existing renderer limits. Bloom, DOF and vignette remain disabled.

Version 1 optionally includes a top-level `renderPasses` object mapping technique
GUIDs to boolean enabled states, for example `{"ws.base": false}`. Absent GUIDs
are enabled. A missing object preserves the current overrides; an explicit `{}`
clears them. Unknown technique GUIDs are retained through load and Save so future
passes keep their preferences. The object accepts up to 256 entries, with nonempty
GUIDs of at most 128 ASCII letters, digits, periods, underscores or hyphens.
Invalid names, counts, object types or nonboolean states reject the whole load
before any live setting changes. Save validates these same limits before touching
the previous file. Existing version 1 files without `renderPasses` remain valid.

Saving writes a temporary sibling file before replacing the previous JSON.
The panel reports `Saved` or `Save failed`; failure details appear in the log.
The UI calls a scene callback and does not own file access or renderer objects.

CPU persistence checks: `./gbScripts/check-graphics-settings.ps1`.
Windows Vulkan is the verified runtime. Apple/OpenGL execution is unverified;
packaged applications without a repository `gbResources` directory would need
a separate writable preferences location.

Terrain controls in the main toolbar share this save operation:
- **Terrain LODs**, to the right of Time of day: 1?8 total levels, default 5.
- **Contour simplification**, to the right of Skybox rotation: 0?90%, default 0.
  Disabled for one LOD; its saved value is retained.

Version 1 optionally stores `"terrainLOD": {"lodCount": 5,
"contourSimplificationPercent": 0}`. Older files use these defaults. N must be
an integer; invalid or out-of-range values reject the complete load atomically.
For five levels and 50%, targets are 0 / 12.5 / 25 / 37.5 / 50% relative to the
original contour. Safety constraints can retain more points; the Inspector's
ws.base controls show actual/requested reduction and terrain-only triangle,
selection, CPU generation, last upload and GPU cache counters. At 0%, all
contours are exact. Shadow maps select independently by light-projection error;
the shoreline water mask always uses the exact contour.

The F8 Save button saves these controls as well as the existing render settings.
The top toolbar Save continues to save the authored level separately.

The **Wireframe** On/Off toggle to the right of Terrain LODs draws the actual
selected terrain triangles with native line rasterization. It updates material
state immediately, without changing LOD settings revisions, rebuilding meshes,
or uploading extra geometry. Other scene objects, shadow maps and shoreline
masks retain filled rasterization. The toggle is off by default and is saved as
`terrainLOD.wireframe` by F8 Save; missing fields in older JSON default to false.
Vulkan checks the device's fillModeNonSolid capability. Unsupported backends
show a disabled N/A button. Metal has the matching encoder mode; the legacy
OpenGL/GLES paths currently report this debug mode as unsupported.

Native Windows toggle/save/restore smoke: `./gbScripts/check-terrain-lod-demo.ps1 -Wireframe`.

Verified on Windows Vulkan: Release build, On/Off/On switching, F8 Save and a
fresh process loading Wireframe=On. Validation logs were clean. After the initial
captured frame, GPU mesh uploads stayed at 420 and no editor rebuild occurred
while toggling. Off restored the normal image (45 differing scene pixels out of
363,400 in the fixed-time comparison). Metal code is implemented but unverified;
OpenGL/GLES wireframe remains unavailable. Other saved graphics values are
preserved when adding the optional wireframe key.

The **Tris** value under Wireframe counts selected main-view terrain triangles,
excluding grass, water, shoreline masks and shadow passes. Its text updates at
most four times per second. At P=0, additional levels intentionally preserve the
same exact contour, so changing N alone can leave the triangle count unchanged.
For P>0, selection now uses each prepared variant's projected geometric error,
not fixed distance bands. F8 -> ws.base exposes nearErrorPx (default .5),
normalErrorPx (1.5) and farErrorPx (5). The budget is continuous and local to
node distance. Equal-error thresholds are ordered conservatively so intermediate
states are not skipped. Full details and measurements: terrain-sse-lod.md.

The contour cap now extends to 90% to support the requested fivefold overview
reduction. Existing 0..50 preferences remain valid. terrainLOD optionally saves
nearErrorPx, normalErrorPx, farErrorPx and debugMode. Debug modes 0..5 are off,
actual contour reduction, wireframe, projected error/budget, collapse targets,
and active contour markers. Error budgets must be ordered, finite and within
.05..12px. Old JSON uses the defaults above; F8 Save stores current values.

Run ./gbScripts/check-terrain-lod-demo.ps1 -Zoom for local zoom and fivefold
reduction checks, -Orbit for density visualization while orbiting, and
-Inspector -InspectorMode 4 (or 5) for F8 overlay/save checks.
