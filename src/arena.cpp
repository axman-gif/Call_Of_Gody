/******************************************************************************
 * File: src/arena.cpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * Direct Godot 4.7 C++17 translation of `Call_Of_Pyray.py` and `CallOfRay/src/main.cpp`:
 * 1. `_ready()`        -> Initialization before the main loop.
 * 2. `_process(delta)` -> Per-frame input, AI, ballistics, collisions, and audio.
 * 3. `_draw()`         -> Per-frame Vulkan 2D rendering and HUD telemetry.
 ******************************************************************************/

#include "arena.hpp"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/theme_db.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace godot {

CallOfGodyArena::~CallOfGodyArena() {
  active_bullets.clear();
  enemies.clear();
  active_ammo.clear();
  SFXS.clear();
  if (initialized) {
    unload_assets();
    initialized = false;
  }
}

AudioStreamPlayer *CallOfGodyArena::create_audio_channel(const char *node_name, float volume_linear) {
  AudioStreamPlayer *channel_ptr = memnew(AudioStreamPlayer);
  AudioStreamPlayer &channel = *channel_ptr;

  channel.set_name(String(node_name));
  float volume_db = (volume_linear > 0.001f) ? (20.0f * std::log10(volume_linear)) : -80.0f;
  channel.set_volume_db(volume_db);

  add_child(channel_ptr);
  return channel_ptr;
}

// ============================================================================
// 1. Initialization (`_ready()`)
// ============================================================================
void CallOfGodyArena::_ready() {
  load_assets();
  initialized = true;

  // Query active Vulkan version and GPU name
  RenderingServer *rs_ptr = RenderingServer::get_singleton();
  if (rs_ptr != nullptr) {
    RenderingServer &rs = *rs_ptr;
    String api_ver = rs.get_video_adapter_api_version();
    String gpu_name = rs.get_video_adapter_name();
    String combined = String("Vulkan ") + api_ver + String(" (") + gpu_name + String(")");
    graphics_api_version = combined.utf8().get_data();
  }

  // Create audio channels
  rifle_player    = create_audio_channel("SFX_Rifle", 0.6f);
  pistol_player   = create_audio_channel("SFX_Pistol", 0.5f);
  shotgun_player  = create_audio_channel("SFX_Shotgun", 0.3f);
  death_player    = create_audio_channel("SFX_ZombieDeath", 0.7f);
  clipping_player = create_audio_channel("SFX_EmptyMag", 0.7f);
  bgm_player      = create_audio_channel("BGM_DarkFuture", 1.0f);

  if (bgm_player != nullptr && BGM_music.is_valid()) {
    AudioStreamPlayer &bgm = *bgm_player;
    bgm.set_stream(BGM_music);
  }

  SFXS["rifle"]    = SFX(auto_fire,    rifle_player,    10.0f,  1000.0f);
  SFXS["shotgun"]  = SFX(shotgun,      shotgun_player,  50.0f,  1000.0f);
  SFXS["pistol"]   = SFX(single_shot,  pistol_player,   0.0f,   1000.0f);
  SFXS["death"]    = SFX(zombie_death, death_player,    250.0f, 1000.0f);
  SFXS["clipping"] = SFX(empty_mag,    clipping_player, 125.0f, 1000.0f);

  plays["rifle"]    = false;
  plays["shotgun"]  = false;
  plays["pistol"]   = false;
  plays["death"]    = false;
  plays["clipping"] = false;

  // Initial entities (Survivor + 2 starting zombies)
  Vector2 center(WIDTH / 2.0f, HEIGHT / 2.0f);
  Vector2 enemy_pos(WIDTH * 3.0f / 4.0f, HEIGHT * 3.0f / 4.0f);

  player = Character(center);
  enemies.clear();
  enemies.push_back(Enemy(enemy_pos, my_size, starting_life, random_sprite()));
  enemies.push_back(Enemy(
      Vector2(get_random_value(0.0f, WIDTH), get_random_value(0.0f, HEIGHT)),
      my_size,
      starting_life,
      random_sprite()
  ));

  Engine *engine_ptr = Engine::get_singleton();
  bool in_editor = (engine_ptr != nullptr) ? (*engine_ptr).is_editor_hint() : false;
  if (!in_editor && bgm_player != nullptr && BGM_music.is_valid()) {
    AudioStreamPlayer &bgm = *bgm_player;
    bgm.play();
  }

  queue_redraw();
}

