# 🧟 Call Of Gody (Godot 4.7 C++17 / Vulkan Forward+) 🔫

<p align="center">
  <b>High-performance, beginner-friendly 2D top-down zombie survival shooter translated from <i>CallOfRay</i> (C++17 raylib) and <i>Call_Of_Pyray</i> (Python) to the official <a href="https://godotengine.org/">Godot Engine 4.7</a> C++ API (<code>godot-cpp</code> / GDExtension) powered by <b>Vulkan</b>.</b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C.svg?style=for-the-badge&logo=c%2B%2B" alt="C++17" />
  <img src="https://img.shields.io/badge/Godot_Engine-4.7_Stable-478CBF.svg?style=for-the-badge&logo=godotengine" alt="Godot 4.7" />
  <img src="https://img.shields.io/badge/Graphics_API-Vulkan_1.0%2B_(Forward%2B)-AC162C.svg?style=for-the-badge&logo=vulkan" alt="Vulkan" />
  <img src="https://img.shields.io/badge/Syntax_Rule-100%25_Dot_Operator_(.)-2EA043.svg?style=for-the-badge" alt="Zero Arrow Operator" />
  <img src="https://img.shields.io/badge/License-Apache_2.0-D22128.svg?style=for-the-badge&logo=apache" alt="Apache 2.0 License" />
</p>

---

## 📖 Project Overview & Philosophy

Welcome to **Call Of Gody**! 🎉 This open-source repository translates the **CallOfRay** / **Call_Of_Pyray** (`Call_Of_Pythy`) top-down shooter into **Godot Engine 4.7 (Stable)** using **GDExtension (`godot-cpp`)** and the **Vulkan (`Forward+`)** rendering backend (backwards-compatible all the way to **Vulkan 1.0**).

### 💡 Designed for C++ Beginners, Python/Raylib Developers & Unforgiving Keyboards!
1. 🚫 **Strict Zero-Arrow Rule (`->` is Forbidden!)**:
   Typing the arrow operator on many international keyboard layouts is painful and interrupts your coding flow. Every single line of C++ in `include/` and `src/` uses **100% dot (`.`) syntax** via C++ reference binding (`godot::Input &input = *godot::Input::get_singleton(); input.is_key_pressed(...);`) and pointer dereferencing (`(*ref.ptr()).method()`).
2. 🐍 **Python-Like Readability + 100% C++17 Performance**:
   - **Clean Object-Oriented Entity Methods**: Just like in `Call_Of_Pyray` (`entities.py`), `Bullet`, `Enemy`, and `Ammo` implement `.move(step)`, `.chasing(...)`, and `.collision(...)`, keeping the main game loop in `src/arena.cpp` short, flat, and easy to read.
   - **2-Map Input Controller (`CONTROLS`)**: Uses two simple `std::unordered_map<std::string, bool>` maps—`inputs` (currently held keys, equivalent to raylib's `IsKeyDown`) and `released` (keys just released this frame, equivalent to raylib's `IsKeyReleased`)—with no unnecessary input timers or extra state booleans.
   - **Zero C++ Syntactic Clutter**: Uses in-class default values instead of long constructor colon-initializer lists, a clean `text_format(...)` helper instead of raw `char[]` / `snprintf` buffers, and `float`-friendly helpers that eliminate noisy `static_cast` calls.
   - **Full C++17 & Vulkan Optimization**: Retains contiguous `std::vector<T>` memory storage, $O(1)$ swap-and-pop projectile/loot removal, fast squared-distance collision checks (`distance_squared_to`), and runtime texture-atlas stitching (`zombie_atlas`) for single-draw-call batching.
