---
title: User Interface
id: ui
---

## How it works ?

The user interface system of the R-Type game engine relies on [LUA](https://www.lua.org/).

The users can define the UI elements via an exposed C++ API, this allows runtime reloading of the UI without needing to recompile anything (F5 by default).

As the UI elements are stored as entities in an ECS, then the interaction must be written in C++ via a system.

The rest of this file is a quick tutorial on how to make a basic app with a single menu scene.


## Creating the scene

You need to add a set of components & systems to a scene before you can load a LUA UI  layout file.

### 1. Register UI components

First you need to register the core components of the UI module.

Then you can add the ones corresponding to the UI elements you wish to use.

```cpp title="Registering UI components"
#include "engine/scenes_loaders.h"

void load_main_menu_scene(engn::EngineContext& ctx) {
    // Register core UI components, these five components are required if you want to use the UI system
    reg.register_component<cpnt::UITransform>();
    reg.register_component<cpnt::UINavigation>();
    reg.register_component<cpnt::UIFocusable>();
    reg.register_component<cpnt::UIInteractable>();
    reg.register_component<cpnt::UIStyle>();

    // Register the individual UI elements you wish to use. 
    reg.register_component<cpnt::UIText>();
    reg.register_component<cpnt::UIButton>();
    // reg.register_component<cpnt::UIInputField>();
    // reg.register_component<cpnt::UISlider>();

    // ... 
}
```

### 2. Register UI systems

First you need to register the core systems of the UI module.

Then you can add the ones corresponding to the UI elements you wish to use.

```cpp title="Registering UI systems"
#include "engine/scenes_loaders.h"

void load_main_menu_scene(engn::EngineContext& ctx) {
    // ...

    // Register the core UI systems, you can opt out of the ones used for specific UI elements you do not wish to use
    engine_ctx.add_system<>(sys::fetch_inputs);
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<cpnt::UIInteractable, cpnt::UIFocusable, cpnt::UINavigation>(sys::ui_navigation);
    engine_ctx.add_system<>(sys::ui_press);
    // engine_ctx.add_system<cpnt::UIInteractable>(sys::ui_input_field_updater);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_text_renderer);

    // ...
}
```

### 3. Add your [UI events handling system](#handling-ui-events)

```cpp title="Registering UI events handling system "
#include "engine/scenes_loaders.h"

void load_main_menu_scene(engn::EngineContext& ctx) {
    // ...

    // Register your system which will handle UI events
    engine_ctx.add_system<>(handle_main_menu_ui_events);

    // ...
}
```

### 4. Load your LUA layout file(s)

To do so, you can use two methods :
- `engn::lua::load_lua_script_from_file`
- `engn::lua::load_lua_scripts_from_folder`
- `engn::lua::load_lua_scripts_from_folder_recurse`

```cpp title="Loading LUA layout file "
#include "engine/scenes_loaders.h"

void load_main_menu_scene(engn::EngineContext& ctx) {
    // ...

    // Load from single file
    engn::LUA::load_LUA_script_from_file(engine_ctx.LUA_ctx->get_LUA_state(), "lua/main_menu.LUA");

    // Load from folder
    lua::load_lua_scripts_from_folder_recurse(engine_ctx.LUA_ctx->get_LUA_state("lua/"))


    // Load from folder recurse
    lua::load_lua_scripts_from_folder_recurse(engine_ctx.LUA_ctx->get_LUA_state("lua/"))

    // ...
}
```

:::info loading multiple files
Note that both `load_lua_scripts_from_folder` & `load_lua_scripts_from_folder` will not load files with a name starting with an underscore (ex:"_styles.lua").

Use this to organize and reuse your styling files !
:::

#### Final result

```cpp title="Basic GUI scene loader"
#include "engine/scenes_loaders.h"

void load_main_menu_scene(engn::EngineContext& ctx) {
    // Register core UI components, these five components are required if you want to use the UI system
    reg.register_component<cpnt::UITransform>();
    reg.register_component<cpnt::UINavigation>();
    reg.register_component<cpnt::UIFocusable>();
    reg.register_component<cpnt::UIInteractable>();
    reg.register_component<cpnt::UIStyle>();

    // Register the individual UI elements you wish to use. 
    reg.register_component<cpnt::UIText>();
    reg.register_component<cpnt::UIButton>();
    // reg.register_component<cpnt::UIInputField>();
    // reg.register_component<cpnt::UISlider>();

    // Register the core UI systems, you can opt out of the ones used for specific UI elements you do not wish to use
    engine_ctx.add_system<>(sys::fetch_inputs);
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<cpnt::UIInteractable, cpnt::UIFocusable, cpnt::UINavigation>(sys::ui_navigation);
    engine_ctx.add_system<>(sys::ui_press);
    // engine_ctx.add_system<cpnt::UIInteractable>(sys::ui_input_field_updater);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle, cpnt::UIInteractable>(sys::ui_text_renderer);

    // Register your system which will handle UI events
    engine_ctx.add_system<>(handle_main_menu_ui_events);

    // Load your LUA UI file
    engn::LUA::load_LUA_script_from_file(engine_ctx.LUA_ctx->get_LUA_state(), "main_menu.LUA");
}
```

## Handling UI events

In the [step 3 of creating the scene](#3-add-your-ui-events-handling-system), you have been told to register a system you need to write for handling UI events.

Here is how to do it:

```cpp
// Main handler system
void handle_main_menu_ui_events(engn::EngineContext& engine_ctx) {
    // Retreive the event queue
    const auto& evts = engine_ctx.ui_event_queue;

    // Use the for_each utility to cycle trough all events of a certain type
    evts.for_each<evts::UIButtonClicked>(
        [&engine_ctx](const evts::UIButtonClicked& evt) { handle_ui_button_clicked(engine_ctx, evt); });

    // iterators are also available for the queue, so you can also use a for range loop along with the visitor pattern:
    // https://refactoring.guru/design-patterns/visitor/cpp/example
}

static void handle_ui_button_clicked(EngineContext& ctx, const evts::UIButtonClicked& evt) {
    // Retreive the tag registry from the context & use it to get the button tag
    const auto& tags = ctx.registry.get_tag_registry();
    std::string tag_name = tags.get_tag_name(evt.tag);

    if (tag_name == "play_solo_button") {
        ctx.set_scene("singleplayer_game"); // Navigate to singleplayer game scene
    } else if (tag_name == "play_multiplayer_button") {
        ctx.set_scene("lobby"); // Navigate to lobby scene
    } else if (tag_name == "setting_button") {
        ctx.settings_return_scene = "main_menu";
        ctx.set_scene("audio_settings"); // Navigate to audio settings scene
    } else if (tag_name == "difficulty_button") {
        ctx.set_scene("difficulty_settings"); // Navigate to difficulty settings scene
    } else if (tag_name == "exit_button") {
        ctx.should_quit = true;
    }
}
```

And here is the list of all available UI events under the namespace `engn::evts`:

```cpp title="Available UI events"
struct UIButtonClicked {
    ecs::TagRegistry::TagId tag;
};

struct UISliderChanged {
    ecs::TagRegistry::TagId tag;
    float new_value;
};

struct UICheckboxToggled {
    ecs::TagRegistry::TagId tag;
    bool is_checked;
};

struct UIInputSubmit {
    ecs::TagRegistry::TagId tag;
    std::string text;
};

using UIEvent = std::variant<UIButtonClicked, UISliderChanged, UICheckboxToggled, UIInputSubmit>;
```


## Defining the UI layout

Here is a short example, for more precision, refer to the [LUA API reference](#lua-api-reference).

### 1. Create an element

```lua title="Create an UI element"
Create_ui_button("play_solo_button")

```

### 2. Define it's properties

```lua title="Set it's properties"
local styles = {
    button = {
        background_color =          {r = 0,  g = 0,  b = 0,  a = 0},
        background_color_hovered =  {r = 0,  g = 0,  b = 0,  a = 0},
        background_color_pressed =  {r = 0,  g = 0,  b = 0,  a = 0},
        text_color =                {r = 200, g = 200, b = 200, a = 255},
        text_color_hovered =        {r = 200, g = 200, b = 255, a = 255},
        text_color_pressed =        {r = 150, g = 150, b = 200, a = 255},
        border_color =              {r = 0, g = 0, b = 0, a = 0},
        border_color_hovered =      {r = 0,  g = 0,  b = 0,  a = 0},
        border_color_pressed =      {r = 0,  g = 0,  b = 0,  a = 0},
        border_radius = 0,
        border_thickness = 0
    }
}

Set_ui_text("play_solo_button", {
    content = "Play",
    font_size = 32
})

Set_ui_style("play_solo_button", styles.button)

Set_ui_transform("play_solo_button", {
    x = 39.35,
    y = 34.72,
    z = 0,
    w = 18.52,
    h = 6.94,
    anchor_x = 0,
    anchor_y = 0,
    rotation = 0
})

-- The first Set_ui_transform call will mark the entity as the default one for focus
Set_ui_navigation("play_solo_button", { up = "exit_button", down = "exit_button" })

```

## LUA API reference

You can find the UI LUA api under /scripts/lua/api/_ui_api.lua.

This documentation file is written with EmmyLua documentation synthax, you can get the project from [GitHub](https://github.com/EmmyLuaLs/emmylua-analyzer-rust/tree/main) or as an extension on VScode for autocompletion.

Here is a dump of this file:

```LUA title="LUA API reference"
---@diagnostic disable: missing-return

--------------------------------------------------------------------------------
--- ECS & Entities
--------------------------------------------------------------------------------

---@alias Tag string

--------------------------------------------------------------------------------
--- UI Components
--------------------------------------------------------------------------------

---@class UITransform
---@field x number # Horizontal position (0.0-100.0%) of screen width
---@field y number # Vertical position (0.0-100.0%) of screen width
---@field z number # Layer index
---@field w number # Width (0.0-100.0%) of screen width
---@field h number # Height (0.0-100.0%) of screen width
---@field anchor_x number # Horizontal rotation anchor point (0.0 - 100.0%) of Width
---@field anchor_y number # Vertical rotation anchor point (0.0 - 100.0%) of Width
---@field rotation number # Rotation in degrees

---@class UIStyle
---@field background_color Color
---@field background_color_hovered Color
---@field background_color_pressed Color
---@field text_color Color
---@field text_color_hovered Color
---@field text_color_pressed Color
---@field border_color Color
---@field border_color_hovered Color
---@field border_color_pressed Color
---@field border_radius number
---@field border_thickness number

---@class UINavigation # Used to defines controller/keyboard navigation
---@field up Tag
---@field down Tag
---@field right Tag
---@field left Tag

---@class UIButton # Identifier component

---@class UIText
---@field content string
---@field font_size integer

----------------------------------------------------------
-- Math & Utility Types
----------------------------------------------------------


---@class Color
---@field r integer
---@field g integer
---@field b integer
---@field a integer

----------------------------------------------------------
-- Engine API
----------------------------------------------------------

--- Creates a new entity with these components:
--- - UIButton
--- - UIFocusable
--- - UITransform
--- - UIStyle
--- - UINavigation
--- @param name string # Unique name (tag) for the entity.
function Create_ui_button(name) end

--- Create a new entity with these components:
--- - UIButton
--- - UITransform
--- - UIStyle
--- @param name string # Unique name (tag) for the entity.
--- @param UIText UIText
function Create_ui_text(name, UIText) end

--- Creates a new entity with these components:
--- - UIButton
--- - UIFocusable
--- - UITransform
--- - UIStyle
--- - UINavigation
--- @param name string # Unique name (tag) for the entity.
--- @param default_value string
function Create_ui_input_field(name, default_value) end

----------------------------------------------------------
-- UI Components Setters
----------------------------------------------------------

--- Sets the UITransform component for the given entity tag.
--- @param tag Tag
--- @param UITransform UITransform
function Set_ui_transform(tag, UITransform) end

--- Sets the UIStyle component for the given entity tag.
--- @param tag Tag
--- @param UIStyle UIStyle
function Set_ui_style(tag, UIStyle) end

--- Sets the UIText component for the given entity tag
--- @param tag Tag
--- @param UIText UIText
function Set_ui_text(tag, UIText) end

--- Sets the UINavigation component for the given entity tag
--- @param tag Tag
--- @param UINavigation UINavigation
function Set_ui_navigation(tag, UINavigation) end
```
