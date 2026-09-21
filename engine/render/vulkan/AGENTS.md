# Vulkan Backend Instructions

Read `engine/render/AGENTS.md` first. Vulkan is the source-selected Windows backend, but successful build/runtime behavior must still be verified for each change.

## Where code belongs

- Context/frame lifecycle: `gbCore/sources/graphics_context_win32.cpp`.
- Instance/device/queues: `gbCore/sources/vk_device.*`.
- Surface/swapchain/presentation: `vk_surface.*`, `vk_swap_chain.*`.
- Buffers and uniform resources: `vk_buffer.*`, `vk_uniform_value_buffer.*`, `vk_uniform_sampler_buffer.*`.
- Render-pass/framebuffer behavior: `vk_render_pass_descriptor.*` behind `render_pass_descriptor.*`.
- Shader conversion/module creation: Vulkan branch of `shader_compiler_glsl.cpp`.
- Pipeline/descriptors/material binding: Vulkan branches of `material.*` and common resource classes.

Keep Vulkan-native types and calls in existing Vulkan classes or guarded backend branches. Gameplay, ECS, and scene code should not include Vulkan headers.

## Canonical flow and rules

1. `graphics_context_win32` creates instance, debug callback, surface, device, swapchain, command buffers, framebuffers, fences, and semaphores.
2. `make_current()` waits/acquires frame state.
3. `render_pipeline::on_draw_begin()` resets/begins command recording.
4. Vulkan render-pass descriptors begin/end native passes.
5. Materials construct/bind Vulkan pipelines and descriptor resources for the active vertex-input/pass state.
6. `on_draw_end()` ends recording; context draw submits and presents.

For a configured pass, start with XML/material/shader changes described in the shared guide. Modify `vk_render_pass_descriptor` only when existing attachment/load/store/layout behavior cannot represent the configuration.

Vulkan shaders originate from the paired GLSL sources and are converted by shaderc. Follow the existing binding/layout conventions inserted or converted by `shader_compiler_glsl.cpp`; do not add a conflicting standalone SPIR-V registration path.

## What not to touch

For an ordinary pass or material, do not change:

- instance/device singleton initialization order
- queue-family or physical-device selection
- frame fences, semaphores, image-in-flight tracking, or command-buffer ownership
- swapchain acquire/present order
- resource destruction order

No complete swapchain resize/recreation strategy was discovered. Treat resize, device loss, or swapchain lifetime work as a dedicated investigation, not an incidental outline/material change.

## Build and test

1. Build the current Windows path:

   ```powershell
   ./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
   ```

2. Treat shaderc warnings/errors as failures relevant to shader changes.
3. Run the affected transition with Vulkan validation enabled when the available build/environment supports it.
4. Check pipeline creation, descriptor bindings, attachment layouts, render-pass compatibility, command-buffer state, queue submission, and presentation messages.
5. Exercise multiple frames and resource teardown, not only first-frame creation.
6. If shared render code changed, verify or explicitly report Metal/OpenGL as untested.

