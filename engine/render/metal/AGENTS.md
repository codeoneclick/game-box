# Metal Backend Instructions

Read `engine/render/AGENTS.md` first. Current Apple source branches select Metal for macOS, iOS, and tvOS; the current green target matrix is `UNKNOWN` until built.

## Where code belongs

- Platform context/view integration: `gbCore/sources/graphics_context_osx.cpp`, `graphics_context_ios.cpp`, `graphics_context_tvos.cpp`.
- Device/default library/command-buffer lifecycle: `gbCore/sources/mtl_device.*`.
- Pass and encoder wrappers: `mtl_render_pass_descriptor.*`, `mtl_render_encoder.*`.
- Pipeline/depth/vertex state: `mtl_render_pipeline_state.*`, `mtl_depth_stencil_state.*`, `mtl_vertex_descriptor.*`.
- Native resources: `mtl_buffer.*`, `mtl_texture.*`.
- Material and draw integration: Metal branches in `material.cpp` and `ces_render_system.cpp`.
- Shader functions: `gbBundle/Shaders/metal/*.metal`, included in the applicable Xcode target/default Metal library.

Keep Objective-C/Metal syntax in files already configured for Objective-C++ compilation. Follow existing implementation-wrapper conventions rather than exposing Objective-C types to ordinary cross-platform headers.

## Canonical flow and rules

1. The platform graphics context initializes an `MTKView` and `mtl_device`.
2. Device bind creates separate WS, SS, and output command-buffer tracks.
3. Render-pass descriptors create/select the active encoder.
4. Materials bind pipeline, depth, texture, and buffer state.
5. `ces_render_system` supplies Metal MVP/custom buffers and performs the explicit Metal draw path.
6. Device unbind commits command buffers and presents the drawable using the existing semaphore order.

Metal shader lookup is convention-based. `mtl_device` loads `newDefaultLibrary`, and `mtl_render_pipeline_state` resolves functions named `vertex_<shader-guid>` and `fragment_<shader-guid>`. A new GLSL shader pair therefore also needs matching Metal functions with exactly the expected names. Confirm the `.metal` source belongs to every intended Apple target.

For a configured pass, begin with shared technique/material configuration. Modify Metal descriptor/encoder wrappers only when existing attachments or encoder state cannot express the pass.

## What not to touch

For an ordinary material or pass, do not change:

- command-buffer track ordering
- semaphore wait/signal behavior
- drawable acquisition/presentation order
- `mtl_device` singleton lifetime
- native-object bridge ownership
- platform `MTKView` setup

Do not assume successful GLSL loading verifies Metal shader availability; Metal pipeline creation depends on functions compiled into the default library.

## Build and test

No single evidenced command covers every Apple target. For Metal work:

1. Build each affected existing Xcode target (macOS, iOS, and/or tvOS) using its checked-in project/workspace configuration.
2. Confirm all edited `.metal` files are target members and compile into the default library.
3. Run the affected transition and check pipeline/function lookup, attachment formats, encoder creation, validation output, command-buffer completion, and drawable presentation.
4. Exercise multiple frames and teardown.
5. For shared changes, run the Windows Vulkan build as an available regression check and report other Apple/OpenGL targets not exercised.
6. Never claim all Apple platforms verified from one simulator/device target.

