/******************************************************************************
 * File: src/settings.cpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * This source file initializes all global configuration constants declared in
 * `include/settings.hpp`.
 *
 * HOW GODOT COLORS WORK vs. RAYLIB COLORS:
 * - In raylib, `Color` uses 8-bit unsigned integers `{255, 220, 0, 255}`.
 * - In Godot's C++ API (`godot::Color`), each channel (`r`, `g`, `b`, `a`) is
 *   a normalized 32-bit float between `0.0f` and `1.0f`.
 * - To keep the exact visual appearance of CallOfRay while teaching how the
 *   conversion works, we divide each original 0-255 byte value by `255.0f`!
 *
 * OFFICIAL GODOT API REFERENCES:
 * - godot::Color Constructor Reference:
 *   https://docs.godotengine.org/en/stable/classes/class_color.html#constructors
 ******************************************************************************/

#include "settings.hpp"

// --- Environment Configurations ---
float WIDTH = 1280.0f;
float HEIGHT = 720.0f;
int FPS = 0; // 0 = uncapped frame rate

// --- Color Palette (Normalized RGBA floats converted from 0..255) ---
const godot::Color WHITE_C  = godot::Color(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);
const godot::Color PINK_C   = godot::Color(255.0f / 255.0f, 150.0f / 255.0f, 255.0f / 255.0f, 1.0f);
const godot::Color GREEN_C  = godot::Color(0.0f   / 255.0f, 150.0f / 255.0f, 0.0f   / 255.0f, 1.0f);
const godot::Color BLACK_C  = godot::Color(0.0f   / 255.0f, 0.0f   / 255.0f, 0.0f   / 255.0f, 1.0f);
const godot::Color RED_C    = godot::Color(255.0f / 255.0f, 0.0f   / 255.0f, 0.0f   / 255.0f, 1.0f);
const godot::Color YELLOW_C = godot::Color(255.0f / 255.0f, 220.0f / 255.0f, 0.0f   / 255.0f, 1.0f);
const godot::Color BLUE_C   = godot::Color(0.0f   / 255.0f, 0.0f   / 255.0f, 255.0f / 255.0f, 1.0f);
const godot::Color PURPLE_C = godot::Color(255.0f / 255.0f, 0.0f   / 255.0f, 255.0f / 255.0f, 1.0f);

const godot::Color SKIN_COLORS[5] = {
    godot::Color(141.0f / 255.0f, 85.0f  / 255.0f, 36.0f  / 255.0f, 1.0f),
    godot::Color(198.0f / 255.0f, 134.0f / 255.0f, 66.0f  / 255.0f, 1.0f),
    godot::Color(224.0f / 255.0f, 172.0f / 255.0f, 105.0f / 255.0f, 1.0f),
    godot::Color(241.0f / 255.0f, 194.0f / 255.0f, 125.0f / 255.0f, 1.0f),
    godot::Color(255.0f / 255.0f, 219.0f / 255.0f, 172.0f / 255.0f, 1.0f)
};

// --- Gameplay Parameters ---
int fontsize = 14;
int fontsize2 = 18;
int my_size = 25;

// --- Enemy Configurations ---
int starting_life = 200;
int starting_zombies = 5;

// --- System & Initialization Flags ---
float global_angle = 0.0f;
float angle_speed = 180.0f;
bool game_over = false;
float chance = 7.0f;
