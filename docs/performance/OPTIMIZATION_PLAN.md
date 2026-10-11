# Quality-preserving core optimization

Scope approved by the user: fix audited bottlenecks and explicitly inspect ocean/terrain, preserving image quality, final Woodsman mesh and 200 autonomous residents constrained to their terraces.

1. Keep the audit baseline and capture the staged pre-change Release image with fixed ocean time.
2. Remove diagnostic validation from the default Release path, preserve Debug validation and an explicit runtime override.
3. Batch Vulkan descriptor writes, cache device alignment, replace fixed uniform/pool ceilings with reusable chunks.
4. Remove HUD synchronous uploads using submitted-data snapshots and mapped buffers per fenced swapchain image. Preserve multiple updates within one recorded frame.
5. Bucket render entities per technique every frame, borrow internal containers, cache exact normal matrices, reuse target quad and driver pipeline cache.
6. Optimize ECS, animation, navigation and job/resource lifecycle without skipping poses, changing phase order or reducing population.
7. Inspect ocean/terrain ownership and update frequency; eliminate only unused calculations and redundant static work. Preserve sample counts, fields, waves, mesh bytes and target formats.
8. Build Debug and Release, run focused CPU checks and demo smoke, capture quality images, repeat performance measurements.
9. Review changes and record implemented, partially addressed, deferred and unverified audit items honestly.

No resolution/sample/mesh/animation quality reductions are accepted. Larger redesigns (instanced skinning, attachment aliasing, asynchronous texture retirement and Apple synchronization) require separate evidence; they are not considered fixed by unrelated optimizations.
