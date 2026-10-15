# Island terrain editor

The island demo now includes an editor built with the engine's existing
`ui_fabricator`, buttons, panels, sliders and labels. Start it with the existing
VS Code F5 configuration or Release task.

## Architecture and files

```text
TerrainEditorUI / TerrainEditorInput
  -> TerrainEditorState
  -> TerrainEditCommand (cell and tree deltas)
  -> TerrainEditorLevel (terrain, grass, trees, climate)
  -> TerrainEditorRenderCache (affected chunks)
  -> scene uploads / renderer
```

The UI has no dependency on terrain generation or mesh building. The generator
seeds a level; the editor's logical data then owns subsequent changes. Mesh
creation is CPU work in the demo; GPU uploads stay on the game-loop path.

Feature sources and resources:

| Files | Responsibility |
|---|---|
| `gbDemo/sources/terrain_editor_state.h/.cpp` | Tools, logical data, delta commands, history, dirty areas and binary serialization |
| `gbDemo/sources/terrain_editor_ui.h/.cpp` | Existing native engine UI, tool table and contextual controls |
| `gbDemo/sources/terrain_editor_input.h/.cpp` | Picking, strokes, brush spacing/rate and shortcuts |
| `gbDemo/sources/terrain_editor_mesh.h/.cpp` | Local terrain, grass and four procedural tree types |
| `gbDemo/sources/terrain_editor_render_cache.h/.cpp` | Chunk ownership, local rebuild and grouped render batches |
| `gbCore/sources/file_dialog.h/.cpp` | Native desktop file dialogs and unsaved-change confirmation |
| `gbDemo/sources/island_demo_scene.*` | Wiring, resource uploads, brush, water masks, shadows and navigation |
| `gbDemo/sources/ces_ocean_system.*` | Updated mask geometry and bounded retirement of replaced meshes |
| `gbDemo/sources/resident_navigation.*`, `ces_island_resident_system.*` | Navigation from mesh parts and refresh after terrain changes |
| `gbDemo/sources/terrain_data.h` | Optional palette coordinates in CPU vertices |
| `gbCore/sources/input_context_win32.cpp` | Hover motion and Alt/system-key delivery |
| `gbUI/sources/slider.cpp` | Pointer offset normalized by transformed width, including parent scale |
| `gbBundle/Shaders/shader_editor_brush.*` | Projected world-space brush circle |
| `gbBundle/Shaders/shader_island_terrain.*` | Level/climate lookup for editor terrain |
| `gbBundle/Shaders/metal/mtl_library.metal` | Corresponding terrain/brush Metal source |
| `gbBundle/Configurations/game_objects/editor_brush.xml`, `materials/material.editor_brush.xml` | Brush material using the existing foam composition |
| `gbBundle/Configurations/game_objects/materials/material.island_terrain.xml` | Palette sampler |
| `gbBundle/Textures/editor_climate_palette.png`, `gbScripts/generate-editor-palette.py` | Deterministic 24-level by 7-climate lookup |
| `gbBundle/Fonts/Vera.ttf` | Readable UI font through the existing font system |
| `gbBundle/Shaders/shader_editor_ui.*`, `shader_editor_icon.*`, `metal/mtl_editor_ui.metal` | Editor-only rounded controls and icon rendering |
| `gbBundle/Textures/editor_ui_icons.png`, `gbScripts/generate-editor-icons.py` | Shared reproducible icon atlas |
| `gbCore/gbCore.vcxproj`, `gbDemo/gbDemo.vcxproj`, both Xcode projects | Source/asset registration |
| `gbScripts/check-terrain-editor.ps1`, `check_terrain_editor.cpp`, `check_editor_mesh.cpp` | Focused CPU checks |
| `gbScripts/profile_editor_mesh.cpp` | Optimized local rebuild benchmark (`-Profile`) |

Earlier ocean/grass/shadow changes in the dirty workspace are separate from this
feature. Generated Windows staging assets are not edited as source.

## UI and tools

`terrain_tool_ui_config()` is the single Enabled/Disabled/Hidden table. Tool
switching finishes the current stroke, selects the tool and refreshes visibility,
input availability, title, selections and readouts.

| Tool | Brush | Strength | Level | Palette | Trees panel |
|---|---|---|---|---|---|
| Terrain | Enabled | Disabled | Target -5 to +16 | Hidden | Hidden |
| Grass | Enabled | Density | Disabled | Green / Dry / Snow | Hidden |
| Trees | Hidden | Hidden | Hidden | Hidden | Pine / Oak / Birch / Palm |

