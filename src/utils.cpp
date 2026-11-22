/******************************************************************************
 * File: src/utils.cpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * Implements vector math, runtime texture-atlas stitching, millisecond timers,
 * string formatting, and the 2-map (`inputs` + `released`) input controller.
 ******************************************************************************/

#include "utils.hpp"

#include <godot_cpp/classes/audio_stream_ogg_vorbis.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/rect2i.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>

// ============================================================================
// Global Asset Storage (Reference-Counted Godot Handles)
// ============================================================================
godot::Ref<godot::AudioStream> auto_fire;
godot::Ref<godot::AudioStream> single_shot;
godot::Ref<godot::AudioStream> shotgun;
godot::Ref<godot::AudioStream> BGM_music;
godot::Ref<godot::AudioStream> zombie_death;
godot::Ref<godot::AudioStream> empty_mag;

godot::Ref<godot::Texture2D> zombie_atlas;
godot::Rect2 zombie_rects[2];
godot::Ref<godot::Texture2D> player_sprite;
godot::Ref<godot::Texture2D> Gun;
godot::Ref<godot::Texture2D> ammo_box;

// ============================================================================
// 1. Vector, Geometry & Formatting Helper Implementations
// ============================================================================

godot::Vector2 rotate_pt(godot::Vector2 center, float angle, float length) {
  float rad = godot::Math::deg_to_rad(angle);
  float x = center.x + std::sin(rad) * length;
  float y = center.y - std::cos(rad) * length;
  return godot::Vector2(x, y);
}

godot::Vector2 move_pt(godot::Vector2 point, float movement, float angle) {
  float rad = godot::Math::deg_to_rad(angle);
  float x = point.x + movement * std::sin(rad);
  float y = point.y - movement * std::cos(rad);
  return godot::Vector2(x, y);
}

godot::Vector2 chase(godot::Vector2 target_pos, godot::Vector2 entity_pos, float movement) {
  float dx = target_pos.x - entity_pos.x;
  float dy = target_pos.y - entity_pos.y;
  float dist = std::hypot(dx, dy);
  if (dist <= 1.0f) {
    return entity_pos;
  }
  return godot::Vector2(
      entity_pos.x + (dx / dist) * movement,
      entity_pos.y + (dy / dist) * movement
  );
}

float two_points_distance(godot::Vector2 p1, godot::Vector2 p2) {
  return p1.distance_to(p2);
}

bool out_of_range(godot::Vector2 position, float limit_x, float limit_y) {
  return (position.x > limit_x || position.x < 0.0f ||
          position.y > limit_y || position.y < 0.0f);
}

bool distance_trigger(godot::Vector2 target_pos, godot::Vector2 entity_pos, float trigger_distance) {
  // Fast squared-distance check: avoids a costly square root operation.
  float distance_squared = target_pos.distance_squared_to(entity_pos);
  return distance_squared < (trigger_distance * trigger_distance);
}

int get_random_value(float min_val, float max_val) {
  int low = static_cast<int>(min_val);
  int high = static_cast<int>(max_val);
  return static_cast<int>(godot::UtilityFunctions::randi_range(low, high));
}

int random_sprite() {
  return get_random_value(0.0f, 1.0f);
}

bool happened(float chance) {
  int test = static_cast<int>(chance * 100.0f);
  return test >= get_random_value(1.0f, 10000.0f);
}

godot::String text_format(const char *fmt, ...) {
  char buffer[256];
  va_list args;
  va_start(args, fmt);
  std::vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  return godot::String(buffer);
}

// ============================================================================
// 2. Internal Helper Functions for Asset Loading (Pure Dot `.` Syntax)
// ============================================================================

