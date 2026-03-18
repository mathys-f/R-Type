---
title: Entity Component System (ECS)
id: ecs
---

This project uses an Entity–Component–System (ECS) architecture to implement a multiplayer R-Type-like game. The ECS is designed to run both:

- **Server-side (headless)**:
Contains no rendering or input systems. Only gameplay logic, networking, and simulation.

- **Client-side**:
Adds rendering, animation, and local input prediction.

The authoritative simulation runs on the server. Clients receive replicated states and send player actions.

## Components

Components are pure data containers with no logic. Below is the list of components currently defined.

### 1. Controllable

Entities whose movement or actions are driven by user input or AI.

| Field     | Type   | Description                   |
|-----------|--------|-------------------------------|
| `speed`   | float  | Base movement speed modifier  |


### 2. Transform

Spatial properties.

| Field        | Type   | Description    |
|--------------|--------|----------------|
| `x, y, z`    | float  | Position       |
| `rx, ry, rz` | float  | Rotation       |
| `sx, sy, sz` | float  | Scale          |

### 3. Velocity

Spatial properties.

| Field           | Type   | Description       |
|-----------------|--------|-------------------|
| `vx, vy, vz`    | float  | Movement speed    |
| `vrx, vry, vrz` | float  | Rotation speed    |


### 4. Drawable

How an entity should be rendered (client-side only).

| Field     | Type   | Description                     |
|-----------|--------|---------------------------------|
| `mesh`    | string | Identifier of the visual model  |

### 5. Hitbox

Collision bounds.

| Field           | Type           | Description                                   |
|-----------------|----------------|-----------------------------------------------|
| `x, y, z`       | float          | Position                                      |
| `w, h, d`       | float          | Size                                          |
| `collisionLayer` | CollisionLayer | The layer in which the collision box lives in |

```cpp
enum CollisionLayer {
    Ally,           // players ship
    Enemy,          // enemy ships
    AllyProjectile, // bullets fired by allies
    EnemyProjectile,// bullets fired by enemies
    Pickup,         // power-ups, bonuses
    Environment     // walls, boundaries, obstacles
};
```

### 6. Weapon

Ability to create other entities.

| Field        | Type   | Description                                  |
|--------------|--------|----------------------------------------------|
| `cooldown`   | int    | Actual cooldown left in ms                   |
| `firerate`   | int    | Cooldown in ms between shots in ms           |
| `dmg`        | int    | Damage inflicted (recovered if negative)     |
| `vx, vy`     | float  | Initial velocity                             |
| `mesh`       | string | Identifier of the visual model               |

### 7. Health

Tracks hitpoints & damaging/healing events.

| Field          | Type         | Description                                        |
|----------------|--------------|----------------------------------------------------|
| `hp`           | int          | Current health points                              |
| `maxHp`        | int          | Max amount of health point                         |
| `changeQueue`  | vector\<int\>  | Incoming health points changes queued this frame |

:::warning

ChangeQueue must be of a fixed size or there will be problems !

The alternative would be to use a global variable like this:

```cpp
std::vector<int targetEntity, int amount> g_damage_events;
```

:::

### 8. Stats

Ability to influence current game statistics.

| Field        | Type   | Description      |
|--------------|--------|------------------|
| `score`      | int    | Score obtained   |
| `dmg`        | int    | Damage inflicted |
| `kills`      | int    | Kill count       |


### 9. Replicated

Indicates that the entity is present both server & clients.

| Field   | Type   | Description          |
|---------|--------|----------------------|
| `tag`   | string | Unique identifier    |


## Systems

Systems operate on entities containing the required set of components.

Dependencies listed for each system represent the minimal component set an entity must have to be processed by that system.

### Core Gameplay Systems

These systems simulate the game world.
When running on the client, they run in prediction mode (no authority).

### 1. MoveSystem

