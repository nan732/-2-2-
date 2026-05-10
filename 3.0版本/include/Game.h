#pragma once

#include "Level.h"
#include "PlayerBall.h"
#include "Types.h"
#include "UI.h"

#include "raylib.h"

#include <vector>

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    void resetRun();
    void update(float dt);
    void draw();
    void updateCamera();
    void loadMusic();
    void spawnShards(Vector2 origin);
    void updateShards(float dt);
    void drawShards() const;

    GameState state = GameState::Menu;
    PlayerBall player;
    Level level;
    UI ui;
    std::vector<ShardParticle> shards;
    Music music{};
    float elapsedTime = 0.0f;
    float cameraX = 0.0f;
    bool musicReady = false;
};
