---
title: Game Engine
id: engine
---

# Engine

First you need to create an engine, use this line in your main function:

```cpp
EngineContext my_engine_ctx;
```

You will use the object a lot of time during the creation of your project

## Load scenes

To add scene to your engine use this line:

```cpp
my_engine_ctx.add_scene_loader("main_menu", load_main_menu_scene);
```

This line link the function `load_main_menu_scene(engn::EngineContext& engine_ctx)` to the value `main_menu`

Then you can use it like this to change between all your scene:

```cpp
engine_ctx.set_scene("main_menu");
```

A scene loader function should look like this:

```cpp
void load_game_scene(engn::EngineContext& engine_ctx) {
    // Your scene
}
```

## Load texture

Loading the textures in the engine is very easy, all you need to do is add this line:

```cpp
my_engine_ctx.assets_manager.load_texture("img", "assets/sprites/image.png");
```

This would load a texture called `img` using the file stored at this path: `assets/sprites/image.png`, you can then use the texture by calling it when you need it.

:::info

To load musics and sound use the `load_music()` and `load_sound()` function:

```cpp
engine_ctx.assets_manager.load_music("menu_music", "assets/music/Menu_music.mp3");
engine_ctx.assets_manager.load_sound("shoot_sound", "assets/music/Blaster.mp3");
```

:::
