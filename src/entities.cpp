/******************************************************************************
 * File: src/entities.cpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * Implements `Bullet`, `Ammo`, `Enemy`, `Character`, and `SFX` mirroring
 * `Call_Of_Pyray` (`entities.py`) and `CallOfRay` (`entities.cpp`).
 ******************************************************************************/

#include "entities.hpp"
#include "settings.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cmath>
#include <string>

// ============================================================================
// 1. Bullet Implementation
// ============================================================================

Bullet::Bullet(godot::Vector2 pos, float ang, bool seek, int dmg) {
  position = pos;
  angle = ang;
  seeking = seek;
  size_r = 5;
  damage = dmg;
  speed = two_points_distance(godot::Vector2(0.0f, 0.0f), godot::Vector2(WIDTH, HEIGHT));
  range = static_cast<float>(6 * my_size);
}

void Bullet::move(float step) {
  position = move_pt(position, step, angle);
}

void Bullet::chasing(godot::Vector2 target_pos, float step) {
  if (distance_trigger(position, target_pos, range)) {
    position = chase(target_pos, position, step);
  } else {
    position = move_pt(position, step, angle);
  }
}

bool Bullet::collision(godot::Vector2 target_pos, float target_size) const {
  return distance_trigger(position, target_pos, static_cast<float>(size_r) + target_size);
}

void Bullet::draw(godot::Node2D &canvas) const {
  canvas.draw_circle(position, static_cast<float>(size_r), YELLOW_C);
}

// ============================================================================
// 2. Ammo Crate Implementation
// ============================================================================

Ammo::Ammo(godot::Vector2 pos, int m, int s, std::string t) {
  position = pos;
  mag = m;
  size_r = s;
  type = t;
  speed = 500.0f;
}

void Ammo::chasing(godot::Vector2 player_pos, float step, float magnet_radius) {
  float attract_range = static_cast<float>(size_r + my_size) + magnet_radius;
  if (distance_trigger(position, player_pos, attract_range)) {
    position = chase(player_pos, position, step);
  }
}

bool Ammo::collision(godot::Vector2 player_pos, float player_size) const {
  return distance_trigger(position, player_pos, static_cast<float>(size_r) + player_size);
}

void Ammo::draw(godot::Node2D &canvas) const {
  if (ammo_box.is_valid()) {
    godot::Vector2 draw_pos(
        position.x - static_cast<float>(size_r),
        position.y - static_cast<float>(size_r)
    );
    canvas.draw_texture(ammo_box, draw_pos, WHITE_C);
  } else {
    canvas.draw_circle(position, static_cast<float>(size_r), BLUE_C);
  }
}

// ============================================================================
// 3. Enemy (Zombie) Implementation
// ============================================================================

Enemy::Enemy(godot::Vector2 enemy_pos, int s, int l, int spr) {
  position = enemy_pos;
  size_r = s;
  life = l;
  sprite_idx = spr;
  color = GREEN_C;
  speed = 150.0f;
  range = 200.0f;
}

void Enemy::chasing(godot::Vector2 player_pos, float step) {
  if (distance_trigger(player_pos, position, range)) {
    position = chase(player_pos, position, step);
  }
}

bool Enemy::collision(godot::Vector2 target_pos, float target_size) const {
  return distance_trigger(target_pos, position, static_cast<float>(size_r) + target_size);
}

void Enemy::draw(godot::Node2D &canvas) const {
  if (zombie_atlas.is_valid()) {
    godot::Rect2 src_rect = zombie_rects[sprite_idx];
    godot::Rect2 dest_rect(
        position.x - static_cast<float>(size_r),
        position.y - static_cast<float>(size_r),
        src_rect.size.x,
        src_rect.size.y
    );
    canvas.draw_texture_rect_region(zombie_atlas, dest_rect, src_rect, WHITE_C);
  } else {
    canvas.draw_circle(position, static_cast<float>(size_r), color);
  }
}

