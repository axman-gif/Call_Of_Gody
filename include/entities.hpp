/******************************************************************************
 * File: include/entities.hpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * Defines `Bullet`, `Ammo`, `Enemy`, `Character`, and `SFX` with the exact
 * `.move()`, `.chasing()`, and `.collision()` methods from `Call_Of_Pyray`
 * (`entities.py`) and `CallOfRay`.
 ******************************************************************************/

#pragma once

#include "utils.hpp"

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

// ============================================================================
// 1. Bullet Entity
// ============================================================================
class Bullet {
public:
  godot::Vector2 position;
  float angle = 0.0f;
  bool seeking = false;
  int size_r = 5;
  int damage = 70;
  float speed = 0.0f;
  float range = 150.0f;

  Bullet(godot::Vector2 pos, float ang, bool seek, int dmg = 70);

  void move(float step);
  void chasing(godot::Vector2 target_pos, float step);
  bool collision(godot::Vector2 target_pos, float target_size) const;
  void draw(godot::Node2D &canvas) const;
};

// ============================================================================
// 2. Ammo Crate Entity
// ============================================================================
class Ammo {
public:
  godot::Vector2 position;
  int mag = 50;
  int size_r = 25;
  std::string type = "rifle";
  float speed = 500.0f;

  explicit Ammo(godot::Vector2 pos, int m = 50, int s = 25,
                std::string t = "rifle");

  void chasing(godot::Vector2 player_pos, float step, float magnet_radius);
  bool collision(godot::Vector2 player_pos, float player_size) const;
  void draw(godot::Node2D &canvas) const;
};

// ============================================================================
// 3. Zombie Enemy Entity
// ============================================================================
class Enemy {
public:
  int life = 200;
  int sprite_idx = 0;
  godot::Vector2 position;
  int size_r = 25;
  godot::Color color;
  float speed = 150.0f;
  float range = 200.0f;

  Enemy(godot::Vector2 enemy_pos, int s, int l, int spr);

  void chasing(godot::Vector2 player_pos, float step);
  bool collision(godot::Vector2 target_pos, float target_size) const;
  void draw(godot::Node2D &canvas) const;
  void draw_label(godot::Node2D &canvas,
                  const godot::Ref<godot::Font> &font) const;
};

// ============================================================================
// 4. Player Survivor Entity (Character)
// ============================================================================
class Character {
public:
  godot::Vector2 position;
  float angle = 0.0f;
  godot::Vector2 sight;
  float speed = 300.0f;
  int mag = 1000;
  int magnet = 100;

  float r_fire_rate = 10.0f;
  TIMER rifle_timer;
  int rifle_accuracy = 2;

  float s_fire_rate = 0.5f;
  TIMER shotgun_timer;
  int shotgun_accuracy = 6;

  bool seeking = false;
  CONTROLS actions;

  Character() = default;
  explicit Character(godot::Vector2 pos);

  void update(float frametime);
  void draw(godot::Node2D &canvas, float w, float h) const;
  void handle_input(float frame, std::unordered_map<std::string, bool> &playing,
                    std::vector<Bullet> &active_bullets);
};

// ============================================================================
// 5. Dynamic Sound Effect Controller (SFX)
// ============================================================================
class SFX {
public:
  TIMER timer;
  bool playing = false;
  float length = 1000.0f;
  godot::Ref<godot::AudioStream> soundtrack;
  godot::AudioStreamPlayer *player_node = nullptr;
  float delay = 0.0f;

  SFX() = default;
  SFX(const godot::Ref<godot::AudioStream> &s, godot::AudioStreamPlayer *node,
      float del, float len = 1000.0f);

  void update(float frame);
  void play_sfx();
  void stop(float max_time = 0.0f);
};
