# FoxtrotEngine Project Context

## Overview

FoxtrotEngine is a 2D game engine built with DirectX 11 for Windows, featuring an editor (FoxtrotEditor) with undo/redo support. The engine uses a component-based architecture and is licensed under GNU GPL v3.0.

## Build System

- **IDE:** Visual Studio 2022 (v143)
- **Solution:** `FoxtrotEngine_DirectX.sln`
- **Language:** C++17 (ISO C++17 Standard)
- **Platform:** Windows x64 (Windows SDK 10.0)
- **Dependencies:** Managed via vcpkg (manifest mode)

### Build Configurations
- `Debug` / `Release` - Standard builds
- `Foxtrot_Debug` - Optimized game builds
- `Foxtrot_Editor_Debug` - Editor-specific builds

### Dependency Chain
```
FoxtrotDS → FoxtrotEngine → FoxtrotEditor
```

## Project Structure

```
FoxtrotEngine/
├── FoxtrotEngine/        # Core engine library (DirectX 11)
│   └── src/
│       ├── Core/         # Engine init, game loop, singletons
│       ├── Actors/       # Actor (game object) base classes
│       ├── Components/   # Component system (SpriteRenderer, Colliders, etc.)
│       ├── Managers/     # Singleton managers (Resource, Scene, Collision, etc.)
│       ├── Renderer/     # DirectX 11 rendering
│       ├── ResourceSystem/  # Asset types (Texture, Sprite, Mesh, Shaders)
│       ├── Physics/      # Box2D wrapper, raycasting
│       ├── FileSystem/   # .chunk file serialization
│       ├── InputSystem/  # Keyboard/mouse handling
│       └── WindowSystem/ # OS window management
├── FoxtrotEditor/        # GUI editor (ImGui-based)
│   └── src/
│       ├── CommandHistory.h/cpp  # Undo/redo system
│       ├── Command.h, ActorCommand.h  # Command pattern
│       ├── EditorLayer.h/cpp  # Editor UI layer
│       └── FTCoreEditor.h/cpp  # Editor main class
├── FoxtrotDS/            # Custom data structures library
│   └── Static/           # FTString, ArrayStack, HashMap
│   └── Dynamic/          # DynamicArray, LinkedList, BinaryTree
├── FoxtrotMath/          # Math library (separate DLL)
└── FoxtrotEngine_DirectX.sln
```

## Key Technologies

- **DirectX 11** - Graphics rendering
- **DirectXTex / DirectXTK** - Texture and toolkit utilities
- **Box2D** - 2D physics engine
- **SoLoud** - Audio engine
- **Dear ImGui** - Editor GUI framework
- **nlohmann/json** - JSON parsing
- **magic_enum** - Enum reflection
- **Spine** - Skeleton animation support

## Coding Conventions

### Naming
- **Classes:** PascalCase (`Actor`, `Component`, `ResourceManager`)
- **Private members:** mCamelCase (`mOwner`, `mComponents`)
- **Public methods:** PascalCase (`GetComponent`, `AddChild`)
- **Enums:** PascalCase with namespace (`State::EActive`)
- **Namespaces:** PascalCase (`FTDS`, `ChunkKey`)
- **Constants:** SCREAMING_SNAKE_CASE

### File Organization
- Use `#pragma once` (not traditional header guards)
- Header (.h) and implementation (.cpp) in same directory
- All headers include GNU GPL v3.0 copyright notice
- Use `/// <summary>` XML documentation style

### Formatting (.clang-format)
- **Indentation:** Tabs (4-wide)
- **Column limit:** None
- **Braces:** Allman style (function/class braces on new line)
- **Pointer alignment:** Left (`int* ptr`)
- **Access modifiers:** -4 indent offset

### Memory Management
- Use `DBG_NEW` macro for debug allocation tracking
- Manual memory management with cleanup in destructors
- No smart pointers (raw `new`/`delete`)

## Design Patterns

### Singleton Pattern
Managers use `SINGLETON` or `SINGLETON_PROTECTED` macros:
```cpp
// In header
SINGLETON(ResourceManager)

// Access
ResourceManager::GetInstance()->LoadTexture(...);
```

### Component Pattern
Actors own components via the Actor-Component pattern:
```cpp
Actor* actor = new Actor();
actor->AddComponent<SpriteRenderer>();
actor->GetComponent<Rigidbody2D>()->SetVelocity(...);
```

### Command Pattern (Editor)
Undo/redo via CommandHistory:
```cpp
CommandHistory::GetInstance()->ExecuteCommand(new ActorMoveCommand(...));
CommandHistory::GetInstance()->Undo();
```

## Key Entry Points

### Game Runtime
- `FTCore::Initialize()` - Initialize renderer, managers, window
- `FTCore::RunLoop()` - Main loop: ProcessInput → UpdateGame → GenerateOutput → ProcessEvent
- `FTCore::Shutdown()` - Cleanup

### Editor
- `FTCoreEditor::Initialize()` - Initialize editor with ImGui
- `FTCoreEditor::RunLoop()` - Editor main loop
- `main.cpp` in FoxtrotEditor - Entry point

## Core Classes

| Class | Purpose |
|-------|---------|
| `FTCore` | Engine singleton, game loop |
| `Actor` | Base game object |
| `Component` | Base component class |
| `Scene` | Container for actors |
| `ResourceManager` | Asset loading/caching |
| `SceneManager` | Scene lifecycle |
| `CollisionManager` | Physics simulation |
| `FoxtrotRenderer` | DirectX 11 renderer |
| `Camera` | View/projection management |

## Data Serialization

The engine uses `.chunk` files for scene/resource serialization:
- `ChunkLoader` - Deserializes actors and components
- `ChunkKey` namespaces define serialization keys
- JSON-based format via nlohmann/json

## Component Lifecycle

1. `Initialize(FTCore*)` - Called once on startup
2. `Setup()` - Called after initialization
3. `ProcessInput()` - Handle input each frame
4. `Update()` - Main update logic
5. `LateUpdate()` - Post-update logic
6. `Render()` - Draw component
7. Destructor - Cleanup on removal

## Important Notes

- Windows-only (DirectX 11, no cross-platform)
- Editor requires `Foxtrot_Editor_Debug` configuration
- Assets stored in `Assets/` subdirectories
