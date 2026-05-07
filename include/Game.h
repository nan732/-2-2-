#pragma once

#include "Level.h"
#include "PlayerBall.h"
#include "Types.h"
#include "UI.h"

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
    void spawnShards(Vector2 origin);
    void updateShards(float dt);
    void drawShards() const;

    GameState state = GameState::Menu;
    PlayerBall player;
    Level level;
    UI ui;
    std::vector<ShardParticle> shards;
    float elapsedTime = 0.0f;
    float cameraX = 0.0f;
};
