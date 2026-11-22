# 📘 The Godot 4.7 C++ Dual-Syntax Grimoire: Dot (`.`) vs. Arrow (`->`) 🧙‍♂️✨

<p align="center">
  <b>A Complete Pedagogical Reference to the Godot 4.7 C++ API (<code>godot-cpp</code>) Featuring Side-by-Side Syntax Comparisons for Every Engine Subsystem Used in <i>Call Of Gody</i>.</b>
</p>

<p align="center">
  <a href="./README.md">⬅️ Back to Main README.md</a>
</p>

---

## 🧭 Table of Contents

1. [Why Two Syntaxes? (The Philosophy Behind This Grimoire)](#-1-why-two-syntaxes-the-philosophy-behind-this-grimoire)
2. [Raylib & Python (`Call_Of_Pyray`) to Godot 4.7 C++ Rosetta Stone](#-2-raylib--python-call_of_pyray-to-godot-47-c-rosetta-stone)
3. [Core Rule: How to Turn Any Pointer or `Ref<T>` into Dot (`.`) Syntax](#-3-core-rule-how-to-turn-any-pointer-or-reft-into-dot--syntax)
4. [Subsystem 1: Engine Singletons (`Input`, `Engine`, `RenderingServer`, `ThemeDB`)](#-4-subsystem-1-engine-singletons-input-engine-renderingserver-themedb)
5. [Subsystem 2: Reference-Counted Resources (`Ref<Image>`, `Ref<Texture2D>`, `Ref<AudioStream>`)](#-5-subsystem-2-reference-counted-resources-refimage-reftexture2d-refaudiostream)
6. [Subsystem 3: Dynamic Texture Atlas Stitching (`Image::create_empty` & `Image::blit_rect`)](#-6-subsystem-3-dynamic-texture-atlas-stitching-imagecreate_empty--imageblit_rect)
7. [Subsystem 4: SceneTree Nodes & Memory Allocation (`memnew`, `add_child`, `AudioStreamPlayer`)](#-7-subsystem-4-scenetree-nodes--memory-allocation-memnew-add_child-audiostreamplayer)
8. [Subsystem 5: 2D Vulkan Canvas Drawing (`CanvasItem::_draw`, Transforms, Shapes & Text)](#-8-subsystem-5-2d-vulkan-canvas-drawing-canvasitem_draw-transforms-shapes--text)
9. [Subsystem 6: 2-Map Input Controller (`inputs` vs. `released`) & Entity Methods](#-9-subsystem-6-2-map-input-controller-inputs-vs-released--entity-methods)

---

## 💡 1. Why Two Syntaxes? (The Philosophy Behind This Grimoire)

In standard C++, whenever a function returns a **pointer** (such as `godot::Input *`) or a **smart reference wrapper** (such as `godot::Ref<godot::Texture2D>`), most documentation online uses the **arrow operator (`->`)** to call methods on that object.

However:
- On many keyboards (especially Latin American / Spanish layouts where `<` and `>` require awkward modifier chords or dead keys), typing `->` over and over is frustrating.
- For C++ beginners coming from Python (`Call_Of_Pyray` / `Call_Of_Pythy`), using `.` everywhere makes C++ look and feel almost identical to Python!

In **Call Of Gody**, **100% of our actual source code (`include/` and `src/`) uses ONLY the dot (`.`) operator**!
At the same time, when you read official Godot forums, tutorials, or engine source code, you will frequently encounter `->`. Therefore, this Grimoire teaches you **both versions side by side**:
- 🟢 **Version A — Dot (`.`) Syntax**: The keyboard-friendly, reference-based style used across `Call Of Gody`.
- 🔵 **Version B — Arrow (`->`) Syntax**: The traditional pointer style you will see in external C++ tutorials.

---

## 🪨 2. Raylib & Python (`Call_Of_Pyray`) to Godot 4.7 C++ Rosetta Stone

Here is how every single function from your **CallOfRay** and **Call_Of_Pyray** repositories translates to the **Godot 4.7 C++ API**:

| Original Raylib / `Call_Of_Pyray` | Godot 4.7 C++ API (`Call_Of_Gody`) | Official Godot Documentation |
|---|---|---|
| `InitWindow(WIDTH, HEIGHT, ...)` | Configured in `project.godot` + `CallOfGodyArena::_ready()` | [Node::_ready](https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-private-method-ready) |
| `while (!WindowShouldClose())` | `CallOfGodyArena::_process(double delta)` | [Node::_process](https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-private-method-process) |
| `CloseWindow()` | `(*get_tree()).quit()` | [SceneTree::quit](https://docs.godotengine.org/en/stable/classes/class_scenetree.html#class-scenetree-method-quit) |
| `GetFrameTime()` | `delta` parameter passed automatically to `_process(double delta)` | [Idle and Physics Processing](https://docs.godotengine.org/en/stable/tutorials/scripting/idle_and_physics_processing.html) |
| `GetScreenWidth()` / `GetScreenHeight()` | `get_viewport_rect().size.x` / `get_viewport_rect().size.y` | [CanvasItem::get_viewport_rect](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-get-viewport-rect) |
| `GetFPS()` | `(*Engine::get_singleton()).get_frames_per_second()` | [Engine::get_frames_per_second](https://docs.godotengine.org/en/stable/classes/class_engine.html#class-engine-method-get-frames-per-second) |
| `glGetString(0x1F02)` | `(*RenderingServer::get_singleton()).get_video_adapter_api_version()` | [RenderingServer](https://docs.godotengine.org/en/stable/classes/class_renderingserver.html) |
| `GetRandomValue(min, max)` | `get_random_value(min, max)` (`UtilityFunctions::randi_range`) | [@GlobalScope::randi_range](https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#class-globalscope-method-randi-range) |
| `TextFormat("FPS: %d", fps)` / `f"FPS: {fps}"` | `text_format("FPS: %d", fps)` (returns `godot::String`) | [String](https://docs.godotengine.org/en/stable/classes/class_string.html) |
| `DEG2RAD` / `math.radians(angle)` | `Math::deg_to_rad(angle)` | [@GlobalScope::deg_to_rad](https://docs.godotengine.org/en/stable/classes/class_%40globalscope.html#class-globalscope-method-deg-to-rad) |
| `IsKeyDown(KEY_...)` | `(*Input::get_singleton()).is_key_pressed(KEY_...)` -> `actions.inputs[...]` | [Input::is_key_pressed](https://docs.godotengine.org/en/stable/classes/class_input.html#class-input-method-is-key-pressed) |
| `IsKeyReleased(KEY_...)` | `released[action] = released[action] && !inputs[action]` -> `actions.released[...]` | [Input::is_key_pressed](https://docs.godotengine.org/en/stable/classes/class_input.html#class-input-method-is-key-pressed) |
| `LoadImage("assets/...")` | `Image::load_from_file("res://assets/...")` | [Image::load_from_file](https://docs.godotengine.org/en/stable/classes/class_image.html#class-image-method-load-from-file) |
| `GenImageColor(w, h, BLANK)` | `Image::create_empty(w, h, false, Image::FORMAT_RGBA8)` | [Image::create_empty](https://docs.godotengine.org/en/stable/classes/class_image.html#class-image-method-create-empty) |
| `ImageDraw(&dst, src, sRec, dRec, WHITE)` | `(*dst.ptr()).blit_rect(src, src_rect_i, dst_top_left_i)` | [Image::blit_rect](https://docs.godotengine.org/en/stable/classes/class_image.html#class-image-method-blit-rect) |
| `LoadTextureFromImage(img)` | `ImageTexture::create_from_image(img)` | [ImageTexture::create_from_image](https://docs.godotengine.org/en/stable/classes/class_imagetexture.html#class-imagetexture-method-create-from-image) |
| `LoadSound("assets/...")` | `AudioStreamOggVorbis::load_from_file("res://assets/...")` | [AudioStreamOggVorbis](https://docs.godotengine.org/en/stable/classes/class_audiostreamoggvorbis.html) |
| `PlaySound(s)` / `StopSound(s)` | `audio_player.play()` / `audio_player.stop()` | [AudioStreamPlayer](https://docs.godotengine.org/en/stable/classes/class_audiostreamplayer.html) |
| `IsSoundPlaying(s)` | `audio_player.is_playing()` | [AudioStreamPlayer::is_playing](https://docs.godotengine.org/en/stable/classes/class_audiostreamplayer.html#class-audiostreamplayer-method-is-playing) |
| `ClearBackground(RED_C)` | `draw_rect(Rect2(0, 0, WIDTH, HEIGHT), RED_C, true)` | [CanvasItem::draw_rect](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-draw-rect) |
| `DrawCircleV(pos, r, col)` | `canvas.draw_circle(pos, r, col)` | [CanvasItem::draw_circle](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-draw-circle) |
| `DrawLineEx(p1, p2, thick, col)` | `canvas.draw_line(p1, p2, col, thick)` | [CanvasItem::draw_line](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-draw-line) |
| `DrawTexture(tex, x, y, WHITE)` | `canvas.draw_texture(tex, Vector2(x, y), WHITE_C)` | [CanvasItem::draw_texture](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-draw-texture) |
| `DrawTextureRec(tex, src, pos, WHITE)` | `canvas.draw_texture_rect_region(tex, dest_rect, src_rect, WHITE_C)` | [CanvasItem::draw_texture_rect_region](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-draw-texture-rect-region) |
| `DrawTexturePro(Gun, ..., angle, ...)` | `draw_set_transform(sight, deg_to_rad(angle))` + `draw_texture(...)` | [CanvasItem::draw_set_transform](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-draw-set-transform) |
| `MeasureText(txt, size)` | `(*font.ptr()).get_string_size(txt, ..., size).x` | [Font::get_string_size](https://docs.godotengine.org/en/stable/classes/class_font.html#class-font-method-get-string-size) |
| `DrawText(txt, x, y, size, col)` | `canvas.draw_string(font, Vector2(x, y), txt, ..., size, col)` | [CanvasItem::draw_string](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html#class-canvasitem-method-draw-string) |

---

## 🔑 3. Core Rule: How to Turn Any Pointer or `Ref<T>` into Dot (`.`) Syntax

In C++, the arrow operator `ptr->method()` is simply shorthand for `(*ptr).method()`.
Even better, C++ lets you bind a **reference (`T &`)** to a dereferenced pointer once, and then use clean dot (`.`) syntax for as many lines as you want!

### Pattern 1: Raw Pointers (`T *`)
```cpp
// 🟢 VERSION A: Dot (.) Operator via C++ Reference Binding (Used in Call Of Gody)
godot::Input *input_ptr = godot::Input::get_singleton();
if (input_ptr != nullptr) {
    godot::Input &input = *input_ptr; // Bind reference once using '*'
    bool left  = input.is_key_pressed(godot::KEY_LEFT);
    bool right = input.is_key_pressed(godot::KEY_RIGHT);
}

// 🔵 VERSION B: Arrow (->) Operator (Traditional Pointer Style)
godot::Input *input_ptr = godot::Input::get_singleton();
if (input_ptr != nullptr) {
    bool left  = input_ptr->is_key_pressed(godot::KEY_LEFT);
    bool right = input_ptr->is_key_pressed(godot::KEY_RIGHT);
}
```

### Pattern 2: Godot Smart References (`godot::Ref<T>`)
Godot's `Ref<T>` template class manages reference-counted resources (`Image`, `Texture2D`, `AudioStream`, `Font`). It provides a `.ptr()` method (which uses the dot operator!) that returns the underlying `T *` pointer. Dereferencing `.ptr()` with `*` gives us a direct C++ reference:

```cpp
// 🟢 VERSION A: Dot (.) Operator via (*ref.ptr()) Reference Binding
godot::Ref<godot::Texture2D> gun_ref = load_texture_asset("res://assets/Gun.png");
if (gun_ref.is_valid()) {
    const godot::Texture2D &gun_tex = *gun_ref.ptr();
    int width  = gun_tex.get_width();
    int height = gun_tex.get_height();
}

// 🔵 VERSION B: Arrow (->) Operator via Ref<T>::operator->()
godot::Ref<godot::Texture2D> gun_ref = load_texture_asset("res://assets/Gun.png");
if (gun_ref.is_valid()) {
    int width  = gun_ref->get_width();
    int height = gun_ref->get_height();
}
```

---

## 🎮 4. Subsystem 1: Engine Singletons (`Input`, `Engine`, `RenderingServer`, `ThemeDB`)

Godot exposes global engine services as **Singletons** retrieved via `ClassName::get_singleton()`.
- **Official Docs**:
  - [`godot::Input`](https://docs.godotengine.org/en/stable/classes/class_input.html)
  - [`godot::Engine`](https://docs.godotengine.org/en/stable/classes/class_engine.html)
  - [`godot::RenderingServer`](https://docs.godotengine.org/en/stable/classes/class_renderingserver.html)
  - [`godot::ThemeDB`](https://docs.godotengine.org/en/stable/classes/class_themedb.html)

### 🟢 Version A: Dot (`.`) Operator Syntax (Used in `src/utils.cpp` & `src/arena.cpp`)
```cpp
// 1. Querying current Frames Per Second (FPS) from godot::Engine
int current_fps = 0;
godot::Engine *engine_ptr = godot::Engine::get_singleton();
if (engine_ptr != nullptr) {
    godot::Engine &engine = *engine_ptr;
    current_fps = static_cast<int>(engine.get_frames_per_second());
}

// 2. Querying Vulkan API version & GPU name from godot::RenderingServer
godot::RenderingServer *rs_ptr = godot::RenderingServer::get_singleton();
if (rs_ptr != nullptr) {
    godot::RenderingServer &rs = *rs_ptr;
    godot::String vulkan_ver = rs.get_video_adapter_api_version();
    godot::String gpu_name   = rs.get_video_adapter_name();
}

// 3. Retrieving the default UI Font from godot::ThemeDB
godot::Ref<godot::Font> default_font;
godot::ThemeDB *theme_db_ptr = godot::ThemeDB::get_singleton();
if (theme_db_ptr != nullptr) {
    godot::ThemeDB &theme_db = *theme_db_ptr;
    default_font = theme_db.get_fallback_font();
}
```

### 🔵 Version B: Arrow (`->`) Operator Syntax (Traditional Style)
```cpp
// 1. Querying current Frames Per Second (FPS) from godot::Engine
int current_fps = 0;
godot::Engine *engine_ptr = godot::Engine::get_singleton();
if (engine_ptr != nullptr) {
    current_fps = static_cast<int>(engine_ptr->get_frames_per_second());
}

// 2. Querying Vulkan API version & GPU name from godot::RenderingServer
godot::RenderingServer *rs_ptr = godot::RenderingServer::get_singleton();
if (rs_ptr != nullptr) {
    godot::String vulkan_ver = rs_ptr->get_video_adapter_api_version();
    godot::String gpu_name   = rs_ptr->get_video_adapter_name();
}

// 3. Retrieving the default UI Font from godot::ThemeDB
godot::Ref<godot::Font> default_font;
godot::ThemeDB *theme_db_ptr = godot::ThemeDB::get_singleton();
if (theme_db_ptr != nullptr) {
    default_font = theme_db_ptr->get_fallback_font();
}
```

---

## 🖼️ 5. Subsystem 2: Reference-Counted Resources (`Ref<Image>`, `Ref<Texture2D>`, `Ref<AudioStream>`)

In Godot 4.7, heavy assets like images, GPU textures, fonts, and audio streams inherit from [`godot::RefCounted`](https://docs.godotengine.org/en/stable/classes/class_refcounted.html) and are held inside [`godot::Ref<T>`](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html).

### 🟢 Version A: Dot (`.`) Operator Syntax (Used in `src/entities.cpp`)
```cpp
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

    canvas.draw_string(font, text_pos, label, godot::HORIZONTAL_ALIGNMENT_LEFT, -1.0f, fontsize, YELLOW_C);
}
```

### 🔵 Version B: Arrow (`->`) Operator Syntax (Traditional Style)
```cpp
void Enemy::draw_label(godot::Node2D *canvas, const godot::Ref<godot::Font> &font) const {
    if (font.is_null() || canvas == nullptr) {
        return;
    }

    godot::String label = godot::String::num_int64(life);
    godot::Vector2 text_size = font->get_string_size(
        label,
        godot::HORIZONTAL_ALIGNMENT_LEFT,
        -1.0f,
        fontsize
    );

    godot::Vector2 text_pos(
        position.x - (text_size.x / 2.0f),
        position.y + static_cast<float>(size_r + 5 + fontsize)
    );

    canvas->draw_string(font, text_pos, label, godot::HORIZONTAL_ALIGNMENT_LEFT, -1.0f, fontsize, YELLOW_C);
}
```

---

## 🧩 6. Subsystem 3: Dynamic Texture Atlas Stitching (`Image::create_empty` & `Image::blit_rect`)

In `CallOfRay`, `load_assets()` loaded `zombie_face_male.png` and `zombie_face_female.png`, created a blank image wide enough for both (`GenImageColor`), copied each zombie face side-by-side (`ImageDraw`), and uploaded the combined atlas to the GPU (`LoadTextureFromImage`).

In Godot 4.7 C++, we use [`godot::Image::create_empty`](https://docs.godotengine.org/en/stable/classes/class_image.html#class-image-method-create-empty), [`godot::Image::blit_rect`](https://docs.godotengine.org/en/stable/classes/class_image.html#class-image-method-blit-rect), and [`godot::ImageTexture::create_from_image`](https://docs.godotengine.org/en/stable/classes/class_imagetexture.html#class-imagetexture-method-create-from-image).

### 🟢 Version A: Dot (`.`) Operator Syntax (Used in `src/utils.cpp`)
```cpp
godot::Ref<godot::Image> z1_ref = load_image_from_texture_asset("res://assets/zombie_face_male.png");
godot::Ref<godot::Image> z2_ref = load_image_from_texture_asset("res://assets/zombie_face_female.png");

if (z1_ref.is_valid() && z2_ref.is_valid()) {
    godot::Image &z1 = *z1_ref.ptr();
    godot::Image &z2 = *z2_ref.ptr();

    z1.convert(godot::Image::FORMAT_RGBA8);
    z2.convert(godot::Image::FORMAT_RGBA8);

    int z1_w = z1.get_width();
    int z1_h = z1.get_height();
    int z2_w = z2.get_width();
    int z2_h = z2.get_height();

    godot::Ref<godot::Image> atlas_img_ref =
        godot::Image::create_empty(z1_w + z2_w, std::max(z1_h, z2_h), false, godot::Image::FORMAT_RGBA8);

    godot::Image &atlas_img = *atlas_img_ref.ptr();
    atlas_img.fill(godot::Color(0.0f, 0.0f, 0.0f, 0.0f));

    atlas_img.blit_rect(z1_ref, godot::Rect2i(0, 0, z1_w, z1_h), godot::Vector2i(0, 0));
    atlas_img.blit_rect(z2_ref, godot::Rect2i(0, 0, z2_w, z2_h), godot::Vector2i(z1_w, 0));

    zombie_atlas = godot::ImageTexture::create_from_image(atlas_img_ref);
}
```

### 🔵 Version B: Arrow (`->`) Operator Syntax (Traditional Style)
```cpp
godot::Ref<godot::Image> z1_ref = load_image_from_texture_asset("res://assets/zombie_face_male.png");
godot::Ref<godot::Image> z2_ref = load_image_from_texture_asset("res://assets/zombie_face_female.png");

if (z1_ref.is_valid() && z2_ref.is_valid()) {
    z1_ref->convert(godot::Image::FORMAT_RGBA8);
    z2_ref->convert(godot::Image::FORMAT_RGBA8);

    int z1_w = z1_ref->get_width();
    int z1_h = z1_ref->get_height();
    int z2_w = z2_ref->get_width();
    int z2_h = z2_ref->get_height();

    godot::Ref<godot::Image> atlas_img_ref =
        godot::Image::create_empty(z1_w + z2_w, std::max(z1_h, z2_h), false, godot::Image::FORMAT_RGBA8);

    atlas_img_ref->fill(godot::Color(0.0f, 0.0f, 0.0f, 0.0f));
    atlas_img_ref->blit_rect(z1_ref, godot::Rect2i(0, 0, z1_w, z1_h), godot::Vector2i(0, 0));
    atlas_img_ref->blit_rect(z2_ref, godot::Rect2i(0, 0, z2_w, z2_h), godot::Vector2i(z1_w, 0));

    zombie_atlas = godot::ImageTexture::create_from_image(atlas_img_ref);
}
```

---

## 🔊 7. Subsystem 4: SceneTree Nodes & Memory Allocation (`memnew`, `add_child`, `AudioStreamPlayer`)

In Godot, nodes that live inside the [`SceneTree`](https://docs.godotengine.org/en/stable/classes/class_scenetree.html) (such as [`godot::AudioStreamPlayer`](https://docs.godotengine.org/en/stable/classes/class_audiostreamplayer.html)) are allocated using Godot's `memnew(ClassName)` macro and attached to a parent node via [`Node::add_child`](https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-method-add-child).

### 🟢 Version A: Dot (`.`) Operator Syntax (Used in `src/arena.cpp` & `src/entities.cpp`)
```cpp
godot::AudioStreamPlayer *CallOfGodyArena::create_audio_channel(const char *node_name, float volume_linear) {
    godot::AudioStreamPlayer *channel_ptr = memnew(godot::AudioStreamPlayer);
    godot::AudioStreamPlayer &channel = *channel_ptr;

    channel.set_name(godot::String(node_name));
    float volume_db = (volume_linear > 0.001f) ? (20.0f * std::log10(volume_linear)) : -80.0f;
    channel.set_volume_db(volume_db);

    add_child(channel_ptr);
    return channel_ptr;
}
```

### 🔵 Version B: Arrow (`->`) Operator Syntax (Traditional Style)
```cpp
godot::AudioStreamPlayer *CallOfGodyArena::create_audio_channel(const char *node_name, float volume_linear) {
    godot::AudioStreamPlayer *channel_ptr = memnew(godot::AudioStreamPlayer);

    channel_ptr->set_name(godot::String(node_name));
    float volume_db = (volume_linear > 0.001f) ? (20.0f * std::log10(volume_linear)) : -80.0f;
    channel_ptr->set_volume_db(volume_db);

    this->add_child(channel_ptr);
    return channel_ptr;
}
```

---

## 🎨 8. Subsystem 5: 2D Vulkan Canvas Drawing (`CanvasItem::_draw`, Transforms, Shapes & Text)

Every [`godot::Node2D`](https://docs.godotengine.org/en/stable/classes/class_node2d.html) inherits from [`godot::CanvasItem`](https://docs.godotengine.org/en/stable/classes/class_canvasitem.html). When you call `queue_redraw()` inside `_process(delta)`, Godot's Vulkan renderer calls your node's virtual `_draw()` method.

### 🟢 Version A: Dot (`.`) Operator Syntax (Used in `src/arena.cpp`)
```cpp
if (Gun.is_valid()) {
    const godot::Texture2D &gun_tex = *Gun.ptr();
    godot::Vector2 origin_offset(-gun_tex.get_width() / 2.0f, -gun_tex.get_height() / 2.0f);

    draw_set_transform(player.sight, godot::Math::deg_to_rad(player.angle), godot::Vector2(1.0f, 1.0f));
    draw_texture(Gun, origin_offset, WHITE_C);
    draw_set_transform(godot::Vector2(0.0f, 0.0f), 0.0f, godot::Vector2(1.0f, 1.0f));
}
```

### 🔵 Version B: Arrow (`->`) Operator Syntax (Traditional Style)
```cpp
if (Gun.is_valid()) {
    godot::Vector2 origin_offset(-Gun->get_width() / 2.0f, -Gun->get_height() / 2.0f);

    this->draw_set_transform(player.sight, godot::Math::deg_to_rad(player.angle), godot::Vector2(1.0f, 1.0f));
    this->draw_texture(Gun, origin_offset, WHITE_C);
    this->draw_set_transform(godot::Vector2(0.0f, 0.0f), 0.0f, godot::Vector2(1.0f, 1.0f));
}
```

---

## 🕹️ 9. Subsystem 6: 2-Map Input Controller (`inputs` vs. `released`) & Entity Methods

To replicate raylib's `IsKeyDown` and `IsKeyReleased` cleanly in Godot C++ (matching `Call_Of_Pyray/utils.py`), `CONTROLS` uses two `std::unordered_map<std::string, bool>` maps:
1. **`inputs`**: True on every frame a key is currently held down.
2. **`released`**: True only on the single frame a key transitions from held down (last frame) to released (this frame).

```cpp
// At the end of each frame, reset() transfers the current frame's held keys
// into `released` and clears `inputs`:
void CONTROLS::reset() {
    for (const auto &action : actions) {
        released[action] = inputs[action];
        inputs[action] = false;
    }
}

// At the start of the next frame, get_input() polls held keys into `inputs`
// and computes `released[action]`:
for (const auto &action : actions) {
    released[action] = released[action] && !inputs[action];
}
```

And just like in `Call_Of_Pyray/entities.py`, each entity encapsulates its own movement, homing/chasing, and squared-distance collision check:
```cpp
void Bullet::chasing(godot::Vector2 target_pos, float step) {
    if (distance_trigger(position, target_pos, range)) {
        position = chase(target_pos, position, step);
    } else {
        position = move_pt(position, step, angle);
    }
}

bool Enemy::collision(godot::Vector2 target_pos, float target_size) const {
    return distance_trigger(target_pos, position, static_cast<float>(size_r) + target_size);
}
```

---

<p align="center">
  <b>Happy C++ Game Crafting in Godot 4.7 — With Zero Arrow Operators! 🎮🧟‍♂️✨</b><br/>
  <a href="./README.md">⬅️ Return to Call Of Gody Main README.md</a>
</p>
