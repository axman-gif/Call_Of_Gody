# ==============================================================================
#                   PROJECT: Call_Of_Gody (Godot 4.7 C++17 / Vulkan)
# ==============================================================================
#
#  Welcome to the CMake build configuration file (CMakeLists.txt) for Call_Of_Gody!
#  This file teaches your computer how to compile your C++17 game code into a
#  native Godot 4.7 GDExtension dynamic library (.dll / .so / .dylib) powered
#  by the Vulkan RenderingDevice backend.
#
#  OFFICIAL DOCUMENTATION REFERENCES:
#  - Godot C++ (GDExtension) CMake Build System:
#    https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html
#  - godot-cpp Official Repository:
#    https://github.com/godotengine/godot-cpp
#
#  QUICK BEGINNER GUIDE:
#  -----------------------------------------------------------------------------
#  1. Configure the project (generates Ninja/Make build files in 'build/'):
#       cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
#
#  2. Compile the C++ GDExtension library:
#       cmake --build build
#
#  3. Open or Run in Godot 4.7 (Vulkan Forward+):
#       Godot Editor : Godot_v4.7.2-stable_win64.exe --path A:/Call_Of_Gody --editor
#       Play Directly: Godot_v4.7.2-stable_win64.exe --path A:/Call_Of_Gody
# ==============================================================================

cmake_minimum_required(VERSION 3.17)

# Set default build type to Release if not specified by the user
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

# Target the exact Godot 4.7 API dumped from our Godot 4.7.2 stable engine
set(GODOTCPP_API_VERSION "4.7" CACHE STRING "Godot API version" FORCE)
set(GODOTCPP_CUSTOM_API_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/extension_api.json" CACHE FILEPATH "Custom Godot 4.7.2 API JSON" FORCE)
set(GODOTCPP_TARGET "template_debug" CACHE STRING "GDExtension target type" FORCE)

# We use compile_flags.txt at the repo root for 100% path-agnostic IDE & clangd
# support (zero hardcoded A:/ or C:/ drive letters, and instant support for new .hpp/.cpp files)
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

# Define project name and language (CXX = C++)
project(Call_Of_Gody VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "==============================================================")
message(STATUS " [Call_Of_Gody] Configuring Godot 4.7 C++17 Vulkan Extension")
message(STATUS "==============================================================")

# ==============================================================================
# STEP 1: Load or Fetch Official 'godot-cpp' Bindings
# ==============================================================================
# If 'build/godot-cpp-src' already exists locally, we use it directly for speed.
# Otherwise, CMake FetchContent downloads it automatically from GitHub!
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/godot-cpp-src/CMakeLists.txt")
    message(STATUS "[Call_Of_Gody] Using local godot-cpp source in build/godot-cpp-src")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/build/godot-cpp-src" "${CMAKE_CURRENT_BINARY_DIR}/godot-cpp-build")
else()
    include(FetchContent)
    set(FETCHCONTENT_QUIET FALSE)
    FetchContent_Declare(
        godot-cpp
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(godot-cpp)
endif()

# ==============================================================================
# STEP 2: Define the Call_Of_Gody Shared Library Target (.dll / .so / .dylib)
# ==============================================================================
set(PROJECT_TARGET "call_of_gody")

# Automatically discover ALL .cpp files in src/ and .hpp files in include/
# so creating a new file in src/ or include/ never requires editing CMakeLists.txt!
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_library(${PROJECT_TARGET} SHARED
    ${PROJECT_SOURCES}
)

# Tell the C++ compiler where to find our pedagogical header files (.hpp)
target_include_directories(${PROJECT_TARGET} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link our library against the official Godot C++ bindings
target_link_libraries(${PROJECT_TARGET} PUBLIC godot-cpp)

# On MinGW (GCC on Windows), statically link libgcc, libstdc++, and winpthread
# so the resulting .dll has zero external MinGW runtime DLL dependencies!
if(MINGW)
    target_link_options(${PROJECT_TARGET} PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# Place the compiled shared library directly into 'bin/' where Godot expects it
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_target_properties(${PROJECT_TARGET} PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
    PREFIX "lib"
    OUTPUT_NAME "call_of_gody"
    LIBRARY_OUTPUT_DIRECTORY "$<1:${OUTPUT_DIR}>"
    RUNTIME_OUTPUT_DIRECTORY "$<1:${OUTPUT_DIR}>"
)
