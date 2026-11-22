# 📏 Code Style, Pedagogical & Keyboard Rules for AI Agents

Whenever you (an AI assistant or human contributor) explain, modify, or add C++ code in `Call_Of_Gody`, you **MUST** obey the following project rules:

## 1. 🚫 The Arrow Operator (`->`) is Strictly Forbidden in Source Code!
- **Why**: Due to keyboard layout ergonomics, typing the arrow operator is painful for the author.
- **How to write C++ without it**:
  - For raw pointers returned by Godot APIs (like `godot::Input::get_singleton()`, `get_tree()`, or `memnew(godot::AudioStreamPlayer)`), dereference the pointer into a C++ reference variable (`T &ref = *ptr;`) and use the **dot (`.`) operator** for all method calls and member accesses:
    ```cpp
    godot::Input *input_ptr = godot::Input::get_singleton();
    if (input_ptr != nullptr) {
        godot::Input &input = *input_ptr;
        if (input.is_key_pressed(godot::KEY_LEFT)) {
            // ...
        }
    }
    ```
  - For Godot reference-counted handles (`godot::Ref<T>`), call `.ptr()` with the dot operator and dereference it into a C++ reference (`T &obj = *my_ref.ptr();`):
    ```cpp
    if (z1_ref.is_valid()) {
        godot::Image &z1 = *z1_ref.ptr();
        int w = z1.get_width();
    }
    ```
- **Exception**: The companion documentation file [`GODOT_CPP_DOT_VS_ARROW_GRIMOIRE.md`](../GODOT_CPP_DOT_VS_ARROW_GRIMOIRE.md) intentionally shows both Version A (`.` syntax) and Version B (`->` syntax) side-by-side for educational comparison.

## 2. 🐍 Mirror `Call_Of_Pyray` (Python) & `CallOfRay` Readability Without Sacrificing Performance
- **In-Class Default Member Initializers**: Prefer initializing class members directly in the `.hpp` declaration (`float speed = 300.0f;`, `AudioStreamPlayer *bgm_player = nullptr;`) and normal assignments inside constructor bodies `{ ... }` rather than long C++ colon initializer lists (`: a(1), b(2), ...`).
- **Entity Methods**: Keep `.move()`, `.chasing()`, and `.collision()` methods on `Bullet`, `Enemy`, and `Ammo` so `src/arena.cpp` stays clean and reads like `Call_Of_Pyray.py`.
- **2-Map Input Controller (`CONTROLS`)**: `CONTROLS` uses only `inputs` (held keys) and `released` (keys released on this exact frame), with no `TIMER` inside `CONTROLS`.
- **String Formatting**: Use `text_format("FPS: %d", current_fps)` from `include/utils.hpp` instead of verbose `char buf[64]; std::snprintf(...)` blocks.
- **Keep All C++17 & Vulkan Optimizations**: Maintain contiguous `std::vector<T>` containers, $O(1)$ swap-and-pop removal (`std::swap(v[i], v.back()); v.pop_back();`), squared-distance checks (`distance_squared_to`), and the runtime-stitched `zombie_atlas`.

## 3. 🧭 Keep All Configs 100% Path-Agnostic (Zero Hardcoded Drive Letters)
- Never hardcode drive letters (`A:/`, `C:/`) in `.clangd`, `compile_flags.txt`, `CMakeLists.txt`, or `.vscode/*.json`.
- `compile_flags.txt` uses purely relative `-I` paths so `clangd` works immediately on any drive/folder and for any newly created `.hpp` or `.cpp` file in `include/` or `src/`.
- `CMakeLists.txt` uses `file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS ...)` so new `.cpp` and `.hpp` files are compiled automatically.

## 4. 🌐 Everything Must Be in English
- All source code, identifiers, comments, commit messages, and Markdown documentation must be written in clear, pedagogical English.