Terrain raises or lowers to the selected target. The inner half of the brush
reaches that target; the outer half uses smoothstep falloff and rounds to integer
terraces. Heights remain flat and the filtered contour field smooths boundaries
in XZ. Repeating a stamp at the same position preserves its feathered edge; moving
the brush closer can bring a previously feathered cell to the target.
Foundation levels -6, -7 and -8 remain immutable. Cells outside the logical world
or with the empty label are ignored. The circular brush diameter is 1–32 metres.
Terrain uses a fixed stamping rate independent of grass density. Each stroke stores a single before/after
delta per changed cell, including cells reached again by a moving brush.

The editor uses dark translucent rounded panels, cyan selection borders, colored
icon cards and rounded sliders. Its materials are scoped to this
editor; other engine UI controls retain their existing materials. Vera sans-serif
is used for controls and the performance HUD, with larger headings and warm white
text. The exact font family in the reference raster is not established.

Grass stores per-cell density and one of three types; Shift removes it. Generated
blades share the existing wind and character interaction shader. Trees are
logical instances with stable ID, species, position, rotation and scale; clicking
places one, Shift removes nearby trees. Sculpting moves their ground heights.

Climate painting, its toolbar button and palette, and the Brush Preview have
been removed. Existing biome maps remain readable in saved levels; legacy
Climate tool selections fall back to Terrain. Terrain vertices carry palette coordinates;
the shader samples a 24-level, 7-climate lookup rather than rebuilding final RGB
colors into geometry. Eight underwater and sixteen above-water levels are
supported. Level 0 sits below the minimum possible wave height, including an
8 cm clearance: with the current ocean its height is `water_y - .81466 m`.
Configurations without room between that surface and terrace -1 are rejected.

## Controls

- LMB: paint/sculpt or place a tree. Shift+LMB: erase grass/trees.
- MMB or Alt+LMB: orbit. RMB: grab and pan. Wheel: existing spring zoom.
- Ctrl+Z / Ctrl+Y: Undo / Redo.
- Ctrl+S / Ctrl+O or toolbar buttons: Save / Load.

UI panels consume pointer space for editing and camera input. Undo/Redo stores
one command per stroke with changed cells and tree instances, not full level
snapshots. New edits clear Redo. Dirty state tracks the saved history position.

## Save, Load and binary format

The scene callbacks finish an active stroke and call `gb::file_dialog`.
Windows uses `GetSaveFileNameW` / `GetOpenFileNameW`, a `.level` filter/default
extension, UTF-8 paths and overwrite confirmation. Load asks before discarding
unsaved changes. Cancel leaves the state intact.

GBLV version 2 is explicitly little-endian:

1. Four-byte `GBLV` magic and 32-bit version.
2. Terrain configuration, signed level map, grass density/type and climate maps.
3. Stable tree IDs, types, positions, rotation, scale and next ID.
4. Logical chunk reconstruction policies and current editor selections/settings,
   including the selected target terrace.
5. Trailing CRC32 over every preceding byte; map counts bound the payload.

No meshes, vertex/index buffers, textures or other GPU resources are serialized.
Version 1 remains readable: its Dig/Raise selection becomes Terrain and its other
tool selections are remapped. Unsupported versions and invalid targets are rejected.
Reconstruction policies preserve the original smooth seed contours and grass
distribution for untouched chunks, and local edited topology for remeshed chunks.
They stay consistent across Undo/Redo and file roundtrips.

Save writes a sibling temporary file and atomically replaces the destination
after a successful write. Failed saves preserve the previous file and dirty state.
Load validates magic/version/CRC, sizes, finite values, ranges, IDs and policies
into temporary data before replacing the current state. Successful Load clears
history, restores editor settings and invalidates scene caches; failed Load keeps
the current level. Water dimensions/height and resident terrace heights follow
the loaded configuration.

## Rebuild and limitations

Local geometry uses 16-cell chunks with a three-cell rebuild halo; render batches group
4x4 chunks and split before the 16-bit index limit. Vertical skirts close seams
between edited chunks and preserved seed contours. Climate/grass-only edits
retain the current terrain geometry. Water interaction masks and tree/person
blob shadows follow rebuilt geometry and logical tree instances. Resident
navigation refreshes after a sculpt stroke; residents keep their assigned terrace.
Contour bands are extracted from a cached 5x5 binomial-filtered level field,
removing the grid's axis/diagonal zigzag before meshing. The filter changes only
contour positions; each band's height remains the exact terrace height. Empty
cells remain empty, and global corner samples match across adjacent chunks.
Rebuilt top vertices at unchanged heights retain the seed surface's material
gradient by sampling its original triangles in affected chunks. Newly exposed
surfaces use the same green-to-brown and brown-to-gray terrace transitions;
wall endpoints use the outer color above and inner color below. This prevents
neighboring surfaces from changing color merely because their chunk was rebuilt.

