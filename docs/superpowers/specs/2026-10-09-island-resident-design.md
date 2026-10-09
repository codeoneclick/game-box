# Male island resident

Status: approved and implemented; Windows Vulkan verified. Metal/OpenGL execution and full transition recreation remain UNKNOWN.

## Confirmed requirements

- A male low-poly character inspired by the supplied `Untitled1.png` reference.
- The character lives in the Game-Box island demo, launched through the existing F5 flow.
- At least looping idle and walk animations.
- Autonomous walks with idle stops.
- One clothing color changes the shirt and trousers together. Boots, skin and hair retain their own fixed colors.

## First model

Create an original model in Blender: an adult man with elongated limbs, a narrow
torso, a simple angular face, short brown hair, a shirt, trousers and brown boots.
Use flat shading, simple color regions and approximately 1,000–3,000 triangles.
Target height is 1.9 metres; scale is verified after import into the island scene.
No facial animation or fingers are required for this first resident.

Use a small humanoid armature, comfortably below the existing shader's 64-bone
limit, with at most four bone influences per vertex. Keep editable source and a
reproducible Blender generation/export script. Export the mesh and idle/walk
sequences as FBX with matching bone names, hierarchy and bind pose.

Idle has subtle breathing and weight shifts. Walk has alternating footfalls,
knee flexion and opposing arm swings. Both are authored in place: the demo
controls world translation, avoiding double movement from animated root motion.
Fit walk cadence to movement speed, initially about 1.2 m/s. Use the existing
animation mixer's transitions, subject to verifying their behavior on this model.

## Clothing customization

Expose one resident clothing-color setting, initially muted blue. The shirt and
trousers use the same selected hue; lighting/shading may vary their visible
brightness. Changing this setting updates both in the same frame.

Use a UV palette and clothing mask rather than repurposing packed skin weights
as vertex colors. Skin, hair, belt and boots are excluded from the mask. The
resident material receives one clothing color through existing custom uniforms.
Include a lightweight demo control to cycle a small palette so simultaneous
recoloring can be verified at runtime; do not add a character editor or wardrobe.

## Autonomous movement

First-version scope: one resident walking on the connected lowest dry terrace
at Y=0.5 m, staying clear of the water and terrace edges. This deliberately avoids
crossing the island's two-metre vertical walls with an ordinary walk animation.
Upper-terrace traversal requires a later decision about paths, ramps or stairs.

Build a small navigation representation from the rendered terrain tops. Validate
walkable locations and segments against the actual mesh, accounting for contour
smoothing. A navigation cell must support the character footprint with an edge
margin; signed logical cells alone are not proof that a location is walkable.
Choose reachable destinations, route around the inner terrace and follow the
route at a bounded speed. Do not move along an unchecked straight line through
water or cliff walls.

Behavior cycles between idle (roughly 2–5 seconds), choosing a reachable
destination, and walking. Face the movement direction with a bounded turn rate.
Use idle if no safe route exists. Limit large-frame movement so a stall cannot
skip navigation boundaries. Keep feet at the ground surface; foot IK, jumping,
swimming, physics collisions and multi-resident crowd avoidance are out of scope.

## Integration and ownership

- Model source/export tooling: a dedicated character asset directory/script.
- Canonical runtime assets: `gbBundle`, not the staged Windows resource folder.
- Resident state and autonomous behavior: `gbDemo` components/system and scene setup.
- Existing skeleton, animation mixer, animation system and configured fabricator: reuse established APIs.
- Run movement and animation selection in the resident system's `on_feed_start`, before animation evaluation in `on_feed`. Copy evaluated bones during its `on_feed` at order 6, after animation order 3 and before rendering order 7. Do not renumber the core systems.
- Keep the animated resident in the existing opaque scene pass. Its shader must write scene color, encoded normals and world position/linear view depth in the ocean pass's expected layout.
- Preserve camera orbit, RMB pan, wheel zoom and existing ocean receivers/props.
- Register all new source/resources in applicable MSBuild and Xcode targets.
- Scene teardown releases resident entities, behavior data and any demo-control listeners.

Verified source entry points: `scene_fabricator::add_animation_sequences_3d`,
`shape_3d::play_animation`, `ces_animation_3d_mixer_component`,
`animation_3d_sequence_serializer_fbx`, `mesh_3d_serializer_fbx`,
`ces_shader_uniforms_component` and `island_demo_scene`.
The old animated-shape shader is a skinning reference, not directly compatible
with the ocean demo's three-attachment scene pass.

## Approach choice

Recommended: an original Blender model and authored skeletal clips, integrated
through the existing FBX path. This preserves an editable source asset and the
reference's visual style. A third-party rigged asset is quicker to acquire but
changes the appearance and introduces asset/license selection. Runtime-generated
rigid body parts are suitable for a placeholder but are a weaker basis for this
requested modeled character.

## Acceptance and verification

1. Inspect model renders from front, side and back for silhouette, clothing and male appearance.
2. Verify exported mesh scale, UVs, normalized weights, bone limits and matching clip skeletons.
3. Exercise idle and walk loops in Game-Box without exploding skinning or visible loop jumps.
4. Change the shared clothing color while animated: shirt and trousers both change; boots, skin and hair do not.
5. Run autonomous movement across multiple routes and idle stops; assert all positions/segments remain on the permitted dry terrace.
6. Check large dt, unavailable destinations and resident removal/recreation.
7. Build Windows `debug-client|x64`, capture the Vulkan runtime and recheck camera controls and ocean composition.
8. Run `git diff --check` and inspect new project membership. Report Apple/Metal, dormant OpenGL and server targets as unverified unless separately exercised.

No new engine-wide navigation framework, editor UI, terrain modification or
replacement animation subsystem is part of this design. Rendering/backend
changes are limited to the verified requirements of the animated resident.
