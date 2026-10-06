# Island demo implementation plan

Use the existing transition, scene_graph, shape_3d, PTNTC buffers and configured
ws.base / basic_demo composition. Keep engine internals unchanged. Preserve the
existing cube demo and choose the island with GB_ISLAND_DEMO=1.

1. Add CPU-only terrain_data/config, terrain_generator and terrain_mesh_builder
   under gbDemo/sources. Defaults: 256 metres, 256 cells, 8 layers, 2 metres,
   seed 1337, water -0.3 metres, water size 400 metres. Outside cells use -1.
   Extract nested radial contours from bilinearly sampled layer occupancy;
   smooth radii and triangulate terrace annuli, summit and independent walls.
   This prototype supports one star-shaped island with nested contours only.
2. Add a standalone compile/smoke check for seed reproducibility, changed seeds,
   all eight layers, flat tops, vertical walls, winding, finite vertices,
   index capacity and invalid parameters. Run it before and after implementation.
3. Add island_demo_scene/transition. Retain terrain_data in the scene. Upload
   CPU geometry to ordinary bindable VBO/IBO on scene creation on the main path.
   Use two shapes and a static elevated camera. Log seed, counts and timings;
   expose layer-colour debug mode through terrain_config.
4. Add configured material and vertex-colour directional/ambient shader pair,
   matching Metal functions, scene configuration and transition XML. Register
   new C++ and resources in Visual Studio and every matching Xcode target.
5. Run CPU checks, XML/reference checks, standard debug-client x64 demo build,
   git diff --check and an island runtime/frame smoke test. Report Metal and
   OpenGL as unverified when unavailable. Document launch, parameters, files,
   representation limits and the next data-driven sculpting step.

Review focus: reject non-finite/invalid sizes, unreasonable resolution and mesh
capacity overflow; never silently render unsupported disconnected contours;
keep top/wall vertices separate; sample only logical data in the builder.