// ============================================================================
// 2. Main Game Loop (`_process(double delta)`)
// ============================================================================
void CallOfGodyArena::_process(double delta) {
  Rect2 viewport_rect = get_viewport_rect();
  if (viewport_rect.size.x > 0.0f && viewport_rect.size.y > 0.0f) {
    WIDTH = viewport_rect.size.x;
    HEIGHT = viewport_rect.size.y;
  }

  Engine *engine_ptr = Engine::get_singleton();
  bool in_editor = (engine_ptr != nullptr) ? (*engine_ptr).is_editor_hint() : false;
  if (in_editor) {
    queue_redraw();
    return;
  }

  // Loop background music if not muted
  if (bgm_player != nullptr && BGM_music.is_valid()) {
    AudioStreamPlayer &bgm = *bgm_player;
    if (!bgm.is_playing() && !mute) {
      bgm.play();
    }
  }

  // Convert delta seconds to milliseconds (capped at 1000/60 ms)
  last_frame_ms = static_cast<float>(delta) * 1000.0f;
  float frame = std::min(last_frame_ms, 1000.0f / 60.0f);

  Vector2 center(WIDTH / 2.0f, HEIGHT / 2.0f);
  Vector2 enemy_pos(WIDTH * 3.0f / 4.0f, HEIGHT * 3.0f / 4.0f);

  // ---- Global Inputs ----
  player.actions.get_input();

  if (player.actions.released["paused"]) {
    paused = !paused;
  }
  if (player.actions.released["muted"]) {
    mute = !mute;
  }
  if (player.actions.released["QUIT"]) {
    game_over = true;
    SceneTree *tree_ptr = get_tree();
    if (tree_ptr != nullptr) {
      SceneTree &tree = *tree_ptr;
      tree.quit();
    }
    return;
  }

  if (player.actions.released["reset"]) {
    player = Character(center);
    enemies.clear();
    active_bullets.clear();
    active_ammo.clear();
    for (int n = 0; n < starting_zombies; n++) {
      enemies.push_back(Enemy(enemy_pos, my_size, starting_life, random_sprite()));
    }
  }

  if (player.actions.inputs["fr+"] &&
      !player.actions.inputs["rifle"] &&
      !player.actions.inputs["shotgun"]) {
    for (int n = 0; n < 10; n++) {
      enemies.push_back(Enemy(
          Vector2(get_random_value(0.0f, WIDTH), get_random_value(0.0f, HEIGHT)),
          my_size,
          starting_life,
          random_sprite()
      ));
    }
  }

  if (!paused) {
    for (auto &play : plays) {
      play.second = false;
    }

    player.handle_input(frame, plays, active_bullets);
    player.update(frame);

    if (mute) {
      for (auto &play : plays) {
        play.second = false;
      }
      if (bgm_player != nullptr) {
        AudioStreamPlayer &bgm = *bgm_player;
        bgm.stop();
      }
    }

    // Zombies chasing player
    for (auto &zombie : enemies) {
      float step = zombie.speed * (frame / 1000.0f);
      zombie.chasing(player.position, step);
    }

    // Bullets movement, homing ("seeking"), and zombie collisions
    for (size_t i = 0; i < active_bullets.size();) {
      Bullet &b = active_bullets[i];
      bool remove_bullet = false;

      if (out_of_range(b.position, WIDTH, HEIGHT)) {
        remove_bullet = true;
      } else {
        float step = b.speed * (frame / 1000.0f);

        if (enemies.empty()) {
          b.move(step);
        } else {
          size_t closest_idx = 0;
          float closest = two_points_distance(b.position, enemies[0].position);
          for (size_t j = 1; j < enemies.size(); j++) {
            float d = two_points_distance(b.position, enemies[j].position);
            if (d < closest) {
              closest = d;
              closest_idx = j;
            }
          }
          Enemy &z = enemies[closest_idx];

          if (b.seeking) {
            b.chasing(z.position, step);
          } else {
            b.move(step);
          }

          if (z.collision(b.position, static_cast<float>(b.size_r))) {
            z.life -= b.damage;
            remove_bullet = true;
            if (z.life <= 0) {
              if (!mute) {
                plays["death"] = true;
              }
              if (happened(chance)) {
                active_ammo.push_back(Ammo(z.position));
              }
              enemies.erase(enemies.begin() + closest_idx);
            }
          }
        }
      }

      if (remove_bullet) {
        std::swap(active_bullets[i], active_bullets.back());
        active_bullets.pop_back();
      } else {
        i += 1;
      }
    }

    // Ammo crates magnetic attraction & pickup collisions
    for (size_t a = 0; a < active_ammo.size();) {
      Ammo &ammo = active_ammo[a];
      float step = ammo.speed * (frame / 1000.0f);

      if (player.magnet > 0) {
        ammo.chasing(player.position, step, static_cast<float>(player.magnet));
      }

      if (ammo.collision(player.position, static_cast<float>(my_size))) {
        player.mag += ammo.mag;
        std::swap(active_ammo[a], active_ammo.back());
        active_ammo.pop_back();
      } else {
        a += 1;
      }
    }

    // Dynamic audio delays based on fire rate
    SFXS["rifle"].delay   = (player.r_fire_rate > 10.0f) ? 50.0f : 0.0f;
    SFXS["shotgun"].delay = (player.s_fire_rate > 5.0f)  ? 50.0f : 0.0f;

    // Trigger & update sound effects
    for (auto &play : plays) {
      SFXS[play.first].update(frame);
      float max_time = 200.0f;
      if (play.second) {
        SFXS[play.first].play_sfx();
      }
      if (play.first == "rifle") {
        max_time = 100.0f;
      }
      if (play.first == "shotgun") {
        max_time = (player.s_fire_rate >= 5.0f) ? 200.0f : (1000.0f / player.s_fire_rate);
      }
      if (play.first == "death") {
        max_time = SFXS[play.first].length;
      }
      SFXS[play.first].stop(max_time);
    }
  } else {
    for (auto &sfx_pair : SFXS) {
      sfx_pair.second.stop(0.0f);
    }
  }

  player.actions.reset();
  queue_redraw();
}