Edits use an isolated logical snapshot and a CPU worker that exclusively owns the
render cache. Disjoint dirty regions stay separate, repeated updates to a render
batch are coalesced, and only affected chunks are rebuilt. Vertex packing also
runs on that worker. Navigation is deferred until a sculpt stroke ends and uses
up to four CPU workers. Generation/revision checks discard stale load/navigation
results. GPU creation and entity changes stay on the game loop, using existing
fence-safe dynamic buffers and at most one changed batch per frame.

Native dialogs are implemented for Windows. Other platforms return an explicit
unsupported message; OpenGL/Metal runtime behavior has not been verified here.
The brush uses scene world positions, so it may project across opaque props too.
The world still has the demo camera/fog/interaction-field spatial limits. Large
brushes and full loads may take time to appear while background work completes;
GPU batch installation is spread over frames. Trees remain
decorative and are not pathfinding obstacles. Undo history is session-only.

## Verification

Run `./gbScripts/check-terrain-editor.ps1` for state/serialization and local
mesh checks. Build with `./gbScripts/build-gbDemo.ps1 -Configuration debug-client
-Platform x64` or `release-client`. These are compile checks and focused demo
smoke tests, not an engine-owned unit-test suite.

Verified Windows results:

- Debug and Release builds: successful, all six directly built modules.
- CPU checks: target limits, raise/lower falloff, repeated and moving stamps,
  protected foundation, history, version-1 migration, invalid version-2 targets,
  atomic/corrupt saves,
  resized loads, holes, seams, deterministic foliage and exact terrain/grass/
  climate render-buffer equality after file roundtrip. Extreme smoothed chunk:
  diagonal contour normals and exact shared chunk boundaries. The alternating
  extreme-level fixture uses 325 vertices after filtering, within the 16-bit limit.
- Vulkan native interaction: Terrain, scaled slider ranges, Undo/Redo, Save/Open dialogs, Load, grass paint,
  tree placement/Shift erase and climate paint with 200 residents; validation log
  empty, normal shutdown exit 0.
- Profiling before the worker split (Debug): mesh 154 ms, uploads 58 ms,
  navigation 1,121 ms on a sample edit.
- Release long-drag sample after the combined brush and UI update, without
  validation, 200 residents: 130 active samples, median frame 6.95 ms,
  p95 9.07 ms, maximum 20.93 ms. These are measurements on this machine,
  not an FPS guarantee for other machines.

Use `./gbScripts/check-terrain-editor.ps1 -Profile` for the CPU rebuild benchmark.
`GB_EDITOR_PROFILE=1` logs active editing frame times, worker phases and main-loop
installation costs in the running demo. OpenGL and Metal remain source-updated
but unverified at runtime.

## Buildings

The fourth tool uses the native UI and actual textured previews of Forester,
Stonecutter and Woodcutter models. LMB places one building; Shift+LMB removes
a building under the cursor. R rotates the preview by 90 degrees around its
own grounded base-center pivot. The Buildings panel also provides a 0–360 degree
slider, numeric input (including decimals), step buttons and Off/15/45/90 degree
Snap choices. Q/E rotates by one degree with Snap Off, or by the selected snap
step. All these controls rotate the placement preview; the placed instance
retains that angle. Preview changes do not dirty the level or enter its history.
The logical yaw is continuous radians, so
future arbitrary-angle rotation does not require a save-format change.

Validation covers the whole model kit plus a 0.5 m margin, using the installed
terrain's actual top triangles. A green preview means the complete rotated
rectangle fits one above-water terrace without overlapping another building;
red means placement is rejected. Pending terrain updates also block placement.
Buildings clear grass and trees in their footprint, protect their foundations
from sculpting, and block resident paths. Placement/removal participate in
Undo/Redo. GBLV v3 stores buildings and reads older v1/v2 files.

Verification: engine-compatible FBX/UV export probes, building support/history/
serialization CPU checks, resident obstacle checks, full Windows Release build,
isolated Debug demo build, and Vulkan placement/rotation/Undo/Redo smoke.
The rotation UI smoke additionally checks numeric 37/37.5 degrees, slider drag,
snap selection, Q/E, safe empty backspace and switching away from Buildings.
The scene registers the existing `gb::ui::ces_textedit_system` for numeric input
and removes its input listener on teardown. Local callbacks keep keyboard focus
and safe editing independent of the legacy write-only focus property. A local
adapter sends keyboard events only while the field captures input, preventing
unfocused keystrokes from replaying later, and keeps focus during pointer motion.
The new native Save dialog automation did not complete (Windows displayed
"Location is not available"); building file roundtrips were verified by CPU
checks. OpenGL and Metal building paths remain unverified at runtime.