void Enemy::draw_label(godot::Node2D &canvas, const godot::Ref<godot::Font> &font) const {
  if (font.is_null()) {
    return;
  }

  const godot::Font &font_obj = *font.ptr();
  godot::String label = godot::String::num_int64(life);
  godot::Vector2 text_size = font_obj.get_string_size(
      label,
      godot::HORIZONTAL_ALIGNMENT_LEFT,
      -1.0f,
      fontsize
  );

  godot::Vector2 text_pos(
      position.x - (text_size.x / 2.0f),
      position.y + static_cast<float>(size_r + 5 + fontsize)
  );

  canvas.draw_string(
      font,
      text_pos,
      label,
      godot::HORIZONTAL_ALIGNMENT_LEFT,
      -1.0f,
      fontsize,
      YELLOW_C
  );
}

// ============================================================================
// 4. Character (Player Survivor) Implementation
// ============================================================================

Character::Character(godot::Vector2 pos) {
  position = pos;
  angle = 0.0f;
  sight = rotate_pt(position, angle, static_cast<float>(my_size * 2));
  speed = 300.0f;
  mag = 1000;
  magnet = 100;

  r_fire_rate = 10.0f;
  rifle_timer = TIMER(1000.0f / r_fire_rate);
  rifle_timer.time = rifle_timer.trigger;
  rifle_accuracy = 2;

  s_fire_rate = 0.5f;
  shotgun_timer = TIMER(1000.0f / s_fire_rate);
  shotgun_timer.time = shotgun_timer.trigger;
  shotgun_accuracy = 6;

  seeking = false;
  actions = CONTROLS();
}

void Character::update(float frametime) {
  rifle_timer.update(frametime);
  shotgun_timer.update(frametime);
}

void Character::draw(godot::Node2D &canvas, float w, float h) const {
  godot::Vector2 direction = rotate_pt(position, angle, std::hypot(w, h));

  canvas.draw_circle(position, static_cast<float>(my_size), SKIN_COLORS[2]);
  canvas.draw_line(position, direction, BLACK_C, 2.0f);

  if (player_sprite.is_valid()) {
    godot::Vector2 sprite_pos(
        position.x - static_cast<float>(my_size),
        position.y - static_cast<float>(my_size)
    );
    canvas.draw_texture(player_sprite, sprite_pos, WHITE_C);
  }
}

