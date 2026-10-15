# Terrain editor implementation plan

**Goal:** Implement the supplied island editor specification using existing gbUI.
**Architecture:** UI -> TerrainEditorState -> delta commands -> logical terrain,
grass, tree and climate data -> affected mesh chunks -> renderer. UI contains no
generator/mesh-builder calls. Generator is only used for initial data seeding.
**Spec:** User attached terrain-editor request plus screenshot and architectural rule.

## Work
- [x] `terrain_editor_state.h/.cpp`: CPU-only state, panel configuration table,
brush commands, stroke history, dirty region tracking and atomic validated binary
GBLV v1 serialization. TerrainData.layers stores actual editor signed levels
(-8..16, empty -9). Initial existing nonnegative levels are mapped to +1..+8.
`editor_level_height`: +1 -> shore_height; +2..16 -> (level-1)*layer_height;
negative -> level*layer_height; 0 -> water_y minus wave bound and .08 clearance.
Grass/type/climate maps are
separate CPU data. Tree instances have stable ID, type, position, yaw and scale.
- [x] `terrain_editor_mesh.h/.cpp`: 16-cell chunks, local clipped triangle
contours with arbitrary local holes/islets; dirty-region rebuild only. Geometry
contains level/climate lookup coordinates, not final terrain RGB. Grass separate
mesh; tree geometry and instances derived from persisted TreeData.
- [x] `terrain_editor_ui.h/.cpp`: existing ui_fabricator buttons/panels/sliders,
native labels; left tools, top Undo/Redo/Save/Load, contextual right panels and
status. Tool table centralizes Enabled/Disabled/Hidden. UI calls state/commands
and passed Save/Load callbacks only. Input blocks scene edits/camera over UI.
- [x] `file_dialog.h/.cpp` in gbCore: desktop native Windows Save/Open dialogs,
UTF-8 paths, .level filter/extension, cancellation; platform abstraction only.
- [x] `terrain_editor_runtime.h/.cpp` and scene integration: camera picking,
world-space brush, stroke spacing/rate, Shift erase, mouse/keyboard shortcuts,
chunk replacement and frame-safe old-resource retirement, water masks, foliage,
blob shadows and resident navigation refresh. Existing camera RMB and spring zoom
remain; LMB sculpts in editor, Alt+LMB or editor toggle rotates the camera.
- [x] Palette texture and GLSL/Metal lookup; preview material; source resources
registered in Windows/Xcode metadata, no generated staging edits.
- [x] Focused CPU edit/undo/save-corruption/mesh checks, full Debug/Release,
Vulkan native UI/input/save/load captures and normal shutdown. Report Apple/GL
execution as unverified unless actual builds are available.

## Invariants
Foundation cells at -6,-7,-8 cannot be sculpted. Dig=max(level-1,-5),
Raise=min(level+1,16); no tool alters locked foundation. Undo stores cell/tree
deltas per stroke, not level snapshots. Load validates everything into temporary
data before replacing the current state. Persist only logical data, never GPU
buffers. Rebuild/commit GPU resources on the game-loop path. Preserve pre-existing
workspace changes, current shader effects and F5 configuration.
