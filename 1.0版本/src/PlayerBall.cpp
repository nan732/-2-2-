#include "PlayerBall.h"

#include "MathUtil.h"

#include <cmath>

void PlayerBall::reset(float groundY)
{
    position = Vector2{90.0f, groundY - radius};
    velocity = Vector2{0.0f, 0.0f};
    rollAngle = 0.0f;
    onGround = true;
}

void PlayerBall::update(float dt, float groundY)
{
    constexpr float maxSpeed = 235.0f;
    constexpr float acceleration = 1250.0f;
    constexpr float friction = 15.0f;
    constexpr float airControl = 0.45f;
    constexpr float jumpVelocity = -620.0f;
    constexpr float gravity = 1850.0f;

    float input = 0.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        input -= 1.0f;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        input += 1.0f;
    }

    const float control = onGround ? 1.0f : airControl;
    velocity.x += input * acceleration * control * dt;
    velocity.x = clampFloat(velocity.x, -maxSpeed, maxSpeed);

    if (onGround && std::fabs(input) < 0.01f) {
        const float drag = clampFloat(friction * dt, 0.0f, 1.0f);
        velocity.x *= 1.0f - drag;
    }

    if (onGround && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))) {
        velocity.y = jumpVelocity;
        onGround = false;
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
}

void PlayerBall::draw(float cameraX) const
{
    const Vector2 screen{position.x - cameraX, position.y};
    DrawCircleV(screen, radius, Color{0, 116, 255, 255});
    DrawCircleLines(static_cast<int>(screen.x), static_cast<int>(screen.y), radius, BLACK);

    const float stripeAngle = rollAngle;
    const Vector2 a{
        screen.x + std::cos(stripeAngle) * radius * 0.82f,
        screen.y + std::sin(stripeAngle) * radius * 0.82f,
    };
    const Vector2 b{
        screen.x - std::cos(stripeAngle) * radius * 0.82f,
        screen.y - std::sin(stripeAngle) * radius * 0.82f,
    };
    DrawLineEx(a, b, 4.0f, Color{205, 232, 255, 255});
    DrawCircleV(Vector2{screen.x - radius * 0.25f, screen.y - radius * 0.35f}, radius * 0.22f, Color{130, 196, 255, 210});
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
