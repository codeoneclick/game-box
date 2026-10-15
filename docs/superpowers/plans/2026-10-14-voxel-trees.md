# Voxel tree replacement plan

Goal: replace four tree models with Minecraft-style Pine/Oak/Birch/Palm/Thuja, without concept platforms.

- [x] Trace existing generator, editor species IDs, save validation, batching, wind and native UI icons.
- [x] Implement shared occupancy-grid generator with internal-face culling and greedy quad meshing, five distinct silhouettes and ground-aligned roots.
- [x] Integrate seeded forest and edited trees; preserve IDs0–3, append Thuja4, keep above-water distribution and wind/outline metadata.
- [x] Add five native menu entries/voxel icons and stable bark/leaf pixel detail with distance fade.
- [x] Verify CPU mesh invariants, Thuja undo/save/load, Debug/Release, all five native models, editor placement, wind/teardown and whitespace.

