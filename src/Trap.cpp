#include "Trap.h"

#include "MathUtil.h"
#include "PlayerBall.h"

#include <cmath>
#include <utility>

namespace {
constexpr float Pi = 3.1415926535f;
constexpr float FurnaceDropFraction = 0.74f;

float easeInOut(float t)
{
    t = clampFloat(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

bool circleHitsSegment(Vector2 center, float radius, Vector2 a, Vector2 b, float thickness)
{
    const float abx = b.x - a.x;
    const float aby = b.y - a.y;
    const float apx = center.x - a.x;
    const float apy = center.y - a.y;
    const float abLenSq = abx * abx + aby * aby;
    if (abLenSq <= 0.001f) {
        const float dx = center.x - a.x;
        const float dy = center.y - a.y;
        return dx * dx + dy * dy <= (radius + thickness) * (radius + thickness);
    }
    const float t = clampFloat((apx * abx + apy * aby) / abLenSq, 0.0f, 1.0f);
    const Vector2 closest{a.x + abx * t, a.y + aby * t};
    const float dx = center.x - closest.x;
    const float dy = center.y - closest.y;
    return dx * dx + dy * dy <= (radius + thickness) * (radius + thickness);
}
}

Trap::Trap(std::string trapName, Vector2 trapCenter, float windupTime, float activeTime, float cooldownTime)
    : name(std::move(trapName)), center(trapCenter), windupDuration(windupTime), activeDuration(activeTime), cooldownDuration(cooldownTime)
{
}

void Trap::update(float dt)
{
    if (state == TrapState::Idle || state == TrapState::Spent) {
        return;
    }

    timer += dt;
    if (state == TrapState::Windup && timer >= windupDuration) {
        state = TrapState::Active;
        timer = 0.0f;
    } else if (state == TrapState::Active && timer >= activeDuration) {
        state = TrapState::Cooldown;
        timer = 0.0f;
    } else if (state == TrapState::Cooldown && timer >= cooldownDuration) {
        state = TrapState::Spent;
        timer = 0.0f;
    }
}

void Trap::constrainPlayer(PlayerBall& player) const
{
    (void)player;
}

bool Trap::trigger()
{
    if (!isAvailable()) {
        return false;
    }
    state = TrapState::Windup;
    timer = 0.0f;
    return true;
}

bool Trap::isAvailable() const
{
    return state == TrapState::Idle;
}

bool Trap::isBusy() const
{
    return state == TrapState::Windup || state == TrapState::Active;
}

bool Trap::isSpent() const
{
    return state == TrapState::Spent;
}

TrapState Trap::getState() const
{
    return state;
}

Vector2 Trap::getCenter() const
{
    return center;
}

const std::string& Trap::getName() const
{
    return name;
}

float Trap::getStateProgress() const
{
    if (state == TrapState::Windup) {
        return windupDuration <= 0.0f ? 1.0f : clampFloat(timer / windupDuration, 0.0f, 1.0f);
    }
    if (state == TrapState::Active) {
        return activeDuration <= 0.0f ? 1.0f : clampFloat(timer / activeDuration, 0.0f, 1.0f);
    }
    if (state == TrapState::Cooldown) {
        return cooldownDuration <= 0.0f ? 1.0f : clampFloat(timer / cooldownDuration, 0.0f, 1.0f);
    }
    return 0.0f;
}

float Trap::activeProgress() const
{
    return state == TrapState::Active ? getStateProgress() : 0.0f;
}

Color Trap::stateColor(bool highlighted) const
{
    if (state == TrapState::Windup) {
        return Color{255, 191, 0, 255};
    }
    if (state == TrapState::Active) {
        return RED;
    }
    if (state == TrapState::Cooldown) {
        return Color{120, 120, 120, 255};
    }
    if (state == TrapState::Spent) {
        return Color{210, 210, 210, 255};
    }
    return highlighted ? Color{0, 160, 80, 255} : BLACK;
}

HammerTrap::HammerTrap(Vector2 trapCenter)
    : Trap("Big Hammer", trapCenter, 0.18f, 0.72f, 0.28f)
{
}

float HammerTrap::hammerAngle() const
{
    constexpr float startAngle = -2.32f;
    constexpr float restAngle = -1.75f;
    constexpr float fullTurn = Pi * 2.0f;

    if (state == TrapState::Windup) {
        return lerpFloat(restAngle, startAngle, easeInOut(getStateProgress()));
    }
    if (state == TrapState::Active) {
        return startAngle + fullTurn * easeInOut(activeProgress());
    }
    if (state == TrapState::Cooldown) {
        return lerpFloat(startAngle + fullTurn, restAngle, easeInOut(getStateProgress()));
    }
    return restAngle;
}

Vector2 HammerTrap::hammerHead() const
{
    constexpr float armLength = 292.0f;
    const float angle = hammerAngle();
    return Vector2{
        center.x + std::cos(angle) * armLength,
        center.y + std::sin(angle) * armLength,
    };
}

void HammerTrap::draw(float cameraX, bool highlighted) const
{
    const Color color = stateColor(highlighted);
    const Vector2 pivot{center.x - cameraX, center.y};
    const Vector2 headWorld = hammerHead();
    const Vector2 head{headWorld.x - cameraX, headWorld.y};

    DrawCircleV(pivot, 13.0f, BLACK);
    DrawLineEx(pivot, head, 18.0f, color);
    DrawRectanglePro(Rectangle{head.x, head.y, 156.0f, 76.0f}, Vector2{78.0f, 38.0f}, hammerAngle() * 57.2958f, color);
    DrawRectangleLinesEx(Rectangle{center.x - cameraX - 175.0f, 328.0f, 350.0f, 285.0f}, 2.0f, color);

    if (state == TrapState::Spent) {
        DrawLine(static_cast<int>(center.x - cameraX - 165.0f), 346, static_cast<int>(center.x - cameraX + 165.0f), 598, Color{180, 180, 180, 255});
    }
}

bool HammerTrap::hitsPlayer(const PlayerBall& player) const
{
    if (state != TrapState::Active) {
        return false;
    }
    const Vector2 pivot = center;
    const Vector2 head = hammerHead();
    return circleHitsSegment(player.getPosition(), player.getRadius(), pivot, head, 34.0f)
        || circleHitsRect(player.getPosition(), player.getRadius(), Rectangle{head.x - 82.0f, head.y - 44.0f, 164.0f, 88.0f});
}

FurnaceTrap::FurnaceTrap(Vector2 trapCenter)
    : Trap("Furnace", trapCenter, 0.20f, 1.05f, 0.75f)
{
}

float FurnaceTrap::wallDrop() const
{
    if (state == TrapState::Windup) {
        return 0.0f;
    }
    if (state == TrapState::Active) {
        const float p = activeProgress();
        if (p < FurnaceDropFraction) {
            return easeInOut(p / FurnaceDropFraction);
        }
        return 1.0f;
    }
    if (state == TrapState::Cooldown) {
        return 1.0f - easeInOut(getStateProgress());
    }
    return 0.0f;
}

bool FurnaceTrap::wallsClosed() const
{
    return wallDrop() > 0.78f;
}

Rectangle FurnaceTrap::chamber() const
{
    return Rectangle{center.x - 110.0f, 390.0f, 220.0f, 220.0f};
}

Rectangle FurnaceTrap::leftWall() const
{
    const float drop = wallDrop();
    return Rectangle{center.x - 128.0f, lerpFloat(190.0f, 390.0f, drop), 18.0f, 220.0f};
}

Rectangle FurnaceTrap::rightWall() const
{
    const float drop = wallDrop();
    return Rectangle{center.x + 110.0f, lerpFloat(190.0f, 390.0f, drop), 18.0f, 220.0f};
}

void FurnaceTrap::draw(float cameraX, bool highlighted) const
{
    const Color color = stateColor(highlighted);
    const Rectangle box = chamber();
    DrawRectangleLinesEx(Rectangle{box.x - cameraX, box.y, box.width, box.height}, 2.0f, color);
    DrawRectangle(static_cast<int>(box.x - cameraX + 12.0f), static_cast<int>(box.y + 168.0f), static_cast<int>(box.width - 24.0f), 22, Color{20, 20, 20, 255});

    const Rectangle left = leftWall();
    const Rectangle right = rightWall();
    DrawRectangleRec(Rectangle{left.x - cameraX, left.y, left.width, left.height}, color);
    DrawRectangleRec(Rectangle{right.x - cameraX, right.y, right.width, right.height}, color);

    if (state == TrapState::Active && wallsClosed() && activeProgress() > 0.18f) {
        const float flame = std::sin(GetTime() * 18.0) * 8.0f;
        DrawRectangle(static_cast<int>(box.x - cameraX + 18.0f), 474, static_cast<int>(box.width - 36.0f), 116, Color{255, 96, 0, 210});
        DrawTriangle(Vector2{box.x - cameraX + 40.0f, 580.0f}, Vector2{box.x - cameraX + 72.0f, 488.0f + flame}, Vector2{box.x - cameraX + 104.0f, 580.0f}, YELLOW);
        DrawTriangle(Vector2{box.x - cameraX + 115.0f, 582.0f}, Vector2{box.x - cameraX + 155.0f, 500.0f - flame}, Vector2{box.x - cameraX + 195.0f, 582.0f}, Color{255, 220, 40, 255});
    }

    if (state == TrapState::Spent) {
        DrawText("spent", static_cast<int>(center.x - cameraX - 28.0f), 360, 18, Color{150, 150, 150, 255});
    }
}

bool FurnaceTrap::hitsPlayer(const PlayerBall& player) const
{
    if (state != TrapState::Active || activeProgress() < 0.18f || !wallsClosed()) {
        return false;
    }
    const Rectangle hotArea = chamber();
    return pointInRect(player.getPosition(), hotArea);
}

void FurnaceTrap::constrainPlayer(PlayerBall& player) const
{
    if (!wallsClosed()) {
        return;
    }

    const Vector2 position = player.getPosition();
    const float radius = player.getRadius();
    const Rectangle box = chamber();
    const bool verticallyRelevant = position.y + radius > box.y && position.y - radius < box.y + box.height;
    if (!verticallyRelevant) {
        return;
    }

    const float leftStop = box.x + radius;
    const float rightStop = box.x + box.width - radius;
    const bool inside = position.x > box.x && position.x < box.x + box.width;
    if (inside) {
        player.setPositionX(clampFloat(position.x, leftStop, rightStop));
        return;
    }

    if (position.x <= box.x && position.x + radius > box.x) {
        player.setPositionX(box.x - radius);
    } else if (position.x >= box.x + box.width && position.x - radius < box.x + box.width) {
        player.setPositionX(box.x + box.width + radius);
    }
}
