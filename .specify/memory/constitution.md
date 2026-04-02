<!--
SYNC IMPACT REPORT
==================
Version change: [unversioned template] → 1.0.0
Modified principles: N/A (initial population from template placeholders)

Added sections:
  - I.   Modern C++20
  - II.  Target-Based CMake
  - III. FetchContent Dependency Management
  - IV.  Readable Code & Single Responsibility
  - V.   Header Organization
  - VI.  OpenGL Error Handling (NON-NEGOTIABLE)
  - VII. Shader-Only Rendering (NON-NEGOTIABLE)
  - VIII.Naming Conventions
  - Documentation Standards
  - Development Workflow
  - Governance

Removed sections: None (all were placeholder tokens)

Templates requiring updates:
  ✅ .specify/memory/constitution.md — written now
  ✅ .specify/templates/plan-template.md — Constitution Check section reads
         "[Gates determined based on constitution file]"; dynamically filled
         per feature. No structural change required.
  ✅ .specify/templates/spec-template.md — generic; no constitution-specific
         sections required. No change needed.
  ✅ .specify/templates/tasks-template.md — generic; task categories are
         feature-driven. No change needed.

Deferred TODOs: None — all placeholders resolved.
-->

# C++ OpenGL Renderer Constitution

## Core Principles

### I. Modern C++20

C++20 MUST be the language standard for all source files. Modern language features
(`concepts`, `ranges`, `std::span`) MUST be used where they genuinely improve
clarity or type safety; they MUST NOT be used for novelty or to obscure intent.
Legacy constructs with a clearly superior modern equivalent are forbidden.

### II. Target-Based CMake

CMake minimum version 3.20 is required. Build configuration MUST use only
target-scoped commands: `target_include_directories`, `target_compile_features`,
`target_link_libraries`. Bare commands (`include_directories`, `add_definitions`,
`link_libraries`) are unconditionally forbidden. Every library and executable
MUST be a properly declared CMake target.

### III. FetchContent Dependency Management

All external dependencies MUST be managed exclusively via CMake's `FetchContent`
module. Git submodules and package managers (vcpkg, Conan, apt, Homebrew, etc.)
are forbidden. Each dependency declaration MUST pin an explicit version (tag or
commit hash); floating `main`/`master` references are not permitted.

### IV. Readable Code & Single Responsibility

Code MUST prioritize clarity and readability over brevity or cleverness.
One-liners that sacrifice comprehension are forbidden. Every class MUST have a
single, well-defined responsibility; if that responsibility cannot be stated in
one sentence the class MUST be split. YAGNI applies: no speculative abstractions
or premature generalization.

### V. Header Organization

All header files MUST use `#pragma once`. Interface files use the `.hpp`
extension; implementation files use `.cpp`. Template definitions MUST reside in
`.hpp` files because they cannot be separately compiled. Non-template
implementation MUST NOT appear in headers.

### VI. OpenGL Error Handling (NON-NEGOTIABLE)

Every OpenGL API call MUST be wrapped with an error-checking macro in debug
builds (e.g., `GL_CHECK(call)`). Silent OpenGL failures are forbidden. The macro
MUST capture the call-site source location and either abort or log a fatal
message in debug mode. Release builds MAY strip the check for performance, but
debug-build coverage is mandatory for all rendering code paths.

### VII. Shader-Only Rendering (NON-NEGOTIABLE)

All rendering MUST go through programmable shaders. The legacy fixed-function
pipeline (`glBegin`/`glEnd`, `glColor*`, `glVertex*`, `glMatrixMode`, etc.) is
unconditionally forbidden. Every draw call MUST have an active, fully linked
vertex and fragment shader program bound at the time of submission.

### VIII. Naming Conventions

The following naming rules MUST be applied consistently across all source files:

- **Classes and structs**: `PascalCase` (e.g., `VertexBuffer`, `ShaderProgram`)
- **Methods and variables**: `camelCase` (e.g., `bindTexture`, `vertexCount`)
- **Constants and macros**: `UPPER_SNAKE_CASE` (e.g., `MAX_TEXTURE_UNITS`, `GL_CHECK`)

## Documentation Standards

The `README.md` MUST be kept current at all times. It MUST contain:

- A description of what the renderer does and its current rendering capabilities.
- Up-to-date CMake configure and build steps sufficient to reproduce a working
  build from a clean checkout.
- Runtime dependencies (OpenGL version, windowing libraries) and supported
  platform(s).

Documentation MUST be updated in the same commit that introduces any change
affecting build instructions or project capabilities.

## Development Workflow

- Every code review MUST verify compliance with all Core Principles before merge.
- New rendering code MUST include `GL_CHECK` coverage; reviewers MUST reject
  naked OpenGL calls in debug-build paths.
- New external dependencies MUST be introduced exclusively via `FetchContent`
  with a pinned version; reviewers MUST reject submodule or package-manager
  additions.
- Complexity deviating from these principles MUST be justified in the relevant
  feature plan before implementation begins.

## Governance

This constitution supersedes all other coding practices and style guides for
this project. Amendments require:

1. A written rationale identifying the affected principle and motivation.
2. A version bump per the policy below.
3. Updates to all dependent templates and documentation in the same change.

**Versioning Policy**:
- **MAJOR**: Removal or redefinition of a Core Principle.
- **MINOR**: Addition of a new principle or material expansion of guidance.
- **PATCH**: Clarifications, wording fixes, or non-semantic refinements.

All pull requests MUST be checked against this constitution prior to merge.
Violations MUST be resolved; justified exceptions MUST be documented in the
feature plan's Complexity Tracking table.

**Version**: 1.0.0 | **Ratified**: 2026-04-01 | **Last Amended**: 2026-04-01
