#include "玩家方块.h"

#include "数学工具.h"

#include <cmath>

void PlayerBall::reset(float groundY)
{
    position = Vector2{90.0f, groundY - radius};
    velocity = Vector2{0.0f, 0.0f};
    rollAngle = 0.0f;
    lean = 0.0f;
    inputAxis = 0.0f;
    onGround = true;
    launched = false;
}

void PlayerBall::update(float dt, float groundY)
{
    constexpr float maxSpeed = 265.0f;
    constexpr float acceleration = 1850.0f;
    constexpr float brakeAcceleration = 2750.0f;
    constexpr float friction = 22.0f;
    constexpr float gravity = 1850.0f;

    if (launched) {
        velocity.y += gravity * 0.72f * dt;
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        if (position.y >= groundY - radius) {
            position.y = groundY - radius;
            velocity.y = 0.0f;
            onGround = true;
            velocity.x *= 0.995f;
        }
        rollAngle += velocity.x * dt / radius;
        lean = lerpFloat(lean, clampFloat(velocity.x / maxSpeed, -1.0f, 1.0f), clampFloat(dt * 10.0f, 0.0f, 1.0f));
        return;
    }

    float targetInput = 0.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        targetInput -= 1.0f;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        targetInput += 1.0f;
    }

    constexpr float inputInertiaSeconds = 0.10f;
    inputAxis = lerpFloat(inputAxis, targetInput, clampFloat(dt / inputInertiaSeconds, 0.0f, 1.0f));
    const float input = inputAxis;
    const bool reversing = input * velocity.x < -1.0f;
    velocity.x += input * (reversing ? brakeAcceleration : acceleration) * dt;
    velocity.x = clampFloat(velocity.x, -maxSpeed, maxSpeed);

    if (onGround && std::fabs(input) < 0.01f) {
        const float drag = clampFloat(friction * dt, 0.0f, 1.0f);
        velocity.x *= 1.0f - drag;
    }

    velocity.y += gravity * dt;
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (position.y >= groundY - radius) {
        position.y = groundY - radius;
        velocity.y = 0.0f;
        onGround = true;
    }

    rollAngle += velocity.x * dt / radius;
    const float targetLean = clampFloat(velocity.x / maxSpeed, -1.0f, 1.0f);
    lean = lerpFloat(lean, targetLean, clampFloat(dt * 12.0f, 0.0f, 1.0f));
}

void PlayerBall::draw(float cameraX) const
{
    const Vector2 screen{position.x - cameraX, position.y};
    const float speedRatio = clampFloat(std::fabs(velocity.x) / 265.0f, 0.0f, 1.0f);
    const float leanOffset = lean * 5.5f;
    const float size = radius * 1.62f;
    const float left = screen.x + leanOffset - size * 0.5f;
    const float top = screen.y - size * 0.5f + 2.0f;
    const float slant = lean * 7.0f;

    DrawEllipse(static_cast<int>(screen.x + radius * 0.40f + leanOffset * 0.35f), static_cast<int>(screen.y + radius * 0.94f), radius * 1.05f, radius * 0.32f, Color{0, 0, 0, 98});
    DrawRectangleRec(Rectangle{left + 7.0f, top + 7.0f, size, size}, Color{0, 0, 0, 42});
    DrawRectangleRec(Rectangle{left + slant, top, size, size}, Color{0, 96, 224, 255});
    DrawRectangleRec(Rectangle{left + slant, top, size, 8.0f}, Color{123, 211, 255, 255});
    DrawRectangleRec(Rectangle{left + slant + size - 9.0f, top + 8.0f, 9.0f, size - 8.0f}, Color{0, 58, 175, 255});
    DrawRectangleLinesEx(Rectangle{left + slant, top, size, size}, 2.0f, BLACK);
    DrawRectangle(static_cast<int>(left + slant + 9.0f), static_cast<int>(top + 11.0f), 12, 7, Color{225, 246, 255, 225});
    DrawLineEx(Vector2{left + slant + 9.0f, top + size - 10.0f}, Vector2{left + slant + size - 11.0f, top + 13.0f}, 3.0f, Color{195, 229, 255, 160});

    if (speedRatio > 0.45f) {
        const float tail = velocity.x > 0.0f ? -1.0f : 1.0f;
        DrawRectangle(static_cast<int>(screen.x + tail * (radius + 6.0f)), static_cast<int>(screen.y - 7.0f), static_cast<int>(12.0f * speedRatio), 14, Color{88, 164, 240, 70});
        DrawRectangle(static_cast<int>(screen.x + tail * (radius + 18.0f)), static_cast<int>(screen.y - 4.0f), static_cast<int>(8.0f * speedRatio), 8, Color{88, 164, 240, 44});
    }
}

void PlayerBall::launch(Vector2 launchVelocity)
{
    launched = true;
    velocity = launchVelocity;
    inputAxis = 0.0f;
    onGround = false;
}

void PlayerBall::standOnSurface(float surfaceY)
{
    if (launched) {
        return;
    }
    position.y = surfaceY - radius;
    velocity.y = 0.0f;
    onGround = true;
}

void PlayerBall::clampToTrack(float minX, float maxX)
{
    position.x = clampFloat(position.x, minX + radius, maxX - radius);
}

void PlayerBall::setPositionX(float x)
{
    position.x = x;
}

Vector2 PlayerBall::getPosition() const
{
    return position;
}

float PlayerBall::getRadius() const
{
    return radius;
}

float PlayerBall::getSpeed() const
{
    return std::fabs(velocity.x);
}

bool PlayerBall::locksCamera() const
{
    return launched;
}
