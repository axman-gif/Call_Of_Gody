/******************************************************************************
 * File: src/register_types.cpp
 * Project: Call_Of_Gody (Godot 4.7 C++17 GDExtension / Vulkan Forward+)
 *
 * PEDAGOGICAL OVERVIEW:
 * ---------------------
 * This file is the C++ entry point (`call_of_gody_library_init`) that Godot 4.7
 * calls when loading our compiled shared library (`.dll` / `.so` / `.dylib`)
 * declared in `call_of_gody.gdextension`.
 *
 * Notice how `init_obj.register_initializer(...)`, `init_obj.register_terminator(...)`,
 * and `init_obj.init()` all naturally use the dot (`.`) operator!
 *
 * OFFICIAL GODOT API REFERENCES:
 * - GDExtension C++ Registration Lifecycle:
 *   https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html#using-the-gdextension-module
 ******************************************************************************/

#include "register_types.hpp"
#include "arena.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

void initialize_call_of_gody_module(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Registers our custom C++ Node2D subclass `CallOfGodyArena` with Godot's
    // ClassDB so it appears in the "Create New Node" dialog and in `main.tscn`!
    GDREGISTER_CLASS(godot::CallOfGodyArena);
}

void uninitialize_call_of_gody_module(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Entry point symbol referenced by `entry_symbol = "call_of_gody_library_init"`
// inside `A:\Call_Of_Gody\call_of_gody.gdextension`.
GDExtensionBool GDE_EXPORT call_of_gody_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization) {

    godot::GDExtensionBinding::InitObject init_obj(
        p_get_proc_address,
        p_library,
        r_initialization
    );

    init_obj.register_initializer(initialize_call_of_gody_module);
    init_obj.register_terminator(uninitialize_call_of_gody_module);
    init_obj.set_minimum_library_initialization_level(
        godot::MODULE_INITIALIZATION_LEVEL_SCENE
    );

    return init_obj.init();
}
}
