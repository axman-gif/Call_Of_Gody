/******************************************************************************
 * File: include/register_types.hpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * Every Godot 4 C++ GDExtension library must define an initialization and
 * termination callback so Godot knows when to register our custom C++ classes
 * (such as `CallOfGodyArena`) into Godot's `ClassDB`.
 *
 * OFFICIAL GODOT API REFERENCES:
 * - GDExtension C++ Example (Registering Types):
 *   https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html
 ******************************************************************************/

#pragma once

#include <godot_cpp/core/class_db.hpp>

void initialize_call_of_gody_module(godot::ModuleInitializationLevel p_level);
void uninitialize_call_of_gody_module(godot::ModuleInitializationLevel p_level);
