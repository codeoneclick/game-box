# Voxel terrain plan

Goal: reproduce the supplied seven terrain materials without changing terrace shape.

- [x] Trace logical terrace IDs, seed and edited mesh paths, underwater floor and material texture bindings.
- [x] Author deterministic pixel-art top/side atlas with non-overlapping borders and six linear-light LOD levels.
- [x] Sample by world coordinates and triangle terrace ID in GLSL/Metal; preserve other meshes and render effects.
- [x] Bind atlas, register Apple resources, mark seabed and pass existing terrain dimensions in unused uniform channels.
- [x] Verify Debug/Release, CPU terrain checks, shader compilation, native terrain editing and zoom, validation/teardown and whitespace.