Dependencies: [`Transform`](#2-transform), [`Velocity`](#3-velocity)

Applies velocities to transforms every tick.

### 2. CollisionSystem

Dependencies: [`Transform`](#2-transform), [`Hitbox`](#5-hitbox), [`Health`](#7-health), [`Stats`](#8-stats)

Checks collisions between entities based on CollisionLayer rules and registers damage events.

### 3. UpdateHealthSystem

Dependencies: [`Health`](#7-health)

Consumes all entries in changeQueue (or global damage events) and updates hp.
Triggers death/despawn events.

### 4. WeaponFireSystem

Dependencies: [`Weapon`](#6-weapon), [`Transform`](#2-transform)

Handles cooldown logic

Spawns projectile entities

Assigns Velocity + Hitbox + Damage payload

### B. Player Interaction Systems

Used to turn player input into entity behavior.

### 6. PlayerControlSystem

Dependencies: [`Controllable`](#1-controllable), [`Velocity`](#3-velocity)

Consumes input commands from the Input Buffer (not a component).
Adjusts velocity.

### C. Networking Systems (Server or Client)

Autoritative server sends state → client receives.

### 8. NetworkReceiveInputSystem (Server-only)

Dependencies: `None`

Receives input packets from network and stores them in the global InputBuffer.

### 9. NetworkReplicationSystem (Server-only)

Dependencies: [`Replicated`](#9-replicated), (any components allowed)

Serializes updated components

Sends deltas to clients

Sends spawn/despawn events

### 10. NetworkReceiveStateSystem (Client-only)

Dependencies: `none`

Receives authoritative state updates from server and stores them for reconciliation.

### 11. ClientReconciliationSystem (Client-only)

Dependencies: [`Transform`](#2-transform), [`Velocity`](#3-velocity), [`Controllable`](#1-controllable)

Replays local inputs after server updates to reduce snapping.

### D. Client-Only Visual Systems

Only used on machines rendering the game.

### 12. RenderSystem (Client-only)

Dependencies: [`Transform`](#2-transform), [`Drawable`](#4-drawable), [`Health`](#7-health), [`Stats`](#8-stats)

Handles rendering meshes, sprites, HUD.

### 13. AnimationSystem (Client-only)

Dependencies: [`Drawable`](#4-drawable)

Sprite animations, particle effects, etc.

### 14. DebugDrawSystem (optional)

Dependencies: [`Transform`](#2-transform), [`Hitbox`](#5-hitbox)

Draws hitboxes during debugging.


## Use the ECS

:::tip

See the engine page for more information on the engine itself

:::

### Create components

Add a file in `/src/game_engine/components/` containing the components you want added.

Example:

```cpp
namespace engn::cpnt {
    
    struct Bullet : ISyncComponent {
        Bullet() = default;

        engn::SerializedComponent serialize() const override;
        void deserialize(const std::vector<std::byte>& data) override;
    };
}
```
In this example thecomponent bullet is defined with:
- The constructor `Bullet()`
- The functions `serialize()` and `deserialize()`
- Inherit the `ISyncComponent` class

The two functions `serialize()` and `deserialize()` are used when components need to be synchronised between all the players; for example, the components `Star` would look like this:
```cpp
namespace engn::cpnt {

    struct Star {
        float z;
    };
}
```
These components don't need to be synchronised between the players.

Here is how you should defines the `serialize()` and `deserialize()` function:

```cpp
engn::SerializedComponent Bullet::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::bullet;
    serialized.data = {};
    return serialized;
}

void Bullet::deserialize(const std::vector<std::byte>& data) {

}
```

:::info

For other components that have variable stored in them you should defines them like this:
For a components `Health` with variable `hp` and `max_hp`

```cpp
engn::SerializedComponent Health::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::health;
    std::uint32_t size = sizeof(hp) + sizeof(max_hp);
    serialized.data.resize(size);

    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &hp, sizeof(hp));
    offset += sizeof(hp);
    std::memcpy(serialized.data.data() + offset, &max_hp, sizeof(max_hp));
    return serialized;
}

void Health::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(hp) + sizeof(max_hp);

    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&hp, data.data() + offset, sizeof(hp));
        offset += sizeof(hp);
        std::memcpy(&max_hp, data.data() + offset, sizeof(max_hp));
    }
}
```

:::

### Add components to the scene

In each scene loader the registry is reset, so you need to use this line to have access to a new registry.

```cpp 
auto& my_registry = my_engine_ctx.registry;
```

This create a registry called `my_registry`

Then in your function, after creating the registry, add the components to the registry:

```cpp
my_registry.register_component<cpnt::Bullet>();
my_registry.register_component<cpnt::Star>();
```

### Create a system

At this path `/src/game_engine/systems/` you can a this function to create a system called `star_scroll_system`

```cpp
void sys::star_scroll_system(EngineContext& my_engine_ctx,ecs::SparseArray<cpnt::Transform> const& positions, ecs::SparseArray<cpnt::Star> const& stars) {
    // Add your Function
}
```

### Add a system

After creating youre system you can use this line in youre scene loader to use it:

```cpp
my_engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
```

this would add a system called `star_scroll_system` that use the component `Transform` and `Star`.

### Create an entity

To create an entity use this line:

```cpp
auto star = my_registry.spawn_entity();
```

This add an entity called `star` to your registry.

Then you need to add components to the entity like this:

```cpp
my_engine_ctx.registry.add_component(star, cpnt::Star{42.0f});
```

This add the components `Star` to your `mob`, and with how i created my `Star` component i needed to add the float `z` to the component, here equal to `42.0f`.