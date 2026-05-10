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
    void drawForeground(float cameraX) const;

    void constrainPlayer(PlayerBall& player) const;
    bool triggerNearestTrap(PlayerBall& player);
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
    float trackMaxX = 3740.0f;
    float groundY = 610.0f;
};
