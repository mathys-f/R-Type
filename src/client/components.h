#include "raylib.h"

#pragma once

#define WIDTH               1920
#define HEIGHT              1000
#define SCROLL_SPEED        12
#define PARTICLES           3
#define STARS               1000
#define MAX_BULLETS         100
#define MAX_ENEMIES         8

struct Position {
    float x, y;
    Vector2 origin;
};

struct Velocity {
    float x, y, z;
    float rx, ry, rz;
};

struct Sprite {
    Rectangle sourceRect;
    float scale;
    int frame;
};

struct Health {
    int current;
    int max;
};

struct Player {
    // Tag component
};

struct Enemy {
    // Tag component
};

struct Bullet {
    // Tag component
};

struct Star {
    float z; // depth for parallax
};

struct Particle {
    float lifetime;
    float maxLifetime;
};

struct Hitbox {
    float width, height;
    float offsetX, offsetY;
};

enum class PatternType {
    Straight,
    Sine,
    ZigZag,
    Dive
};

struct MovementPattern {
    PatternType type;
    float speed;
    float amplitude;
    float frequency;
    float timer;
    float baseY;
};

enum class ExplosionType {
    Small,
    Large
};

struct Explosion {
    ExplosionType type;
    float timer;
    float frameDuration;
    int currentFrame;
};