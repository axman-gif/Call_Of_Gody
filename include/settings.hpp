/******************************************************************************
 * File: include/settings.hpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * In the original "CallOfRay" (raylib) project, this file declared the global
 * screen dimensions, the RGB color palette, and the default gameplay tuning
 * numbers (starting health, zombie count, font sizes, and drop probability).
 *
 * In Godot 4.7 C++, we replace raylib's 0-255 integer `Color` struct with
 * Godot's floating-point `godot::Color(r, g, b, a)` class, where each channel
 * normally ranges from 0.0f to 1.0f (or can be constructed from 0-255 bytes
 * via `godot::Color::hex()` or dividing by 255.0f).
 *
 * OFFICIAL GODOT API REFERENCES:
 * - godot::Color Class Reference:
 *   https://docs.godotengine.org/en/stable/classes/class_color.html
 *
 * KEYBOARD RULE COMPLIANCE:
 * - Zero occurrences of the forbidden operator (see project rules).
 * - Prioritizes explicit, human-readable beginner-friendly C++17 syntax.
 ******************************************************************************/

#pragma once

#include <godot_cpp/variant/color.hpp>

// ============================================================================
// 1. Environment & Viewport Configurations
// ============================================================================
// Default viewport width and height in pixels (updated dynamically every frame
// when the user resizes the Godot window).
extern float WIDTH;
extern float HEIGHT;
extern int FPS;

// ============================================================================
// 2. Color Palette (Godot Linear/sRGB Floating-Point RGBA Colors)
// ============================================================================
// Official API: https://docs.godotengine.org/en/stable/classes/class_color.html
extern const godot::Color WHITE_C;
extern const godot::Color PINK_C;
extern const godot::Color GREEN_C;
extern const godot::Color BLACK_C;
extern const godot::Color RED_C;
extern const godot::Color YELLOW_C;
extern const godot::Color BLUE_C;
extern const godot::Color PURPLE_C;

// Array of 5 skin tone colors used to draw the survivor's base circle
extern const godot::Color SKIN_COLORS[5];

// ============================================================================
// 3. Gameplay & Typography Parameters
// ============================================================================
// Font sizes for HUD telemetry (`fontsize`) and center-screen alerts (`fontsize2`).
extern int fontsize;
extern int fontsize2;

// Base radius in pixels for both the player survivor and zombie entities.
// Named `my_size` just like in CallOfRay to avoid collisions with `std::size`.
extern int my_size;

// ============================================================================
// 4. Enemy Wave Configurations
// ============================================================================
extern int starting_life;
extern int starting_zombies;

// ============================================================================
// 5. System & Combat Tuning Flags
// ============================================================================
extern float global_angle;
extern float angle_speed;
extern bool game_over;

// Probability percentage (e.g. 7.0f = 7% chance) for a defeated zombie to drop
// an ammunition crate.
extern float chance;
