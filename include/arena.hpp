/******************************************************************************
 * File: include/arena.hpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * `CallOfGodyArena` is the main game node (`godot::Node2D`) that mirrors
 * `Call_Of_Pyray.py` and `CallOfRay/src/main.cpp`:
 * - `_ready()`        : Setup code before the `while` loop.
 * - `_process(delta)` : Per-frame game logic inside the `while` loop.
 * - `_draw()`         : Per-frame rendering between `BeginDrawing()` / `EndDrawing()`.
 ******************************************************************************/

#pragma once

#include "entities.hpp"
#include "settings.hpp"
#include "utils.hpp"

#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace godot {

class CallOfGodyArena : public Node2D {
  GDCLASS(CallOfGodyArena, Node2D)

private:
  // ========================================================================
  // Active Game State Containers (Mirrors Call_Of_Pyray.py & CallOfRay).
  // ========================================================================
  Character player;
  std::vector<Bullet> active_bullets;
  std::vector<Enemy> enemies;
  std::vector<Ammo> active_ammo;

  std::unordered_map<std::string, SFX> SFXS;
  std::unordered_map<std::string, bool> plays;

  // Audio channel nodes created at startup in `_ready()`
  AudioStreamPlayer *bgm_player = nullptr;
  AudioStreamPlayer *rifle_player = nullptr;
  AudioStreamPlayer *shotgun_player = nullptr;
  AudioStreamPlayer *pistol_player = nullptr;
  AudioStreamPlayer *death_player = nullptr;
  AudioStreamPlayer *clipping_player = nullptr;

  std::string graphics_api_version = "Vulkan (Forward+)";
  float last_frame_ms = 16.6f;
  bool initialized = false;
  bool paused = false;
  bool mute = false;

  AudioStreamPlayer *create_audio_channel(const char *node_name, float volume_linear);

protected:
  static void _bind_methods();

public:
  CallOfGodyArena() = default;
  ~CallOfGodyArena() override;

  void _ready() override;
  void _process(double delta) override;
  void _draw() override;
};

} // namespace godot
