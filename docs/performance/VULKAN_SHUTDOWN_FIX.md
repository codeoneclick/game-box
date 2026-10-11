# Vulkan shutdown lifetime fix

2026-10-11. The user reported `0xC0000005` in `texture::~texture()` at
`vkDestroyImageView` after closing the application.

## Cause and reproduction

The shutdown path destroyed the native device while the static
`ces_font_component::m_font_atlases` still strongly owned uploaded
textures. Its static destruction subsequently called Vulkan using the already
destroyed logical device. The `vk_device` singleton likewise destroyed its
command pool after the context had destroyed the device. The old debug callback
was removed before device destruction, hiding leaked-device-child diagnostics.
The current context was itself retained in a global strong pointer, deferring
its cleanup until cross-translation-unit static destruction.
After moving teardown before static destruction, validation exposed a further
retained function-local fullscreen quad: two buffers and two allocations were
still live at device destruction. All primitive mesh caches now participate in
explicit cleanup as well.

`gbScripts/check-vulkan-shutdown.ps1 -Configuration debug` reproduced the failure
before the fix: normal close of the `game-box` window returned **-1073741819
(0xC0000005)**. The script retains the native process handle, closes only the game
window, and requires both exit code 0 and no validation errors. It never treats
forced termination as a successful shutdown. Closing every process-owned window
is unsuitable because it also closes the console and can produce `0xC000013A`.

## Change and invariant

After scene/resources have been released and submitted GPU work is idle, the
Windows Vulkan context clears cached material GPU references, font atlases and
shared primitive meshes
while the device is still valid. Font CPU records and atlas storage are also
freed; no live font components may use this cache-clear operation.
The Vulkan controller explicitly releases its matching global current-context
registration and its own context after releasing scene/resource ownership.
This executes teardown before returning from normal application cleanup, while
the caches and device singleton are still alive. An unrelated current context
registration is not cleared.

Vulkan classes explicitly release their owned objects before the device:
pipelines/cache, command pool/buffers, framebuffers, depth view/image/memory,
fences/semaphores, swapchain layout/pass/views/swapchain. Device destruction sets
its handle to null; repeated destruction from the static singleton is a no-op.
The validation callback remains active through device and surface destruction.
`texture::~texture()` continues freeing its resources normally: the fix changes
owner lifetime rather than skipping destruction against an invalid device.

No shader, geometry, camera or image-quality setting changed. Cache-clear
helpers are shared declarations, but only Windows Vulkan calls them here. This
does not establish general multi-context/device-recreation support.

## Verification

- Full final Windows `debug-client|x64` and `release-client|x64` demo builds
  passed, all six engine/demo modules.
- Final `./gbScripts/check-vulkan-shutdown.ps1 -Configuration debug -Runs 3`
  passed after explicit-controller and primitive-cache cleanup: three initialized
  island scenes, three normal exits with code 0 and no Vulkan validation errors,
  including teardown. Validation defaults on in this check and the log must
  confirm the requested state.
- `./gbScripts/check-vulkan-shutdown.ps1 -Configuration release -Runs 3` passed
  with validation on. An additional Release run with `-Validation 0` passed.
  Every run rendered the initialized island and exited normally with code 0.
- `git -c core.safecrlf=false diff --check` passed.
- Final Release fixed-time capture `quality-shutdown-0/360.ppm` matches the
  pre-shutdown-fix `quality-final-0/360.ppm` exactly outside the dynamic HUD
  rectangle `(900,0)-(1280,120)` at 1280x720, ocean time 3 and zero residents.
- OpenGL/Metal and Apple/server targets are not verified. No critical/important
  findings remained in the final ownership review.

Local logs: `gbDemo/output/island-check/shutdown-debug/` and
`shutdown-release/`; baseline logs are preserved as `shutdown-debug/before.*`.
