# Island terrain prototype

The island is the default application demo in `gbDemo`, using the existing
Game-Box renderer. The cube demo and its scene-specific assets were removed.

## VS Code: F5

Open `C:\Hobby\game-box` as the workspace folder and press **F5** with the
`Island demo (Debug x64)` launch configuration. The pre-launch task builds all
demo dependencies and stages assets, then launches `gb_demo.exe` with `gbDemo`
as its working directory. Output appears in the integrated terminal. The
Microsoft C/C++ extension (`ms-vscode.cpptools`) supplies `cppvsdbg`.

Stop the previous debug session with **Shift+F5** before rebuilding so the
running executable and its PDB are released.

Hold the **left mouse button** and drag to orbit the camera. Horizontal motion
changes yaw; vertical motion changes elevation, clamped to 10–80 degrees.
The initial view remains at yaw 45°, elevation 55°, radius 245 m. Releasing the
button stops camera movement. There is no zoom in this version.

## Build and launch (PowerShell, repository root)

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
Push-Location gbDemo
try { & ./output/debug/x64/gb_demo.exe } finally { Pop-Location }
```

The working directory is `gbDemo`, as required by the existing Windows resource lookup.
For layer tint debugging, set `$env:GB_ISLAND_LAYER_COLORS = '1'` before launch.
Seed, layer count, vertex/triangle counts and CPU generation/build timings are
printed to stdout. No new debug UI is required.

## Architecture

`terrain_generator::generate(config)` → mutable `terrain_data::layers` →
`terrain_mesh_builder::build(data)` → CPU vertices/indices → `gb::vbo`/`gb::ibo`
→ `gb::mesh_3d` → `shape_3d` with geometry/material components → existing
`ces_render_system` and configured WS/SS/output techniques.

The scene owns the logical data. The builder consumes cell data, never seed or
the generator's shape formula. Cells are sampled at their centres in X/Z;
`-1` means ocean, `0..7` means land at `layer * layer_height` metres along Y.
GPU upload occurs during scene creation on the game-loop/main path. The
transition owns the scene and installs the existing scene fabricator. No new
component, system, physics world or renderer framework is introduced.

The generator uses seeded low-frequency angular deformations with shrinking,
slightly differing contours. The builder bilinearly samples each layer's
occupancy mask, locates radial boundaries, smooths the radii and triangulates
horizontal terrace annuli and the summit. Every boundary has a separate
vertical wall; the shoreline wall extends below water. Top normals point up;
wall normals point outward and never share top vertices. The terrain is one
mesh/entity/draw, and the water is a second mesh/entity/draw in the WS pass.

The shader uses vertex colours, directional diffuse and ambient lighting,
without textures or light entities. GLSL and matching Metal functions are
provided. The island transition uses its own WS configuration, retaining the
`ws.base` attachment names expected by the existing basic-demo postprocess.
The clear colour extends the water background beyond its finite plane.

## Parameters

Edit `terrain_config` defaults in `gbDemo/sources/terrain_data.h`, or set fields
in `island_demo_scene::create()` before generation.

| Parameter | Default | Meaning |
|---|---:|---|
| `world_size` | 256 m | Square generation domain centred at the origin |
| `resolution` | 256 | Cells along each axis; supported range 32–1024 |
| `layers_count` | 8 | Default produces all eight levels; supported range 1–8 |
| `layer_height` | 2 m | Heights 0, 2, 4, 6, 8, 10, 12, 14 m |
| `seed` | 1337 | Deterministic initial cell data for the same build/config |
| `water_y` | −0.3 m | Water surface, below the lowest terrace |
| `water_size` | 400 m | Square water plane |
| `debug_layer_colors` | false | Alternate per-layer top tints |

The coastline occupies most of the domain, leaving ocean around the island;
256×256 is the generation area, not a rectangular land footprint. The orbit
camera starts at radius 245 m, elevation 55°, yaw 45°, FOV 60°, near/far 10/600 m.
Its input listener is registered with the transition's input context and
removed when the scene is destroyed.

## Verification

```powershell
./gbScripts/check-island-terrain.ps1
git diff --check
```

The standalone CPU compile/smoke check covers reproducible and changing seeds,
all eight height levels, finite geometry, triangle winding against normals,
16-bit index limits, changed logical cell data, resolutions 32/64/256/1024,
one-layer configuration, water geometry, disconnected cells, enclosed holes
and malformed parameters/data. This
does not introduce or claim an engine-owned unit-test suite.

Windows `debug-client|x64` build and Vulkan runtime/frame capture were exercised.
The default mesh has 23,808 vertices and 7,936 triangles. Validation stderr was
empty during the island smoke run. Existing conversion warnings remain in
core headers. Metal/Xcode and dormant OpenGL builds/runs are **UNKNOWN** on this
Windows machine; source/project updates are not proof of runtime parity.

## Limits and next step

The terrain is static. Every layer must contain the origin and have a
single star-shaped, nested contour; holes, disconnected land and overhangs are
unsupported. Every layer mask is checked for disconnected land and enclosed
holes with flood fill; sampled radial re-entry is also rejected. Radial sampling can
miss changes narrower than its angular spacing. Smoothing moves the rendered
boundary slightly relative to cells. Contour segment count is capped at 512
to stay within the existing 16-bit mesh contract. No chunks, LOD, collisions,
editing, partial rebuild, vegetation or advanced water are implemented.

For future sculpting, first replace radial contour extraction with general
marching-squares contours and polygon triangulation (including holes), keeping
the `terrain_data → terrain_mesh_builder → mesh` boundary. Then add world-to-cell
selection and data edits with a full mesh rebuild. Add dirty-region rebuilds
after that path works; do not put the generation formula in the renderer.

## Source-file manifest

Created:

- `gbDemo/sources/terrain_data.h`
- `gbDemo/sources/terrain_generator.h`, `terrain_generator.cpp`
- `gbDemo/sources/terrain_mesh_builder.h`, `terrain_mesh_builder.cpp`
- `gbDemo/sources/island_demo_scene.h`, `island_demo_scene.cpp`
- `gbDemo/sources/island_demo_transition.h`, `island_demo_transition.cpp`
- `gbBundle/Configurations/game_objects/island_terrain.xml`
- `gbBundle/Configurations/game_objects/materials/material.island_terrain.xml`
- `gbBundle/Configurations/transitions/transition.island_demo.xml`
- `gbBundle/Configurations/techniques/ws_techniques/ws.technique.island_demo.xml`
- `gbBundle/Shaders/shader_island_terrain.vert`, `shader_island_terrain.frag`
- `gbScripts/check-island-terrain.ps1`, `check_island_terrain.cpp`
- `docs/superpowers/plans/2026-10-06-island-demo.md`, `ISLAND_DEMO.md`

Modified:

- `gbDemo/sources/main_game_controller.cpp`, `main_game_controller.h`: default island startup
- `.vscode/launch.json`, `.vscode/tasks.json`: F5 build and debug launch
- `gbDemo/sources/game_conroller_win32.h`, `game_conroller_osx.h`: remove unused cube-transition members
- `gbDemo/sources/game_controller_ios.mm`, `game_controller_tvos.mm`: remove cube-transition includes
- `gbDemo/gbDemo.vcxproj`, `gbDemo.vcxproj.filters`: new source/header membership
- `gbDemo/gbDemo.xcodeproj/project.pbxproj`: sources and resources in the three existing targets
- `gbBundle/Shaders/metal/mtl_library.metal`: matching terrain shader functions
- `.gitignore`: exclude local island check/capture/build artifacts

The standard build stages source assets into `gbWin32SharedResources`; that
directory already had local changes before this task and is not canonical.

Removed cube-only files: `gbDemo/sources/main_menu_scene.{h,cpp}`,
`main_menu_transition.{h,cpp}`, `gbBundle/Configurations/game_objects/demo_cube.xml`,
`demo_ground.xml`, their `materials/material.demo_cube.xml` and
`material.demo_ground.xml`, `gbBundle/Configurations/transitions/transition.basic_demo.xml`,
`gbBundle/Textures/demo_cube.png`, `demo_ground.png`, and
`gbBundle/Shaders/shader_demo_primitive.{vert,frag}`. The basic-demo SS/output
configurations remain because the island uses that postprocessing pipeline.
Architecture-map and canonical-example references now point to the island scene.
