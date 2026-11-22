/******************************************************************************
 * File: include/utils.hpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * Mathematical helpers, asset loaders, millisecond timers, and 2-map keyboard
 * input controller (`inputs` + `released`) structured to mirror `Call_Of_Pyray`
 * and `CallOfRay`.
 ******************************************************************************/

#pragma once

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector2.hpp>

#include <string>
#include <unordered_map>
#include <vector>

// ============================================================================
// 1. Vector, Geometry & Formatting Helper Functions
// ============================================================================

godot::Vector2 rotate_pt(godot::Vector2 center, float angle, float length);
godot::Vector2 move_pt(godot::Vector2 point, float movement, float angle);
godot::Vector2 chase(godot::Vector2 target_pos, godot::Vector2 entity_pos, float movement);
float two_points_distance(godot::Vector2 p1, godot::Vector2 p2);
bool out_of_range(godot::Vector2 position, float limit_x, float limit_y);
bool distance_trigger(godot::Vector2 target_pos, godot::Vector2 entity_pos, float trigger_distance);

int random_sprite();
bool happened(float chance);
int get_random_value(float min_val, float max_val);

// Formats a text string just like raylib's `TextFormat(...)` or Python f-strings.
godot::String text_format(const char *fmt, ...);

// ============================================================================
// 2. Preloaded Game Assets (Textures & Audio Streams)
// ============================================================================

extern godot::Ref<godot::AudioStream> auto_fire;
extern godot::Ref<godot::AudioStream> single_shot;
extern godot::Ref<godot::AudioStream> shotgun;
extern godot::Ref<godot::AudioStream> BGM_music;
extern godot::Ref<godot::AudioStream> zombie_death;
extern godot::Ref<godot::AudioStream> empty_mag;

extern godot::Ref<godot::Texture2D> zombie_atlas;
extern godot::Rect2 zombie_rects[2];
extern godot::Ref<godot::Texture2D> player_sprite;
extern godot::Ref<godot::Texture2D> Gun;
extern godot::Ref<godot::Texture2D> ammo_box;

void load_assets();
void unload_assets();

// ============================================================================
// 3. Millisecond Accumulator Timer (TIMER)
// ============================================================================
class TIMER {
public:
  float time = 0.0f;
  float trigger = 0.0f;
  float limit = 1000.0f;

  TIMER() = default;
  explicit TIMER(float t);
  void update(float dt);
  bool ready() const;
  void reset();
};

// ============================================================================
// 4. Input & Action Mapping Controller (CONTROLS)
// ============================================================================
class CONTROLS {
public:
  std::vector<std::string> actions;
  std::unordered_map<std::string, bool> inputs;
  std::unordered_map<std::string, bool> released;

  CONTROLS();
  void get_input();
  void reset();
};
