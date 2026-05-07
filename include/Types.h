#pragma once

#include "raylib.h"

#include <string>

enum class GameState {
    Menu,
    Playing,
    Paused,
    Victory,
    Defeat
};

enum class TrapState {
    Idle,
    Windup,
    Active,
    Cooldown,
    Spent
};

struct ShardParticle {
    Vector2 position{};
    Vector2 velocity{};
    Color color{};
    float life = 0.0f;
    float maxLife = 1.0f;
};
