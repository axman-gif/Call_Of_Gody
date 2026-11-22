# 🏗️ Architecture & Translation Guide: From `CallOfRay` & `Call_Of_Pyray` to `Call_Of_Gody` (Godot 4.7 C++ / Vulkan)

This guide explains how the original raylib (`CallOfRay`) and Python (`Call_Of_Pyray` / `Call_Of_Pythy`) projects map directly to Godot 4.7 C++ (`Call_Of_Gody`).

---

## 1. File-by-File Mapping

| Original `CallOfRay` / `Call_Of_Pyray` File | Translated `Call_Of_Gody` File | Role in the Project |
|---|---|---|
| `include/settings.hpp` & `src/settings.cpp` (`settings.py`) | `include/settings.hpp` & `src/settings.cpp` | Global viewport dimensions (`WIDTH`, `HEIGHT`), gameplay constants (`starting_life`, `starting_zombies`, `my_size`, `chance`), and the normalized RGBA color palette (`godot::Color`). |
| `include/utils.hpp` & `src/utils.cpp` (`utils.py`) | `include/utils.hpp` & `src/utils.cpp` | Vector trigonometry (`rotate_pt`, `move_pt`, `chase`, `two_points_distance`, `distance_trigger`), RNG (`get_random_value`), string formatting (`text_format`), asset loading (`load_assets`, `unload_assets` with dynamic `Image::blit_rect` zombie atlas), `TIMER`, and the 2-map `CONTROLS` class (`inputs` + `released`). |
| `include/entities.hpp` & `src/entities.cpp` (`entities.py`) | `include/entities.hpp` & `src/entities.cpp` | Entity classes: `Bullet`, `Ammo`, `Enemy`, `Character`, and `SFX`. Mirrors `entities.py` with `.move()`, `.chasing()`, `.collision()`, and `.draw(godot::Node2D &canvas)`. |
| `src/main.cpp` (`Call_Of_Pyray.py`) | `include/arena.hpp` & `src/arena.cpp` | Replaces `int main()` and the `while (!WindowShouldClose())` loop with `godot::CallOfGodyArena` (inheriting from `godot::Node2D`), implementing `_ready()`, `_process(double delta)`, and `_draw()`. |
| *(N/A in raylib/Python)* | `include/register_types.hpp` & `src/register_types.cpp` | Registers `CallOfGodyArena` with Godot 4.7's `ClassDB` when the GDExtension dynamic library (`bin/libcall_of_gody.dll`) is loaded. |
| *(N/A in raylib/Python)* | `compile_flags.txt`, `.clangd`, `CMakeLists.txt` | Provides 100% path-agnostic relative include paths (zero hardcoded drive letters) and automatic `src/*.cpp` & `include/*.hpp` discovery so new files work immediately in any IDE. |

---

## 2. How the Game Loop Works (`main()` vs. `Node2D`)

In **Godot 4.7 C++ (`Call_Of_Gody`)**, the engine manages the Vulkan window and swapchain (backwards compatible down to **Vulkan 1.0**), calling three virtual methods on `CallOfGodyArena`:

1. **`_ready()`**: Runs once when `CallOfGodyArena` enters the SceneTree. Calls `load_assets()`, queries the Vulkan driver version from `RenderingServer`, creates child `AudioStreamPlayer` nodes for each sound channel, and spawns the survivor and initial zombies.
2. **`_process(double delta)`**: Runs every frame. Converts `delta` (seconds) into `frame` (milliseconds, capped at `16.6667 ms`), polls keyboard state via `player.actions.get_input()`, handles global toggles (`paused` on `P`, `mute` on `M`, `reset` on `R`, `QUIT` on `Q`), updates zombie chase AI (`zombie.chasing(...)`), processes bullet movement/homing/collisions (`b.move(...)`, `b.chasing(...)`, `z.collision(...)`) and magnetic ammo pickups (`ammo.chasing(...)`, `ammo.collision(...)`), updates `SFX` timers, calls `player.actions.reset()`, and requests `queue_redraw()`.
3. **`_draw()`**: Runs each frame to render the background, survivor, aiming line, rotating gun sprite (`draw_set_transform`), ammo crates, bullets, zombies, health labels, pause overlay, and telemetry HUD (`text_format(...)`).
