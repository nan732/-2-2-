#pragma once

#include "Trap.h"

#include <memory>
#include <vector>

class PlayerBall;

class Level {
public:
    Level();

    void reset();
    void update(float dt);
    void draw(const PlayerBall& player, float cameraX, int screenWidth, int screenHeight) const;

    void constrainPlayer(PlayerBall& player) const;
    bool triggerNearestTrap(const PlayerBall& player);
    bool hitsPlayer(const PlayerBall& player) const;
    bool reachedFinish(const PlayerBall& player) const;

    int nearestTrapIndex(const PlayerBall& player) const;
    const Trap* nearestTrap(const PlayerBall& player) const;
    float getTrackMinX() const;
    float getTrackMaxX() const;
    float getGroundY() const;

private:
    std::vector<std::unique_ptr<Trap>> traps;
    float trackMinX = 40.0f;
    float trackMaxX = 2300.0f;
    float groundY = 610.0f;
};
