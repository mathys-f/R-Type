#include "raylib.h"

#pragma once

#define WIDTH               1920
#define HEIGHT              1000
#define SCROLL_SPEED        5
#define PARTICLES           3
#define STARS               1000
#define MAX_BULLETS         100
#define MAX_ENEMIES         8

// TODO after the assets manager is done
struct Sprite {
    Rectangle sourceRect;
    float scale;
    int frame;
    Texture2D texture;
};

