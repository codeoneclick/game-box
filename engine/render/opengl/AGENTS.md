# OpenGL Backend Instructions

Read `engine/render/AGENTS.md` first. These rules apply only when a task explicitly changes or validates the OpenGL path.

## Status and code location

OpenGL branches exist for Windows and Apple platform contexts, but current `gl_commands.hpp` branches select Vulkan on Windows and Metal on Apple. Current OpenGL build/runtime health is `UNKNOWN`.

Primary files:

- `gbCore/sources/gl_commands.hpp` and `.cpp`: OpenGL-shaped constants and commands.
- `gbCore/sources/graphics_context_win32.cpp`, `graphics_context_osx.cpp`, `graphics_context_ios.cpp`, `graphics_context_tvos.cpp`: platform context/swap/flush branches.
- `gbCore/sources/render_technique_ws.cpp`, `render_technique_ss.cpp`, `render_technique_main.cpp`: framebuffer-based pass handling.
- `gbCore/sources/shader_compiler_glsl.cpp`: GLSL compile/link.
- OpenGL branches in `material.cpp`, `texture.cpp`, `vbo.cpp`, `ibo.cpp`, and `vao.cpp`.

## Patterns and rules

- Shared renderer code should use established `gl::constant` and `gl::command` wrappers rather than adding scattered raw GL calls.
- Copy the closest command wrapper and implement all relevant GL version/platform guards when the shared vocabulary must grow.
- Preserve both `OPENGL_20_API` and `OPENGL_30_API` behavior only when source evidence shows the operation is meaningful for both. Guard unsupported behavior explicitly.
- Render techniques own GL framebuffer binding/attachment behavior; the common render-pass descriptor factory has Vulkan/Metal implementations, not an OpenGL descriptor.
- External shaders remain paired `.vert`/`.frag` GLSL sources and must respect the version/platform defines assembled by `shader_compiler_glsl.cpp`.

## What not to touch

- Do not change `USED_GRAPHICS_API` merely to implement a shader/material/pass.
- Do not alter platform context creation, buffer swapping, or GL version selection for an isolated draw effect.
- Do not add OpenGL-only behavior to application code or unguarded shared code.
- Do not report OpenGL as supported/working solely because guarded source exists.

## Build and test

There is no evidenced current OpenGL build helper or green target. When OpenGL is explicitly in scope:

1. Record the platform, GL version branch, and project configuration used.
2. Build the actual target with OpenGL deliberately selected using the repository's existing selector mechanism; do not leave an unrelated selector change behind.
3. Run the affected transition and inspect framebuffer completeness, GLSL compile/link output, GL errors, material state, and presentation.
4. Exercise both creation and destruction of any changed GL resource.
5. Run the normal current backend build as a regression check if shared code changed.
6. If an OpenGL target cannot be built, report it as unverified; a Vulkan or Metal result is not a substitute.