3. 🧭 **100% Path-Agnostic IDE Setup (Zero False Positives on New Files)**:
   - Uses [`compile_flags.txt`](./compile_flags.txt) and [`.clangd`](./.clangd) with purely relative include paths (`-Iinclude`, `-Isrc`, `-Ibuild/_deps/godot-cpp-src/include`, `-Ibuild/_deps/godot-cpp-build/gen/include`) and **zero hardcoded drive letters (`A:\`, `C:\`)**.
   - [`CMakeLists.txt`](./CMakeLists.txt) automatically discovers all `src/*.cpp` and `include/*.hpp` files via `GLOB_RECURSE ... CONFIGURE_DEPENDS`. Whenever you create a new `.hpp` or `.cpp` file, your IDE and CMake recognize it immediately with zero false environment errors.

---

## 📚 Deep-Dive Godot C++ API Documentation (Dual Syntax Tome!)

Want to learn how Godot's C++ classes, singletons, smart references (`Ref<T>`), 2D canvas drawing, and audio nodes work—with **side-by-side code examples** comparing **Version A (Dot `.` Syntax)** against **Version B (Arrow `->` Syntax)**?

👉 **[📘 Open `GODOT_CPP_DOT_VS_ARROW_GRIMOIRE.md` (The Dual-Syntax Godot C++ API Grimoire)](./GODOT_CPP_DOT_VS_ARROW_GRIMOIRE.md)** 👈

Inside that companion guide you will find:
- 🔄 Complete Raylib / Python-to-Godot 4.7 C++ Rosetta Stone translation tables.
- ⚖️ Side-by-side **Dot (`.`) vs. Arrow (`->`)** implementations for Singletons, `Ref<T>` resources, child Nodes, 2D Canvas transforms, dynamic texture atlases, and Vulkan telemetry.
- 🔗 Direct links to the official Godot 4.7 Class Reference for every function.

---

## 🚀 Key Features

* 🎯 **Advanced Ballistics & Homing Bullets (*Seeking Mode*)**:
  * Projectiles use trigonometric vector math with randomized accuracy spread cones.
  * Toggleable homing system (`A` key release): bullets in flight scan for the closest zombie within `6×` its radius (`150 px`) via `Bullet::chasing()` and dynamically bend their trajectory toward the target.
* 💥 **Dual Arsenal with Live Fire-Rate Tuning & Dynamic SFX**:
  * **Assault Rifle / Pistol (`F`)**: Continuous automatic fire with medium spread (`±6°`). Automatically plays the machine-gun SFX when `r_fire_rate > 5.0` and switches to single-shot pistol SFX when `r_fire_rate <= 5.0`.
  * **Shotgun (`S`)**: High-impact 5-pellet buckshot blast with wide spread (`±18°`).
  * **Real-Time Cadence Control**: Hold `F` or `S` alongside `+` or `-` during combat to speed up or slow down each weapon's fire rate on the fly!
* 🧲 **Magnetic Loot Pickup & Survival Mechanics**:
  * Defeated zombies have a `7%` chance (`chance` in `settings.cpp`) to drop an ammunition crate (`+50` rounds).
  * The survivor's magnetic field (`player.magnet = 100 px`) automatically pulls nearby ammo crates into your magazine via `Ammo::chasing()` and `Ammo::collision()`.
  * Realistic empty-magazine dry-fire clicking sound (*clipping*) when your ammo hits `0`.
* 🧟 **Dynamic Swarm AI & Runtime Texture Atlas**:
  * Combines `zombie_face_male.png` and `zombie_face_female.png` at runtime in CPU memory using `godot::Image::blit_rect` and uploads a single unified `godot::ImageTexture` atlas to the Vulkan GPU.
  * Zombies detect the player within `200 px` (`Enemy::chasing()`) and relentlessly chase them while displaying floating hit-point (HP) labels underneath each sprite.
* 📊 **Real-Time Vulkan HUD, Pause & Mute Controls**:
  * Live overlay displaying current **FPS**, active **Enemies**, **Rifle & Shotgun Fire Rates**, **Seeking (ON/OFF)** status, **Frame Time (ms)**, active **Bullets**, remaining **Mag** capacity, and the active **Vulkan API Version & GPU Adapter** string.
  * Instant **Pause (`P`)** with a translucent dim overlay and **Mute (`M`)** toggle.

---

## 🎮 Game Controls

| Key / Combination | Trigger Mode | Action Description |
|---|---|---|
| `←` / `→` (**Left / Right Arrows**) | Held (`inputs`) | Rotate the survivor and laser aiming line counter-clockwise / clockwise |
| `↑` / `↓` (**Up / Down Arrows**) | Held (`inputs`) | Move forward / backward along your current aiming direction |
| `F` | Held (`inputs`) | Fire **Assault Rifle / Pistol** (consumes `1` bullet per shot) |
| `S` | Held (`inputs`) | Fire **Shotgun** (5-pellet spread blast, consumes up to `5` bullets) |
| `A` | On Release (`released`) | Toggle homing / seeking bullets (**Seeking ON / OFF**) |
| `P` | On Release (`released`) | Toggle **Pause** (freezes gameplay & dims the screen) |
| `M` | On Release (`released`) | Toggle **Audio Mute** (mutes BGM and all weapon/death SFX) |
| `F` + `+` *(Numpad `+` or `=`)* | Held (`inputs`) | **Increase** Assault Rifle fire rate (`+1.0` shots/sec) |
| `F` + `-` *(Numpad `-` or `-`)* | Held (`inputs`) | **Decrease** Assault Rifle fire rate (`-1.0` shots/sec, min `1.0`) |
| `S` + `+` *(Numpad `+` or `=`)* | Held (`inputs`) | **Increase** Shotgun fire rate (`+0.1` shots/sec) |
| `S` + `-` *(Numpad `-` or `-`)* | Held (`inputs`) | **Decrease** Shotgun fire rate (`-0.1` shots/sec, min `0.1`) |
| `+` *(Without holding `F` or `S`)* | Held (`inputs`) | **Spawn Zombie Horde**: Spawns `10` new zombies at random positions |
| `R` | On Release (`released`) | **Restart Match**: Resets the wave, bullets, ammo crates, and player position |
| `Q` / Close Window | On Release (`released`) | **Quit Game** cleanly via `godot::SceneTree::quit()` |

---

## ⚙️ Default Gameplay & Engine Parameters

Defined cleanly in [`include/settings.hpp`](./include/settings.hpp), [`src/settings.cpp`](./src/settings.cpp), and [`src/entities.cpp`](./src/entities.cpp):

| C++ Variable | Default Value | Description |
|---|---|---|
| `WIDTH`, `HEIGHT` | `1280 × 720` | Resizable viewport dimensions, Vulkan Forward+ renderer (Vulkan 1.0+ compatible) |
| `player.speed` | `300.0 px/s` | Survivor forward/backward movement speed |
| `player.mag` | `1000 bullets` | Initial ammunition count in magazine |
| `player.magnet` | `100 px` | Magnetic attraction range for dropped ammo crates |
| `player.r_fire_rate` | `10.0 shots/s` | Assault rifle cadence (Spread: `±6°`) |
| `player.s_fire_rate` | `0.5 shots/s` | Shotgun cadence (Spread: `±18°`, 5 pellets) |
| `player.seeking` | `false` (`OFF`) | Homing radius: `6 * my_size` (`150 px`) |
| `starting_zombies` | `5 zombies` | Number of zombies spawned when resetting the wave (`R`) |
| `starting_life` | `200 HP` | Hit points per spawned zombie (`70` damage per bullet) |
| `chance` | `7.0%` | Probability for a defeated zombie to drop an `Ammo` crate (`+50` rounds) |
| `RED_C` | `Color(1, 0, 0, 1)` | Classic crimson arena background |

---

## 🛠️ Building & Running the Project

### Prerequisites

* **Godot Engine 4.7 (Stable)** executable
* **C++17 Compiler** (MinGW-w64 GCC, Clang, or MSVC)
* **CMake 3.17+** and **Ninja** (or Make / Visual Studio generator)
* **Python 3.8+** (used by `godot-cpp` during the initial CMake configure step to generate the C++ binding headers from `build/extension_api.json`)

### Quick Start

1. **Configure and compile the C++17 GDExtension library** (or press **`Ctrl+Shift+B`** in VS Code / Antigravity):
   ```powershell
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
   cmake --build build -j 8
   ```
   > ✨ This generates the `godot-cpp` headers inside `build/_deps/` and outputs the compiled library directly into `bin/libcall_of_gody.dll` (or `.so` / `.dylib` on Linux/macOS).

2. **Open or Play in Godot 4.7**:
   * Open the project folder in the **Godot 4.7 Editor** and press **F5** (Play) to run and export the game directly from Godot!

---

## 📂 Repository Directory Structure

```plaintext
Call_Of_Gody/
├── .agents/                                   # 🤖 AI Agent context, architecture notes & pedagogical guides
│   ├── README.md                              # Overview of the .agents knowledge base
│   ├── ARCHITECTURE_AND_TRANSLATION_GUIDE.md  # How CallOfRay & Call_Of_Pyray map to Call_Of_Gody
│   └── CODE_STYLE_AND_KEYBOARD_RULES.md       # Rules enforcing 100% dot (.) syntax & beginner readability
├── assets/                                    # 🎨 Sprites (.png), icons (.ico), and audio tracks (.ogg)
├── bin/                                       # ⚙️ Compiled GDExtension shared library (libcall_of_gody.dll)
├── build/                                     # 🏗️ CMake build artifacts, extension_api.json & godot-cpp
├── include/                                   # 📁 Pedagogical C++17 Header Files (.hpp)
│   ├── arena.hpp                              # Main Godot Node2D class (CallOfGodyArena)
│   ├── entities.hpp                           # Gameplay classes (Bullet, Ammo, Enemy, Character, SFX)
│   ├── register_types.hpp                     # GDExtension module registration header
│   ├── settings.hpp                           # Global screen constants, tuning numbers & Godot Color palette
│   └── utils.hpp                              # Vector trigonometry, text_format, TIMER & 2-map CONTROLS
├── scenes/                                    # 🎬 Godot Scene Files
│   └── main.tscn                              # Main scene instantiating the C++ CallOfGodyArena root node
├── src/                                       # 💻 Pedagogical C++17 Implementation Files (.cpp)
│   ├── arena.cpp                              # _ready(), _process(), and _draw() game loop & Vulkan HUD
│   ├── entities.cpp                           # Entity .move(), .chasing(), .collision(), .draw() & SFX
│   ├── register_types.cpp                     # GDExtension entry point (call_of_gody_library_init)
│   ├── settings.cpp                           # Normalized RGBA Color definitions & gameplay defaults
│   └── utils.cpp                              # Math helpers, dynamic Image::blit_rect atlas & Input polling
├── .clangd                                    # 🧭 Path-agnostic clangd configuration
├── call_of_gody.gdextension                   # 🧩 Connects Godot 4.7 to bin/libcall_of_gody.dll
├── CMakeLists.txt                             # 🛠️ Modern CMake build script with automatic src/ & include/ globbing
├── compile_flags.txt                          # 🧭 Relative include paths for instant IDE support on any drive/OS
├── GODOT_CPP_DOT_VS_ARROW_GRIMOIRE.md         # 📘 Creative API reference comparing "." vs "->" syntax
├── LICENSE                                    # ⚖️ Apache License 2.0
├── project.godot                              # 🚀 Godot 4.7 project configuration (Vulkan Forward+ enabled)
└── README.md                                  # 📖 Open-source documentation (you are here!)
```

---

## 🤖 Need Help or Want an AI Agent to Explain Anything?

Check out the [`.agents/`](./.agents/README.md) directory! It contains tailored guides and context files so that whenever you—or any contributor—ask an AI coding assistant to explain a file, add a new weapon, or create new `.hpp`/`.cpp` files, the assistant immediately understands your **zero-arrow-operator (`.` only)** rule, your Python-friendly C++ style, and the project's path-agnostic Vulkan GDExtension architecture.

---

## 📜 License

Released as **Open Source** under the **[Apache License, Version 2.0](./LICENSE)** for educational, pedagogical, and entertainment purposes.
* [Godot Engine](https://godotengine.org/) and [`godot-cpp`](https://github.com/godotengine/godot-cpp) are licensed under the **MIT License**.