// ============================================================================
// 3. Rendering & HUD (`_draw()`)
// ============================================================================
void CallOfGodyArena::_draw() {
  // 1. Background.
  draw_rect(Rect2(0.0f, 0.0f, WIDTH, HEIGHT), RED_C, true);

  // 2. Player & Sight Line
  player.draw(*this, WIDTH, HEIGHT);

  // 3. Rotating Gun Sprite at `player.sight`
  if (Gun.is_valid()) {
    const Texture2D &gun_tex = *Gun.ptr();
    Vector2 origin_offset(-gun_tex.get_width() / 2.0f, -gun_tex.get_height() / 2.0f);

    draw_set_transform(player.sight, Math::deg_to_rad(player.angle), Vector2(1.0f, 1.0f));
    draw_texture(Gun, origin_offset, WHITE_C);
    draw_set_transform(Vector2(0.0f, 0.0f), 0.0f, Vector2(1.0f, 1.0f));
  }

  // 4. Default UI Font
  Ref<Font> default_font;
  ThemeDB *theme_db_ptr = ThemeDB::get_singleton();
  if (theme_db_ptr != nullptr) {
    ThemeDB &theme_db = *theme_db_ptr;
    default_font = theme_db.get_fallback_font();
  }

  // 5. Entities (Ammo Crates, Bullets, Zombies, Health Labels)
  for (const auto &ammo : active_ammo) {
    ammo.draw(*this);
  }
  for (const auto &bullet : active_bullets) {
    bullet.draw(*this);
  }
  for (const auto &zombie : enemies) {
    zombie.draw(*this);
  }
  for (const auto &zombie : enemies) {
    zombie.draw_label(*this, default_font);
  }

  if (default_font.is_null()) {
    return;
  }
  const Font &font_obj = *default_font.ptr();

  // 6. Center-Screen Performance & Closest Zombie Alerts
  String txt2 = text_format("Last frame took %d ms, bullets: %d",
                            static_cast<int>(last_frame_ms),
                            static_cast<int>(active_bullets.size()));
  Vector2 txt2_size = font_obj.get_string_size(txt2, HORIZONTAL_ALIGNMENT_LEFT, -1.0f, fontsize2);
  draw_string(
      default_font,
      Vector2(WIDTH / 2.0f - txt2_size.x / 2.0f, HEIGHT / 2.0f + 20.0f + fontsize2),
      txt2,
      HORIZONTAL_ALIGNMENT_LEFT,
      -1.0f,
      fontsize2,
      YELLOW_C
  );

  if (!enemies.empty()) {
    float closest_dist = std::hypot(WIDTH, HEIGHT);
    for (const auto &z : enemies) {
      float d = two_points_distance(z.position, player.position);
      if (d < closest_dist) {
        closest_dist = d;
      }
    }
    String txt1 = text_format("Stay away! closest zombie: %d", static_cast<int>(closest_dist));
    Vector2 txt1_size = font_obj.get_string_size(txt1, HORIZONTAL_ALIGNMENT_LEFT, -1.0f, fontsize2);
    draw_string(
        default_font,
        Vector2(WIDTH / 2.0f - txt1_size.x / 2.0f, HEIGHT / 2.0f - 20.0f + fontsize2),
        txt1,
        HORIZONTAL_ALIGNMENT_LEFT,
        -1.0f,
        fontsize2,
        YELLOW_C
    );
  }

  // 7. Pause Dim Overlay (matches Call_Of_Pyray.py line 331)
  if (paused) {
    draw_rect(Rect2(0.0f, 0.0f, WIDTH, HEIGHT), Color(0.0f, 0.0f, 0.0f, 150.0f / 255.0f), true);
  }

  // 8. Top-Left Telemetry HUD Box
  draw_rect(Rect2(0.0f, 0.0f, 380.0f, 185.0f), Color(0.0f, 0.0f, 0.0f, 150.0f / 255.0f), true);

  int current_fps = 0;
  Engine *engine_ptr = Engine::get_singleton();
  if (engine_ptr != nullptr) {
    Engine &engine = *engine_ptr;
    current_fps = static_cast<int>(engine.get_frames_per_second());
  }

  std::vector<String> hud_lines = {
      text_format("FPS: %d", current_fps),
      text_format("Enemies: %d", static_cast<int>(enemies.size())),
      text_format("Fire Rates - Rifle: %.1f | Shotgun: %.1f", player.r_fire_rate, player.s_fire_rate),
      text_format("Seeking (A): %s", player.seeking ? "ON" : "OFF"),
      text_format("Frame: %.1f ms", last_frame_ms),
      text_format("Bullets: %d", static_cast<int>(active_bullets.size())),
      text_format("Mag: %d", player.mag),
      text_format("API: %s", graphics_api_version.c_str())
  };

  for (size_t i = 0; i < hud_lines.size(); i++) {
    float x_pos = (i == 6) ? 200.0f : 10.0f;
    float y_pos = (i == 6) ? (5.0f + fontsize) : (5.0f + static_cast<float>(i * 23 + fontsize));
    draw_string(
        default_font,
        Vector2(x_pos, y_pos),
        hud_lines[i],
        HORIZONTAL_ALIGNMENT_LEFT,
        -1.0f,
        fontsize,
        WHITE_C
    );
  }
}

void CallOfGodyArena::_bind_methods() {}

} // namespace godot