static godot::Ref<godot::Texture2D> load_texture_asset(const char *res_path) {
  godot::ResourceLoader *loader_ptr = godot::ResourceLoader::get_singleton();
  if (loader_ptr != nullptr) {
    godot::ResourceLoader &loader = *loader_ptr;
    if (loader.exists(godot::String(res_path))) {
      godot::Ref<godot::Texture2D> imported_tex = loader.load(godot::String(res_path));
      if (imported_tex.is_valid()) {
        return imported_tex;
      }
    }
  }

  godot::Ref<godot::Image> img = godot::Image::load_from_file(godot::String(res_path));
  if (img.is_valid()) {
    return godot::ImageTexture::create_from_image(img);
  }
  return godot::Ref<godot::Texture2D>();
}

static godot::Ref<godot::Image> load_image_from_texture_asset(const char *res_path) {
  godot::Ref<godot::Texture2D> tex_ref = load_texture_asset(res_path);
  if (tex_ref.is_valid()) {
    const godot::Texture2D &tex = *tex_ref.ptr();
    return tex.get_image();
  }
  return godot::Image::load_from_file(godot::String(res_path));
}

static godot::Ref<godot::AudioStream> load_ogg_asset(const char *res_path) {
  godot::ResourceLoader *loader_ptr = godot::ResourceLoader::get_singleton();
  if (loader_ptr != nullptr) {
    godot::ResourceLoader &loader = *loader_ptr;
    if (loader.exists(godot::String(res_path))) {
      godot::Ref<godot::AudioStream> imported_audio = loader.load(godot::String(res_path));
      if (imported_audio.is_valid()) {
        return imported_audio;
      }
    }
  }
  return godot::AudioStreamOggVorbis::load_from_file(godot::String(res_path));
}

void load_assets() {
  auto_fire    = load_ogg_asset("res://assets/Game_machine_gun.ogg");
  single_shot  = load_ogg_asset("res://assets/Gun_shot.ogg");
  shotgun      = load_ogg_asset("res://assets/shotgun_echo.ogg");
  BGM_music    = load_ogg_asset("res://assets/Dark_future.ogg");
  zombie_death = load_ogg_asset("res://assets/Zombie_Killed.ogg");
  empty_mag    = load_ogg_asset("res://assets/Gun_Clipping.ogg");

  // Stitch the 2 zombie faces into a single GPU texture atlas for batched drawing
  godot::Ref<godot::Image> z1_ref = load_image_from_texture_asset("res://assets/zombie_face_male.png");
  godot::Ref<godot::Image> z2_ref = load_image_from_texture_asset("res://assets/zombie_face_female.png");

  if (z1_ref.is_valid() && z2_ref.is_valid()) {
    godot::Image &z1 = *z1_ref.ptr();
    godot::Image &z2 = *z2_ref.ptr();

    if (z1.get_format() != godot::Image::FORMAT_RGBA8) {
      z1.convert(godot::Image::FORMAT_RGBA8);
    }
    if (z2.get_format() != godot::Image::FORMAT_RGBA8) {
      z2.convert(godot::Image::FORMAT_RGBA8);
    }

    int z1_w = z1.get_width();
    int z1_h = z1.get_height();
    int z2_w = z2.get_width();
    int z2_h = z2.get_height();

    int atlas_w = z1_w + z2_w;
    int atlas_h = std::max(z1_h, z2_h);

    godot::Ref<godot::Image> atlas_img_ref =
        godot::Image::create_empty(atlas_w, atlas_h, false, godot::Image::FORMAT_RGBA8);

    if (atlas_img_ref.is_valid()) {
      godot::Image &atlas_img = *atlas_img_ref.ptr();
      atlas_img.fill(godot::Color(0.0f, 0.0f, 0.0f, 0.0f));

      zombie_rects[0] = godot::Rect2(0.0f, 0.0f, static_cast<float>(z1_w), static_cast<float>(z1_h));
      zombie_rects[1] = godot::Rect2(static_cast<float>(z1_w), 0.0f, static_cast<float>(z2_w), static_cast<float>(z2_h));

      atlas_img.blit_rect(z1_ref, godot::Rect2i(0, 0, z1_w, z1_h), godot::Vector2i(0, 0));
      atlas_img.blit_rect(z2_ref, godot::Rect2i(0, 0, z2_w, z2_h), godot::Vector2i(z1_w, 0));

      zombie_atlas = godot::ImageTexture::create_from_image(atlas_img_ref);
    }
  }

  ammo_box      = load_texture_asset("res://assets/Ammo.png");
  player_sprite = load_texture_asset("res://assets/player.png");
  Gun           = load_texture_asset("res://assets/Gun.png");
}