void Character::handle_input(float frame,
                             std::unordered_map<std::string, bool> &playing,
                             std::vector<Bullet> &active_bullets) {
  // 1. Rotation (Left / Right Arrow Keys)
  if (actions.inputs["turn_l"] && !actions.inputs["turn_r"]) {
    angle -= angle_speed * (frame / 1000.0f);
    angle = std::fmod(angle, 360.0f);
    sight = rotate_pt(position, angle, static_cast<float>(my_size * 2));
  }
  if (actions.inputs["turn_r"] && !actions.inputs["turn_l"]) {
    angle += angle_speed * (frame / 1000.0f);
    angle = std::fmod(angle, 360.0f);
    sight = rotate_pt(position, angle, static_cast<float>(my_size * 2));
  }

  // 2. Movement Forward / Backward (Up / Down Arrow Keys)
  if (actions.inputs["forwards"]) {
    position = move_pt(position, speed * (frame / 1000.0f), angle);
    sight = rotate_pt(position, angle, static_cast<float>(my_size * 2));
  }
  if (actions.inputs["backwards"]) {
    position = move_pt(position, -speed * (frame / 1000.0f), angle);
    sight = rotate_pt(position, angle, static_cast<float>(my_size * 2));
  }

  // 3. Toggle Homing / Seeking Bullets (Released 'A' key)
  if (actions.released["seek"]) {
    seeking = !seeking;
  }

  // 4. Fire Assault Rifle / Pistol (Hold 'F' key)
  if (actions.inputs["rifle"] && rifle_timer.ready() && mag > 0) {
    mag -= 1;
    if (r_fire_rate > 5.0f) {
      playing["rifle"] = true;
    } else {
      playing["pistol"] = true;
    }

    float spread = static_cast<float>(
        get_random_value(-3.0f * rifle_accuracy * 100.0f, 3.0f * rifle_accuracy * 100.0f)
    ) / 100.0f;
    active_bullets.push_back(Bullet(sight, angle + spread, seeking));
    rifle_timer.reset();
  }

  // 5. Fire Shotgun (Hold 'S' key: 5 pellets)
  if (actions.inputs["shotgun"] && shotgun_timer.ready() && mag > 0) {
    playing["shotgun"] = true;
    for (int n = 0; n < 5; n++) {
      if (mag <= 0) {
        break;
      }
      mag -= 1;
      float spread = static_cast<float>(
          get_random_value(-3.0f * shotgun_accuracy * 100.0f, 3.0f * shotgun_accuracy * 100.0f)
      ) / 100.0f;
      active_bullets.push_back(Bullet(sight, angle + spread, seeking));
    }
    shotgun_timer.reset();
  }

  // 6. Empty Magazine Clicking Sound
  if ((actions.inputs["rifle"] || actions.inputs["shotgun"]) && mag <= 0) {
    playing["clipping"] = true;
  }

  // 7. Live Fire-Rate Adjustments (`F + +`, `F + -`, `S + +`, `S + -`)
  if (actions.inputs["rifle"] && actions.inputs["fr+"]) {
    r_fire_rate += 1.0f;
    rifle_timer.trigger = 1000.0f / r_fire_rate;
  }
  if (actions.inputs["rifle"] && actions.inputs["fr-"] && r_fire_rate > 1.0f) {
    r_fire_rate -= 1.0f;
    rifle_timer.trigger = 1000.0f / r_fire_rate;
  }
  if (actions.inputs["shotgun"] && actions.inputs["fr+"]) {
    s_fire_rate += 0.1f;
    shotgun_timer.trigger = 1000.0f / s_fire_rate;
  }
  if (actions.inputs["shotgun"] && actions.inputs["fr-"] && s_fire_rate > 0.1f) {
    s_fire_rate -= 0.1f;
    shotgun_timer.trigger = 1000.0f / s_fire_rate;
  }

  // 8. Clamp player position inside the screen boundaries.
  position.x = std::max(0.0f, std::min(position.x, WIDTH));
  position.y = std::max(0.0f, std::min(position.y, HEIGHT));
}

// ============================================================================
// 5. Dynamic Sound Effect Controller (SFX) Implementation
// ============================================================================

SFX::SFX(const godot::Ref<godot::AudioStream> &s,
         godot::AudioStreamPlayer *node,
         float del,
         float len) {
  soundtrack = s;
  player_node = node;
  delay = del;
  length = len;
  timer = TIMER(del);
  timer.time = del;
  timer.limit = std::max(1000.0f, len);
  playing = false;

  if (player_node != nullptr && soundtrack.is_valid()) {
    godot::AudioStreamPlayer &audio_player = *player_node;
    audio_player.set_stream(soundtrack);
  }
}

void SFX::update(float frame) {
  timer.trigger = delay;
  timer.update(frame);
}

void SFX::play_sfx() {
  timer.trigger = delay;
  if (timer.ready()) {
    if (player_node != nullptr && soundtrack.is_valid()) {
      godot::AudioStreamPlayer &audio_player = *player_node;
      audio_player.play();
    }
    timer.reset();
  }
}

void SFX::stop(float max_time) {
  if (player_node == nullptr) {
    return;
  }
  godot::AudioStreamPlayer &audio_player = *player_node;

  if (max_time > 0.0f && timer.time >= max_time && audio_player.is_playing()) {
    audio_player.stop();
    timer.reset();
  } else if (max_time == 0.0f && audio_player.is_playing()) {
    audio_player.stop();
  }
}
