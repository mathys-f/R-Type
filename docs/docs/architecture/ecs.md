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
| `colisionLayer` | CollisionLayer | The layer in which the collision box lives in |

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

Dependencies: [`Transform`](#2-transform), [`Velocity`](#2-velocity)

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