void unload_assets() {
  auto_fire.unref();
  single_shot.unref();
  shotgun.unref();
  BGM_music.unref();
  zombie_death.unref();
  empty_mag.unref();

  zombie_atlas.unref();
  ammo_box.unref();
  player_sprite.unref();
  Gun.unref();
}

// ============================================================================
// 3. Millisecond Accumulator Timer (TIMER)
// ============================================================================

TIMER::TIMER(float t) {
  time = 0.0f;
  trigger = t;
  limit = std::max(1000.0f, t);
}

void TIMER::update(float dt) {
  if (time <= std::max(limit, trigger)) {
    time += dt;
  }
}

bool TIMER::ready() const {
  return time >= trigger;
}

void TIMER::reset() {
  time = 0.0f;
}

// ============================================================================
// 4. Input & Action Mapping Controller (CONTROLS)
// ============================================================================

CONTROLS::CONTROLS() {
  actions = {"forwards", "backwards", "turn_l", "turn_r", "fr+",    "fr-",
             "seek",     "shotgun",   "rifle",  "reset",  "paused", "muted",
             "QUIT"};
  for (const auto &action : actions) {
    inputs[action] = false;
    released[action] = false;
  }
}

void CONTROLS::get_input() {
  godot::Input *input_ptr = godot::Input::get_singleton();
  if (input_ptr == nullptr) {
    return;
  }
  godot::Input &input = *input_ptr;

  if (input.is_key_pressed(godot::KEY_LEFT)) {
    inputs["turn_l"] = true;
  }
  if (input.is_key_pressed(godot::KEY_RIGHT)) {
    inputs["turn_r"] = true;
  }
  if (input.is_key_pressed(godot::KEY_UP)) {
    inputs["forwards"] = true;
  }
  if (input.is_key_pressed(godot::KEY_DOWN)) {
    inputs["backwards"] = true;
  }
  if (input.is_key_pressed(godot::KEY_KP_ADD) ||
      input.is_key_pressed(godot::KEY_EQUAL) ||
      input.is_key_pressed(godot::KEY_PLUS)) {
    inputs["fr+"] = true;
  }
  if (input.is_key_pressed(godot::KEY_KP_SUBTRACT) ||
      input.is_key_pressed(godot::KEY_MINUS)) {
    inputs["fr-"] = true;
  }
  if (input.is_key_pressed(godot::KEY_A)) {
    inputs["seek"] = true;
  }
  if (input.is_key_pressed(godot::KEY_S)) {
    inputs["shotgun"] = true;
  }
  if (input.is_key_pressed(godot::KEY_F)) {
    inputs["rifle"] = true;
  }
  if (input.is_key_pressed(godot::KEY_R)) {
    inputs["reset"] = true;
  }
  if (input.is_key_pressed(godot::KEY_P)) {
    inputs["paused"] = true;
  }
  if (input.is_key_pressed(godot::KEY_M)) {
    inputs["muted"] = true;
  }
  if (input.is_key_pressed(godot::KEY_Q)) {
    inputs["QUIT"] = true;
  }

  // `released[action]` is true only on the exact frame a key transitions from DOWN to UP
  for (const auto &action : actions) {
    released[action] = released[action] && !inputs[action];
  }
}

void CONTROLS::reset() {
  for (const auto &action : actions) {
    released[action] = inputs[action];
    inputs[action] = false;
  }
}
