# Directional sun shadows

The island now uses actual light-space shadow maps. This supersedes the earlier
screen-space cast-shadow experiment and its short-contact fallback.

Two 2048x2048 orthographic maps cover a 96m square around the orbit target and an
800m square covering the island. Light-space texel snapping avoids camera-driven
swimming. The resolver blends the near map into the far map between 72% and 94%
of its half-width. Both maps use linear RGBA8 UNORM (`pixel_format=70`) with RGB24
packed depth. An sRGB attachment is unsuitable for this encoding.

The screen resolver uses the G-buffer's world position and normal, not a
screen-space occluder search. A 4x4 tent PCF kernel filters depth comparisons;
each tap compares against the receiver's tangent plane to avoid striped
self-shadowing at low sun. Small normal/depth bias keeps contact points attached.
Cool ambient fill remains visible. Direction and length follow the existing
0-24h solar control; shadows fade near the horizon and switch off at night.

## Ownership and lifecycle

`ces_sun_shadow_system` is demo-owned and queries renderable ECS entities. It
recognizes the terrain/tree/prop, resident and building materials, shares their
meshes, and creates light-space caster proxies. These use `e_none` camera bounds;
CPU bounds checks instead cull against each light volume. Offscreen sources can
therefore cast onto visible ground. Grass does not cast solid-card shadows.

Resident casters borrow the current 64-bone palette; no extra animation mixer is
run. Vulkan/OpenGL arrays use the same explicit material-array upload adapter as
visible residents, since the engine bulk setter skips arrays. Bones and caster
parameters occupy one 4176-byte vertex block, compatible
with the existing Metal material buffer contract. Rigid/tree casters use an
80-byte block and the same time, root, phase and wind equations as visible trees.
Building alpha cutouts sample the current source atlas; placement ghosts do not
cast. Rotations and terrain/editor mesh replacements are followed automatically.
Replaced or detached proxy meshes use the existing 12-frame retirement policy.

The system runs at order 6. Resolve matrices are published during its feed, before
the render feed copies them at order 7; the same stored matrices are used for
casters in the end phase after all simulation feeds. Resolve uniforms are
initialized during scene setup, before the first render feed. The new HDR resolve pass sits after ocean composition and before
tone mapping; it preserves alpha because alpha carries existing outline
coverage, not ordinary opacity. Existing SSAO and blob shadows remain.

The reusable `game_transition::get_ws_technique_size()` hook defaults to viewport
size, preserving existing transitions. The island overrides it only for the two
sun passes. On Metal, a WS target with dimensions different from the viewport
receives a matching private depth/stencil attachment; ordinary viewport targets
continue using their shared attachment. GPU work stays on the render/game loop.

## Controls and diagnostics

- `GB_SUN_SHADOWS=0`: disable sun-map shadows for comparison.
- `GB_SUN_SHADOW_DEBUG=1`: shadow visibility mask.
- `GB_SUN_SHADOW_DEBUG=2` / `3`: decoded near/far depth map.
- The former `GB_SUN_CONTACT_SHADOWS` ray-tracing experiment is no longer enabled.

## Verification and limitations

CPU checks: `./gbScripts/check-sun-shadows.ps1` covers projected ray direction,
ground projection, subtexel camera stability, vertical/horizon light and bounds.
It was run failing before the new math header, then passing. Asset checks in
`check_stylized_post_assets.py` cover linear depth formats, sizes, bindings,
pass dependencies and GLSL/Metal shader pairs. Windows Debug and Release build.

Native Vulkan runs cover sunset overview/close zoom, day, night, terrain edits,
orbit/zoom, on/off comparison and clean shutdown with validation enabled.
Logs/screenshots are under `gbDemo/output/island-check/sun-map-*`; the corrected
close view is `sun-map-final-pose/720.png`. Read-only code review identified and
led to fixes for sRGB depth, Metal uniform-block ownership and Metal depth size, same-frame resolve timing and explicit bone-array uploads.

The maps add geometry work: in the sampled overview, submitted triangles rise
from about 1.08M to 1.81M and draws from 408 to 838. Sampled Release rates were
roughly 19-33 FPS enabled and 35-45 disabled during these smoke runs. They are not
a controlled benchmark; machine load and validation affect results. This is a
material performance cost, not a claim of free shadows. No shadow geometry LOD
or reduced map resolution is used. The far map is less detailed than the near
map, and filtering/bias trade-offs remain at extreme grazing angles.

Apple Metal code and build metadata are updated but cannot be compiled or
exercised on this Windows machine. OpenGL remains unverified. Shadows on water
are suppressed by water coverage; this is not a volumetric shadow solution.

Final verification after the timing/palette fixes repeated close animated poses,
terrain editing with RMB pan/MMB orbit/wheel zoom, and day/night captures with
clean Vulkan validation and exit 0. The final close sample displays about 37 FPS;
it is a different view from the overview comparison above. No Apple runtime or
OpenGL validation is inferred from those Windows results.
