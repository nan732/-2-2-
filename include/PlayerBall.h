#pragma once

#include "raylib.h"

class PlayerBall {
public:
    void reset(float groundY);
    void update(float dt, float groundY);
    void draw(float cameraX) const;

    void clampToTrack(float minX, float maxX);
    void setPositionX(float x);
    Vector2 getPosition() const;
    float getRadius() const;
    float getSpeed() const;

private:
    Vector2 position{80.0f, 0.0f};
    Vector2 velocity{0.0f, 0.0f};
    float radius = 22.0f;
    float rollAngle = 0.0f;
    bool onGround = true;
};
