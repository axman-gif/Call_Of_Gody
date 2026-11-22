# 🤖 `.agents` Knowledge Base for Call Of Gody

Welcome to the **`.agents`** directory of **Call Of Gody**.

This folder helps human learners and AI coding assistants understand the architecture, beginner-friendly Pythonic C++ philosophy, path-agnostic IDE configuration, and strict coding rules of this repository.

## 📚 Files in `.agents/`

1. **[`CODE_STYLE_AND_KEYBOARD_RULES.md`](./CODE_STYLE_AND_KEYBOARD_RULES.md)**
   - Explains the mandatory **Zero-Arrow-Operator Rule** (`->` is strictly forbidden in all project C++ code; only `.` dot syntax via C++ references is allowed).
   - Explains the Python-like (`Call_Of_Pyray`) code style: in-class default initializers instead of constructor colon-initializer lists, entity `.move()` / `.chasing()` / `.collision()` methods, `text_format(...)`, and the 2-map `CONTROLS` system (`inputs` + `released`).
2. **[`ARCHITECTURE_AND_TRANSLATION_GUIDE.md`](./ARCHITECTURE_AND_TRANSLATION_GUIDE.md)**
   - File-by-file mapping showing how `CallOfRay` (C++17 raylib) and `Call_Of_Pyray` (Python) map to `Call_Of_Gody` (Godot 4.7 C++ GDExtension running on **Vulkan**).
   - Explains how `compile_flags.txt` and `CMakeLists.txt` (`GLOB_RECURSE`) provide 100% path-agnostic IDE & build support for any existing or newly created `.hpp` and `.cpp` files.
