#include "Trap.h"

#include "MathUtil.h"
#include "PlayerBall.h"

#include <array>
#include <cmath>
#include <utility>

namespace {
constexpr float Pi = 3.1415926535f;
constexpr float FurnaceDropFraction = 0.60f;
constexpr Vector2 ShadowOffset{18.0f, 20.0f};
constexpr float GroundY = 610.0f;

float easeInOut(float t)
{
    t = clampFloat(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

Vector2 lerpVector(Vector2 a, Vector2 b, float t)
{
    return Vector2{lerpFloat(a.x, b.x, t), lerpFloat(a.y, b.y, t)};
}

float length(Vector2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float distancePointToSegment(Vector2 point, Vector2 a, Vector2 b)
{
    const Vector2 ab{b.x - a.x, b.y - a.y};
    const Vector2 ap{point.x - a.x, point.y - a.y};
    const float denom = ab.x * ab.x + ab.y * ab.y;
    if (denom <= 0.001f) {
        return length(Vector2{point.x - a.x, point.y - a.y});
    }
    const float t = clampFloat((ap.x * ab.x + ap.y * ab.y) / denom, 0.0f, 1.0f);
    const Vector2 closest{a.x + ab.x * t, a.y + ab.y * t};
    return length(Vector2{point.x - closest.x, point.y - closest.y});
}

void drawDangerSign(float worldX, float y, float cameraX, const char* label, Color accent)
{
    const float x = worldX - cameraX;
    DrawRectangle(static_cast<int>(x - 58.0f + ShadowOffset.x * 0.35f), static_cast<int>(y + ShadowOffset.y * 0.28f), 116, 38, Color{0, 0, 0, 74});
    DrawRectangle(static_cast<int>(x - 58.0f), static_cast<int>(y + 8.0f), 116, 28, Color{37, 36, 39, 240});
    DrawRectangle(static_cast<int>(x - 58.0f), static_cast<int>(y + 8.0f), 6, 28, accent);
    DrawRectangleLines(static_cast<int>(x - 58.0f), static_cast<int>(y + 8.0f), 116, 28, BLACK);
    DrawTriangle(Vector2{x - 47.0f, y + 2.0f}, Vector2{x - 34.0f, y + 25.0f}, Vector2{x - 60.0f, y + 25.0f}, accent);
    DrawText("!", static_cast<int>(x - 51.0f), static_cast<int>(y + 8.0f), 14, BLACK);
    const int width = MeasureText(label, 12);
    DrawText(label, static_cast<int>(x + 6.0f - static_cast<float>(width) * 0.5f), static_cast<int>(y + 16.0f), 12, RAYWHITE);
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

void Trap::drawForeground(float cameraX) const
{
    (void)cameraX;
}

bool Trap::trigger()
{
    if (!isAvailable()) {
        return false;
    }
    state = windupDuration <= 0.0f ? TrapState::Active : TrapState::Windup;
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
    : Trap("Meteor Hammer", trapCenter, 0.76f, 1.18f, 0.10f)
{
}

float HammerTrap::hammerAngle() const
{
    constexpr float startAngle = -0.16f;
    constexpr float endAngle = Pi + 0.16f;

    if (state == TrapState::Windup) {
        return lerpFloat(-0.58f, startAngle, easeInOut(getStateProgress()));
    }
    if (state == TrapState::Active) {
        return lerpFloat(startAngle, endAngle, easeInOut(activeProgress()));
    }
    if (state == TrapState::Cooldown) {
        return endAngle;
    }
    return startAngle;
}

Vector2 HammerTrap::hammerHead() const
{
    constexpr float armLength = 252.0f;
    const float angle = hammerAngle();
    return Vector2{
        center.x + std::cos(angle) * armLength,
        center.y + std::sin(angle) * armLength,
    };
}

void HammerTrap::draw(float cameraX, bool highlighted) const
{
    drawDangerSign(center.x, center.y - 112.0f, cameraX, "METEOR", Color{255, 196, 55, 255});

    const Vector2 pivot{center.x - cameraX, center.y};
    const float iconX = center.x - cameraX - 135.0f;
    DrawCircleV(Vector2{iconX, GroundY - 74.0f}, 7.0f, Color{28, 28, 32, 255});
    DrawLineEx(Vector2{iconX, GroundY - 72.0f}, Vector2{iconX + 27.0f, GroundY - 48.0f}, 3.0f, Color{58, 58, 64, 255});
    DrawCircleV(Vector2{iconX + 31.0f, GroundY - 44.0f}, 11.0f, Color{205, 76, 45, 255});
    DrawCircleLines(static_cast<int>(iconX + 31.0f), static_cast<int>(GroundY - 44.0f), 11.0f, BLACK);
    DrawEllipse(static_cast<int>(pivot.x + 19.0f), static_cast<int>(GroundY - 2.0f), 54.0f, 12.0f, Color{0, 0, 0, 62});
    DrawRectangle(static_cast<int>(pivot.x - 12.0f), static_cast<int>(center.y - 18.0f), 24, static_cast<int>(GroundY - center.y + 18.0f), Color{43, 43, 49, 255});
    DrawRectangle(static_cast<int>(pivot.x + 12.0f), static_cast<int>(center.y - 8.0f), 12, static_cast<int>(GroundY - center.y + 8.0f), Color{25, 25, 31, 255});
    DrawRectangle(static_cast<int>(pivot.x - 66.0f), static_cast<int>(center.y - 28.0f), 132, 18, Color{55, 55, 62, 255});
    DrawRectangle(static_cast<int>(pivot.x - 56.0f), static_cast<int>(center.y - 23.0f), 112, 4, Color{255, 255, 255, 48});

    if (state == TrapState::Cooldown || state == TrapState::Spent) {
        return;
    }

    const Color color = stateColor(highlighted);
    const Vector2 headWorld = hammerHead();
    const Vector2 head{headWorld.x - cameraX, headWorld.y};
    const Vector2 pivotShadow{pivot.x + ShadowOffset.x, pivot.y + ShadowOffset.y};
    const Vector2 headShadow{head.x + ShadowOffset.x, head.y + ShadowOffset.y};
    const float ballRadius = 32.0f;

    if (state == TrapState::Windup) {
        const float glow = 0.45f + 0.55f * getStateProgress();
        DrawRing(pivot, 226.0f, 231.0f, 350.0f, 22.0f, 28, Color{255, 191, 0, static_cast<unsigned char>(72.0f * glow)});
        DrawText("LOCK", static_cast<int>(pivot.x + 38.0f), static_cast<int>(pivot.y - 64.0f), 14, Color{255, 210, 80, static_cast<unsigned char>(150.0f * glow)});
    }

    DrawLineEx(pivotShadow, headShadow, 14.0f, Color{0, 0, 0, 58});
    DrawCircleV(headShadow, ballRadius + 5.0f, Color{0, 0, 0, 88});
    DrawCircleV(pivotShadow, 13.0f, Color{0, 0, 0, 76});

    DrawCircleV(pivot, 12.0f, Color{18, 18, 22, 255});
    for (int i = 1; i <= 9; ++i) {
        const float t = static_cast<float>(i) / 10.0f;
        const Vector2 link{lerpFloat(pivot.x, head.x, t), lerpFloat(pivot.y, head.y, t)};
        DrawCircleV(link, 5.2f, Color{52, 52, 58, 255});
        DrawCircleLines(static_cast<int>(link.x), static_cast<int>(link.y), 5.2f, Color{12, 12, 14, 255});
    }
    DrawLineEx(pivot, head, 4.0f, Color{74, 74, 82, 255});
    DrawCircleGradient(static_cast<int>(head.x), static_cast<int>(head.y), ballRadius, Color{255, 174, 86, 255}, color);
    DrawCircleLines(static_cast<int>(head.x), static_cast<int>(head.y), ballRadius, BLACK);
    DrawCircleV(Vector2{head.x - ballRadius * 0.32f, head.y - ballRadius * 0.36f}, ballRadius * 0.22f, Color{255, 245, 220, 190});
    DrawCircleV(Vector2{head.x + ballRadius * 0.35f, head.y + ballRadius * 0.38f}, ballRadius * 0.34f, Color{45, 12, 8, 55});
}

bool HammerTrap::hitsPlayer(const PlayerBall& player) const
{
    if (state != TrapState::Active) {
        return false;
    }
    const Vector2 head = hammerHead();
    const Vector2 playerPosition = player.getPosition();
    const float hitRadius = player.getRadius() + 31.0f;
    const float dx = playerPosition.x - head.x;
    const float dy = playerPosition.y - head.y;
    return dx * dx + dy * dy <= hitRadius * hitRadius;
}

FurnaceTrap::FurnaceTrap(Vector2 trapCenter)
    : Trap("Furnace", trapCenter, 0.22f, 1.15f, 0.62f)
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
    return wallDrop() >= 0.995f;
}

Rectangle FurnaceTrap::chamber() const
{
    return Rectangle{center.x - 122.0f, 426.0f, 244.0f, 184.0f};
}

Rectangle FurnaceTrap::leftWall() const
{
    const float drop = wallDrop();
    return Rectangle{center.x - 142.0f, lerpFloat(242.0f, 426.0f, drop), 20.0f, 184.0f};
}

Rectangle FurnaceTrap::rightWall() const
{
    const float drop = wallDrop();
    return Rectangle{center.x + 122.0f, lerpFloat(242.0f, 426.0f, drop), 20.0f, 184.0f};
}

void FurnaceTrap::draw(float cameraX, bool highlighted) const
{
    const Color color = stateColor(highlighted);
    const Rectangle box = chamber();
    drawDangerSign(center.x, box.y - 52.0f, cameraX, "FIRE", Color{255, 105, 45, 255});
    DrawRectangle(static_cast<int>(box.x - cameraX + 6.0f + ShadowOffset.x * 0.45f), static_cast<int>(box.y + 163.0f + ShadowOffset.y * 0.25f), static_cast<int>(box.width - 12.0f), 34, Color{0, 0, 0, 74});
    DrawRectangle(static_cast<int>(box.x - cameraX + 6.0f), static_cast<int>(box.y + 162.0f), static_cast<int>(box.width - 12.0f), 28, Color{21, 20, 20, 255});
    DrawRectangle(static_cast<int>(box.x - cameraX + 18.0f), static_cast<int>(box.y + 166.0f), static_cast<int>(box.width - 36.0f), 5, Color{150, 98, 58, 170});
    DrawRectangle(static_cast<int>(box.x - cameraX - 12.0f), static_cast<int>(box.y + 26.0f), 12, 164, Color{70, 47, 38, 255});
    DrawRectangle(static_cast<int>(box.x - cameraX + box.width), static_cast<int>(box.y + 26.0f), 12, 164, Color{41, 29, 27, 255});

    if (state == TrapState::Windup) {
        const unsigned char alpha = static_cast<unsigned char>(80.0f + 135.0f * getStateProgress());
        DrawRectangle(static_cast<int>(box.x - cameraX + 18.0f), static_cast<int>(box.y + 140.0f), static_cast<int>(box.width - 36.0f), 8, Color{255, 174, 32, alpha});
        DrawText("SEALING", static_cast<int>(center.x - cameraX - 34.0f), static_cast<int>(box.y - 18.0f), 13, Color{255, 198, 72, alpha});
    }

    const Rectangle left = leftWall();
    const Rectangle right = rightWall();
    DrawRectangleRec(Rectangle{left.x - cameraX + ShadowOffset.x, left.y + ShadowOffset.y, left.width, left.height}, Color{0, 0, 0, 82});
    DrawRectangleRec(Rectangle{right.x - cameraX + ShadowOffset.x, right.y + ShadowOffset.y, right.width, right.height}, Color{0, 0, 0, 82});
    DrawRectangleRec(Rectangle{left.x - cameraX, left.y, left.width, left.height}, color);
    DrawRectangleRec(Rectangle{right.x - cameraX, right.y, right.width, right.height}, color);
    DrawRectangle(static_cast<int>(left.x - cameraX + 3.0f), static_cast<int>(left.y + 6.0f), 4, static_cast<int>(left.height - 12.0f), Color{255, 255, 255, 75});
    DrawRectangle(static_cast<int>(right.x - cameraX + 3.0f), static_cast<int>(right.y + 6.0f), 4, static_cast<int>(right.height - 12.0f), Color{255, 255, 255, 75});

    if (state == TrapState::Active && wallsClosed() && activeProgress() > 0.18f) {
        const float flame = std::sin(GetTime() * 18.0) * 8.0f;
        DrawRectangle(static_cast<int>(box.x - cameraX + 16.0f), static_cast<int>(box.y + 56.0f), static_cast<int>(box.width - 32.0f), static_cast<int>(box.height - 64.0f), Color{255, 78, 0, 218});
        DrawTriangle(Vector2{box.x - cameraX + 34.0f, box.y + 156.0f}, Vector2{box.x - cameraX + 76.0f, box.y + 70.0f + flame}, Vector2{box.x - cameraX + 118.0f, box.y + 156.0f}, YELLOW);
        DrawTriangle(Vector2{box.x - cameraX + 112.0f, box.y + 158.0f}, Vector2{box.x - cameraX + 164.0f, box.y + 80.0f - flame}, Vector2{box.x - cameraX + 216.0f, box.y + 158.0f}, Color{255, 220, 40, 255});
        DrawTriangle(Vector2{box.x - cameraX + 70.0f, box.y + 162.0f}, Vector2{box.x - cameraX + 126.0f, box.y + 96.0f + flame * 0.4f}, Vector2{box.x - cameraX + 182.0f, box.y + 162.0f}, Color{255, 246, 164, 235});
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

LaserPortTrap::LaserPortTrap(Vector2 trapCenter)
    : Trap("Laser Port", trapCenter, 0.0f, 0.88f, 0.16f)
{
}

Vector2 LaserPortTrap::laserPoint(float progress) const
{
    const std::array<Vector2, 5> points{{
        Vector2{center.x - 275.0f, GroundY - 40.0f},
        Vector2{center.x - 170.0f, GroundY - 146.0f},
        Vector2{center.x - 35.0f, GroundY - 82.0f},
        Vector2{center.x + 108.0f, GroundY - 176.0f},
        Vector2{center.x + 262.0f, GroundY - 38.0f},
    }};

    float total = 0.0f;
    std::array<float, 4> segmentLengths{};
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        segmentLengths[i] = length(Vector2{points[i + 1].x - points[i].x, points[i + 1].y - points[i].y});
        total += segmentLengths[i];
    }

    float remaining = clampFloat(progress, 0.0f, 1.0f) * total;
    for (size_t i = 0; i < segmentLengths.size(); ++i) {
        if (remaining <= segmentLengths[i]) {
            return lerpVector(points[i], points[i + 1], remaining / segmentLengths[i]);
        }
        remaining -= segmentLengths[i];
    }
    return points.back();
}

void LaserPortTrap::draw(float cameraX, bool highlighted) const
{
    const Color color = stateColor(highlighted);
    drawDangerSign(center.x, GroundY - 128.0f, cameraX, "LASER", Color{255, 72, 72, 255});

    const float x = center.x - cameraX;
    DrawEllipse(static_cast<int>(x - 270.0f), static_cast<int>(GroundY - 8.0f), 38.0f, 11.0f, Color{0, 0, 0, 70});
    DrawRectangle(static_cast<int>(x - 305.0f), static_cast<int>(GroundY - 82.0f), 58, 68, Color{38, 39, 44, 255});
    DrawRectangle(static_cast<int>(x - 247.0f), static_cast<int>(GroundY - 74.0f), 14, 60, Color{20, 21, 26, 255});
    DrawRectangle(static_cast<int>(x - 311.0f), static_cast<int>(GroundY - 88.0f), 78, 18, Color{30, 30, 34, 255});
    DrawRectangle(static_cast<int>(x - 311.0f), static_cast<int>(GroundY - 88.0f), 6, 18, Color{255, 219, 75, 255});
    DrawCircleV(Vector2{x - 276.0f, GroundY - 40.0f}, 19.0f, BLACK);
    DrawCircleV(Vector2{x - 276.0f, GroundY - 40.0f}, 11.0f, state == TrapState::Idle ? Color{92, 0, 0, 255} : color);

    const std::array<Vector2, 3> mirrors{{
        Vector2{x - 170.0f, GroundY - 146.0f},
        Vector2{x - 35.0f, GroundY - 82.0f},
        Vector2{x + 108.0f, GroundY - 176.0f},
    }};
    for (size_t i = 0; i < mirrors.size(); ++i) {
        const Vector2 m = mirrors[i];
        DrawEllipse(static_cast<int>(m.x + 9.0f), static_cast<int>(m.y + 31.0f), 24.0f, 7.0f, Color{0, 0, 0, 58});
        DrawRectangle(static_cast<int>(m.x - 22.0f), static_cast<int>(m.y - 14.0f), 44, 28, Color{36, 37, 41, 255});
        DrawLineEx(Vector2{m.x - 16.0f, m.y + (i == 1 ? 10.0f : -10.0f)}, Vector2{m.x + 16.0f, m.y + (i == 1 ? -10.0f : 10.0f)}, 5.0f, Color{194, 224, 235, 255});
        DrawRectangleLines(static_cast<int>(m.x - 22.0f), static_cast<int>(m.y - 14.0f), 44, 28, BLACK);
    }

    if (state == TrapState::Active) {
        DrawCircleGradient(static_cast<int>(x - 276.0f), static_cast<int>(GroundY - 40.0f), 32.0f, Color{255, 42, 42, 170}, Color{255, 42, 42, 0});
    }
}

void LaserPortTrap::drawForeground(float cameraX) const
{
    if (state != TrapState::Active) {
        return;
    }

    const float progress = easeInOut(activeProgress());
    const Vector2 dot = laserPoint(progress);
    constexpr int trailCount = 9;
    for (int i = trailCount; i >= 1; --i) {
        const float trailProgress = clampFloat(progress - static_cast<float>(i) * 0.035f, 0.0f, 1.0f);
        const Vector2 tail = laserPoint(trailProgress);
        const unsigned char alpha = static_cast<unsigned char>(25 + (trailCount - i) * 18);
        DrawLineEx(Vector2{tail.x - cameraX, tail.y}, Vector2{dot.x - cameraX, dot.y}, 7.0f - static_cast<float>(i) * 0.35f, Color{255, 18, 14, alpha});
    }
    DrawCircleGradient(static_cast<int>(dot.x - cameraX), static_cast<int>(dot.y), 32.0f, Color{255, 36, 32, 220}, Color{255, 36, 32, 0});
    DrawCircleV(Vector2{dot.x - cameraX, dot.y}, 10.0f, Color{255, 24, 20, 255});
    DrawCircleV(Vector2{dot.x - cameraX - 3.0f, dot.y - 4.0f}, 4.0f, Color{255, 244, 228, 245});
}

bool LaserPortTrap::trailHitsPlayer(const PlayerBall& player) const
{
    const std::array<Vector2, 5> points{{
        Vector2{center.x - 275.0f, GroundY - 40.0f},
        Vector2{center.x - 170.0f, GroundY - 146.0f},
        Vector2{center.x - 35.0f, GroundY - 82.0f},
        Vector2{center.x + 108.0f, GroundY - 176.0f},
        Vector2{center.x + 262.0f, GroundY - 38.0f},
    }};

    std::array<float, 4> segmentLengths{};
    float total = 0.0f;
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        segmentLengths[i] = length(Vector2{points[i + 1].x - points[i].x, points[i + 1].y - points[i].y});
        total += segmentLengths[i];
    }

    const float headDistance = easeInOut(activeProgress()) * total;
    const float tailDistance = clampFloat(headDistance - 155.0f, 0.0f, total);
    float segmentStart = 0.0f;
    const Vector2 playerPosition = player.getPosition();
    const float hitRadius = player.getRadius() + 12.0f;

    for (size_t i = 0; i < segmentLengths.size(); ++i) {
        const float segmentEnd = segmentStart + segmentLengths[i];
        const float overlapStart = clampFloat(tailDistance, segmentStart, segmentEnd);
        const float overlapEnd = clampFloat(headDistance, segmentStart, segmentEnd);
        if (overlapEnd > overlapStart + 0.01f) {
            const float a = (overlapStart - segmentStart) / segmentLengths[i];
            const float b = (overlapEnd - segmentStart) / segmentLengths[i];
            const Vector2 start = lerpVector(points[i], points[i + 1], a);
            const Vector2 end = lerpVector(points[i], points[i + 1], b);
            if (distancePointToSegment(playerPosition, start, end) <= hitRadius) {
                return true;
            }
        }
        segmentStart = segmentEnd;
    }

    return false;
}

bool LaserPortTrap::hitsPlayer(const PlayerBall& player) const
{
    if (state != TrapState::Active) {
        return false;
    }
    return trailHitsPlayer(player);
}

DepthRamTrap::DepthRamTrap(Vector2 trapCenter)
    : Trap("Depth Ram", trapCenter, 0.70f, 0.74f, 0.24f)
{
}

float DepthRamTrap::ramDepth() const
{
    if (state == TrapState::Windup) {
        return 0.0f;
    }
    if (state == TrapState::Active) {
        const float p = activeProgress();
        if (p < 0.35f) {
            return easeInOut(p / 0.35f);
        }
        if (p < 0.66f) {
            return 1.0f;
        }
        return 1.0f - easeInOut((p - 0.66f) / 0.34f);
    }
    return 0.0f;
}

Rectangle DepthRamTrap::hitRect() const
{
    const float depth = ramDepth();
    return Rectangle{center.x - 76.0f, GroundY - 112.0f + 38.0f * (1.0f - depth), 152.0f, 96.0f * depth};
}

void DepthRamTrap::draw(float cameraX, bool highlighted) const
{
    const Color color = stateColor(highlighted);
    const float x = center.x - cameraX;
    const float depth = ramDepth();
    drawDangerSign(center.x, GroundY - 164.0f, cameraX, "PISTON", Color{255, 210, 55, 255});

    DrawEllipse(static_cast<int>(x + 22.0f), static_cast<int>(GroundY - 4.0f), 94.0f, 18.0f, Color{0, 0, 0, static_cast<unsigned char>(55.0f + 55.0f * depth)});
    DrawRectangle(static_cast<int>(x - 94.0f), static_cast<int>(GroundY - 174.0f), 188, 22, Color{232, 235, 232, 255});
    DrawRectangle(static_cast<int>(x + 94.0f), static_cast<int>(GroundY - 164.0f), 24, 152, Color{182, 187, 186, 255});
    DrawRectangle(static_cast<int>(x - 112.0f), static_cast<int>(GroundY - 152.0f), 24, 140, Color{245, 247, 244, 255});
    DrawLineEx(Vector2{x - 70.0f, GroundY - 116.0f}, Vector2{x + 70.0f, GroundY - 116.0f}, 4.0f, Color{188, 193, 192, 255});
    DrawLineEx(Vector2{x - 70.0f, GroundY - 76.0f}, Vector2{x + 70.0f, GroundY - 76.0f}, 4.0f, Color{188, 193, 192, 255});

    if (state == TrapState::Windup) {
        const unsigned char alpha = static_cast<unsigned char>(70.0f + 150.0f * getStateProgress());
        DrawRectangle(static_cast<int>(x - 68.0f), static_cast<int>(GroundY - 142.0f), 136, 8, Color{255, 210, 60, alpha});
        DrawLineEx(Vector2{x - 58.0f, GroundY - 132.0f}, Vector2{x + 58.0f, GroundY - 86.0f}, 4.0f, Color{255, 210, 60, alpha});
        DrawLineEx(Vector2{x + 58.0f, GroundY - 132.0f}, Vector2{x - 58.0f, GroundY - 86.0f}, 4.0f, Color{255, 210, 60, alpha});
    }

    const Rectangle ram = hitRect();
    const float screenY = ram.y;
    const float frontWidth = 92.0f + 70.0f * depth;
    const float frontHeight = 28.0f + 68.0f * depth;
    DrawRectangleRec(Rectangle{x - frontWidth * 0.5f + ShadowOffset.x * 0.35f, screenY + ShadowOffset.y * 0.32f, frontWidth, frontHeight}, Color{0, 0, 0, 72});
    DrawRectangleRec(Rectangle{x - frontWidth * 0.5f, screenY, frontWidth, frontHeight}, state == TrapState::Active ? color : Color{218, 222, 218, 255});
    DrawRectangle(static_cast<int>(x - frontWidth * 0.5f), static_cast<int>(screenY), static_cast<int>(frontWidth), 9, Color{255, 255, 255, 92});
    DrawRectangleLinesEx(Rectangle{x - frontWidth * 0.5f, screenY, frontWidth, frontHeight}, 2.0f, BLACK);
    DrawText("PUSH", static_cast<int>(x - 18.0f), static_cast<int>(screenY + frontHeight * 0.42f), 10, Color{42, 42, 45, 220});
}

bool DepthRamTrap::hitsPlayer(const PlayerBall& player) const
{
    if (state != TrapState::Active || ramDepth() < 0.72f) {
        return false;
    }
    return circleHitsRect(player.getPosition(), player.getRadius(), hitRect());
}

DropTileTrap::DropTileTrap(Vector2 trapCenter)
    : Trap("Drop Tile", trapCenter, 0.62f, 1.08f, 0.22f)
{
}

float DropTileTrap::openAmount() const
{
    if (state == TrapState::Windup) {
        return 0.0f;
    }
    if (state == TrapState::Active) {
        const float p = activeProgress();
        if (p < 0.22f) {
            return easeInOut(p / 0.22f);
        }
        if (p < 0.78f) {
            return 1.0f;
        }
        return 1.0f - easeInOut((p - 0.78f) / 0.22f);
    }
    return 0.0f;
}

Rectangle DropTileTrap::gapRect() const
{
    return Rectangle{center.x - 108.0f, GroundY - 54.0f, 216.0f, 72.0f};
}

void DropTileTrap::draw(float cameraX, bool highlighted) const
{
    const Color color = stateColor(highlighted);
    const Rectangle gap = gapRect();
    const float x = gap.x - cameraX;
    const float open = openAmount();
    drawDangerSign(center.x, GroundY - 132.0f, cameraX, "PIT", Color{255, 210, 55, 255});

    DrawRectangle(static_cast<int>(x + 10.0f), static_cast<int>(gap.y + 42.0f), static_cast<int>(gap.width - 20.0f), 46, Color{0, 0, 0, static_cast<unsigned char>(90.0f + 120.0f * open)});
    DrawRectangle(static_cast<int>(x - 8.0f), static_cast<int>(GroundY - 24.0f), static_cast<int>(gap.width + 16.0f), 16, Color{31, 30, 34, 255});

    if (state == TrapState::Windup) {
        const unsigned char alpha = static_cast<unsigned char>(65.0f + 145.0f * getStateProgress());
        DrawRectangle(static_cast<int>(x + 12.0f), static_cast<int>(GroundY - 66.0f), static_cast<int>(gap.width - 24.0f), 8, Color{255, 210, 60, alpha});
        DrawText("FLOOR", static_cast<int>(center.x - cameraX - 24.0f), static_cast<int>(GroundY - 88.0f), 13, Color{255, 218, 90, alpha});
    }

    const float split = 10.0f + 58.0f * open;
    const Vector2 leftA{x, GroundY - 58.0f};
    const Vector2 leftB{x + gap.width * 0.5f - split, GroundY - 58.0f + 24.0f * open};
    const Vector2 leftC{x + gap.width * 0.5f - split, GroundY - 12.0f + 58.0f * open};
    const Vector2 leftD{x, GroundY - 12.0f};
    const Vector2 rightA{x + gap.width, GroundY - 58.0f};
    const Vector2 rightB{x + gap.width * 0.5f + split, GroundY - 58.0f + 24.0f * open};
    const Vector2 rightC{x + gap.width * 0.5f + split, GroundY - 12.0f + 58.0f * open};
    const Vector2 rightD{x + gap.width, GroundY - 12.0f};

    DrawTriangle(leftA, leftB, leftC, state == TrapState::Active ? color : Color{132, 123, 103, 255});
    DrawTriangle(leftA, leftC, leftD, Color{102, 95, 86, 255});
    DrawTriangle(rightA, rightB, rightC, state == TrapState::Active ? color : Color{132, 123, 103, 255});
    DrawTriangle(rightA, rightC, rightD, Color{102, 95, 86, 255});
    DrawLineEx(leftA, leftB, 2.0f, BLACK);
    DrawLineEx(rightA, rightB, 2.0f, BLACK);
    DrawText("TRAPDOOR", static_cast<int>(center.x - cameraX - 38.0f), static_cast<int>(GroundY - 42.0f + 46.0f * open), 10, Color{235, 235, 230, 210});
}

bool DropTileTrap::hitsPlayer(const PlayerBall& player) const
{
    if (state != TrapState::Active || openAmount() < 0.76f) {
        return false;
    }
    return circleHitsRect(player.getPosition(), player.getRadius(), gapRect());
}
