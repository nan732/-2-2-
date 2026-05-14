#include "陷阱.h"

#include "数学工具.h"
#include "玩家方块.h"

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
    DrawRectangle(static_cast<int>(x - 78.0f + ShadowOffset.x * 0.35f), static_cast<int>(y + ShadowOffset.y * 0.28f), 156, 46, Color{0, 0, 0, 80});
    DrawRectangle(static_cast<int>(x - 78.0f), static_cast<int>(y + 4.0f), 156, 40, Color{37, 36, 39, 245});
    DrawRectangle(static_cast<int>(x - 78.0f), static_cast<int>(y + 4.0f), 8, 40, accent);
    DrawRectangleLines(static_cast<int>(x - 78.0f), static_cast<int>(y + 4.0f), 156, 40, BLACK);
    DrawTriangle(Vector2{x - 62.0f, y + 10.0f}, Vector2{x - 50.0f, y + 32.0f}, Vector2{x - 74.0f, y + 32.0f}, accent);
    DrawText("!", static_cast<int>(x - 66.0f), static_cast<int>(y + 15.0f), 14, BLACK);
    DrawText(label, static_cast<int>(x - 38.0f), static_cast<int>(y + 10.0f), 12, RAYWHITE);
    DrawText("P TRIGGER", static_cast<int>(x - 38.0f), static_cast<int>(y + 26.0f), 10, Color{255, 222, 105, 255});
}

void drawRoadWarningSign(float worldX, float groundY, float cameraX)
{
    const float x = worldX - cameraX;
    const float y = groundY - 168.0f;
    DrawRectangle(static_cast<int>(x - 4.0f), static_cast<int>(y + 42.0f), 8, 100, Color{50, 44, 34, 255});
    DrawRectangle(static_cast<int>(x - 28.0f), static_cast<int>(y + 134.0f), 56, 10, Color{45, 42, 38, 255});
    DrawEllipse(static_cast<int>(x + 10.0f), static_cast<int>(y + 146.0f), 34.0f, 7.0f, Color{0, 0, 0, 48});

    DrawRectanglePro(Rectangle{x + 7.0f, y + 9.0f, 84.0f, 84.0f}, Vector2{42.0f, 42.0f}, 45.0f, Color{0, 0, 0, 70});
    DrawRectanglePro(Rectangle{x, y, 84.0f, 84.0f}, Vector2{42.0f, 42.0f}, 45.0f, Color{255, 213, 58, 255});
    DrawRectanglePro(Rectangle{x, y, 72.0f, 72.0f}, Vector2{36.0f, 36.0f}, 45.0f, Color{255, 225, 85, 255});
    DrawRectanglePro(Rectangle{x, y, 84.0f, 84.0f}, Vector2{42.0f, 42.0f}, 45.0f, Color{0, 0, 0, 0});

    DrawLineEx(Vector2{x - 31.0f, y + 15.0f}, Vector2{x + 29.0f, y + 3.0f}, 5.0f, BLACK);
    DrawCircleV(Vector2{x, y + 22.0f}, 5.0f, BLACK);
    DrawRectangle(static_cast<int>(x + 16.0f), static_cast<int>(y - 28.0f), 25, 20, BLACK);
    DrawLineEx(Vector2{x + 28.0f, y - 8.0f}, Vector2{x + 28.0f, y + 4.0f}, 3.0f, BLACK);
    DrawTriangle(Vector2{x + 12.0f, y + 34.0f}, Vector2{x + 24.0f, y + 34.0f}, Vector2{x + 18.0f, y + 44.0f}, BLACK);
    DrawText("TEETER", static_cast<int>(x - 31.0f), static_cast<int>(y + 56.0f), 10, BLACK);
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

void Trap::onTriggered(PlayerBall& player)
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
    drawDangerSign(center.x, center.y - 112.0f, cameraX, "HAMMER", Color{255, 196, 55, 255});

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
    drawDangerSign(center.x, box.y - 52.0f, cameraX, "FURNACE", Color{255, 105, 45, 255});
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
    : Trap("Mirror Laser", trapCenter, 0.0f, 0.92f, 0.16f)
{
}

Vector2 LaserPortTrap::laserPoint(float progress) const
{
    constexpr float runY = GroundY - 22.0f;
    const std::array<Vector2, 6> points{{
        Vector2{center.x - 300.0f, runY - 66.0f},
        Vector2{center.x - 150.0f, runY},
        Vector2{center.x, runY - 66.0f},
        Vector2{center.x + 150.0f, runY},
        Vector2{center.x + 300.0f, runY - 66.0f},
        Vector2{center.x + 450.0f, runY},
    }};

    float total = 0.0f;
    std::array<float, 5> segmentLengths{};
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
    drawDangerSign(center.x, GroundY - 136.0f, cameraX, "MIRROR LASER", Color{255, 72, 72, 255});

    const float x = center.x - cameraX;
    constexpr float runY = GroundY - 22.0f;
    DrawEllipse(static_cast<int>(x - 294.0f), static_cast<int>(runY - 34.0f), 38.0f, 11.0f, Color{0, 0, 0, 70});
    DrawRectangle(static_cast<int>(x - 335.0f), static_cast<int>(runY - 116.0f), 64, 74, Color{38, 39, 44, 255});
    DrawRectangle(static_cast<int>(x - 271.0f), static_cast<int>(runY - 106.0f), 14, 64, Color{20, 21, 26, 255});
    DrawRectangle(static_cast<int>(x - 342.0f), static_cast<int>(runY - 122.0f), 88, 18, Color{30, 30, 34, 255});
    DrawRectangle(static_cast<int>(x - 342.0f), static_cast<int>(runY - 122.0f), 6, 18, Color{255, 219, 75, 255});
    DrawCircleV(Vector2{x - 300.0f, runY - 66.0f}, 19.0f, BLACK);
    DrawCircleV(Vector2{x - 300.0f, runY - 66.0f}, 11.0f, state == TrapState::Idle ? Color{92, 0, 0, 255} : color);

    const std::array<Vector2, 3> mirrors{{
        Vector2{x - 150.0f, runY},
        Vector2{x, runY - 66.0f},
        Vector2{x + 150.0f, runY},
    }};
    for (size_t i = 0; i < mirrors.size(); ++i) {
        const Vector2 m = mirrors[i];
        DrawEllipse(static_cast<int>(m.x + 9.0f), static_cast<int>(m.y + 31.0f), 24.0f, 7.0f, Color{0, 0, 0, 58});
        DrawRectangle(static_cast<int>(m.x - 22.0f), static_cast<int>(m.y - 14.0f), 44, 28, Color{36, 37, 41, 255});
        DrawLineEx(Vector2{m.x - 16.0f, m.y + (i == 1 ? 10.0f : -10.0f)}, Vector2{m.x + 16.0f, m.y + (i == 1 ? -10.0f : 10.0f)}, 5.0f, Color{194, 224, 235, 255});
        DrawRectangleLines(static_cast<int>(m.x - 22.0f), static_cast<int>(m.y - 14.0f), 44, 28, BLACK);
    }
    DrawRectangle(static_cast<int>(x + 275.0f), static_cast<int>(runY - 92.0f), 50, 32, Color{36, 37, 41, 255});
    DrawLineEx(Vector2{x + 282.0f, runY - 84.0f}, Vector2{x + 318.0f, runY - 66.0f}, 5.0f, Color{194, 224, 235, 255});
    DrawRectangleLines(static_cast<int>(x + 275.0f), static_cast<int>(runY - 92.0f), 50, 32, BLACK);

    if (state == TrapState::Active) {
        DrawCircleGradient(static_cast<int>(x - 300.0f), static_cast<int>(runY - 66.0f), 32.0f, Color{255, 42, 42, 170}, Color{255, 42, 42, 0});
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
    Vector2 previous = dot;
    for (int i = 1; i <= trailCount; ++i) {
        const float trailProgress = clampFloat(progress - static_cast<float>(i) * 0.035f, 0.0f, 1.0f);
        const Vector2 tail = laserPoint(trailProgress);
        const unsigned char alpha = static_cast<unsigned char>(168 - i * 13);
        DrawLineEx(Vector2{tail.x - cameraX, tail.y}, Vector2{previous.x - cameraX, previous.y}, 7.0f - static_cast<float>(i) * 0.35f, Color{255, 18, 14, alpha});
        previous = tail;
    }
    DrawCircleGradient(static_cast<int>(dot.x - cameraX), static_cast<int>(dot.y), 32.0f, Color{255, 36, 32, 220}, Color{255, 36, 32, 0});
    DrawCircleV(Vector2{dot.x - cameraX, dot.y}, 10.0f, Color{255, 24, 20, 255});
    DrawCircleV(Vector2{dot.x - cameraX - 3.0f, dot.y - 4.0f}, 4.0f, Color{255, 244, 228, 245});
}

bool LaserPortTrap::trailHitsPlayer(const PlayerBall& player) const
{
    constexpr float runY = GroundY - 22.0f;
    const std::array<Vector2, 6> points{{
        Vector2{center.x - 300.0f, runY - 66.0f},
        Vector2{center.x - 150.0f, runY},
        Vector2{center.x, runY - 66.0f},
        Vector2{center.x + 150.0f, runY},
        Vector2{center.x + 300.0f, runY - 66.0f},
        Vector2{center.x + 450.0f, runY},
    }};

    std::array<float, 5> segmentLengths{};
    float total = 0.0f;
    for (size_t i = 0; i + 1 < points.size(); ++i) {
        segmentLengths[i] = length(Vector2{points[i + 1].x - points[i].x, points[i + 1].y - points[i].y});
        total += segmentLengths[i];
    }

    const float headDistance = easeInOut(activeProgress()) * total;
    const float tailDistance = clampFloat(headDistance - 118.0f, 0.0f, total);
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

PushDoorTrap::PushDoorTrap(Vector2 trapCenter)
    : Trap("Push Door", trapCenter, 0.55f, 0.62f, 0.18f)
{
}

float PushDoorTrap::pushAmount() const
{
    if (state == TrapState::Windup) {
        return std::sin(GetTime() * 34.0) * 3.0f * getStateProgress();
    }
    if (state == TrapState::Active) {
        return easeInOut(activeProgress()) * 106.0f;
    }
    if (state == TrapState::Cooldown) {
        return 106.0f * (1.0f - easeInOut(getStateProgress()));
    }
    return 0.0f;
}

Rectangle PushDoorTrap::leftDoor() const
{
    const float push = pushAmount();
    return Rectangle{center.x - 53.0f - push, GroundY - 122.0f, 48.0f, 122.0f};
}

Rectangle PushDoorTrap::rightDoor() const
{
    const float push = pushAmount();
    return Rectangle{center.x + 5.0f + push, GroundY - 122.0f, 48.0f, 122.0f};
}

void PushDoorTrap::draw(float cameraX, bool highlighted) const
{
    drawDangerSign(center.x, GroundY - 190.0f, cameraX, "PUSH DOOR", Color{255, 138, 50, 255});

    const float x = center.x - cameraX;
    const Color color = stateColor(highlighted);
    const Rectangle left = leftDoor();
    const Rectangle right = rightDoor();
    DrawEllipse(static_cast<int>(x), static_cast<int>(GroundY + 4.0f), 146.0f, 16.0f, Color{0, 0, 0, 62});
    DrawRectangle(static_cast<int>(x - 122.0f), static_cast<int>(GroundY - 142.0f), 28, 142, Color{42, 43, 50, 255});
    DrawRectangle(static_cast<int>(x + 94.0f), static_cast<int>(GroundY - 142.0f), 28, 142, Color{31, 32, 38, 255});
    DrawRectangle(static_cast<int>(x - 138.0f), static_cast<int>(GroundY - 164.0f), 276, 22, Color{58, 59, 66, 255});
    DrawRectangle(static_cast<int>(x - 72.0f), static_cast<int>(GroundY - 150.0f), 144, 10, Color{255, 222, 105, 120});

    if (state == TrapState::Windup) {
        const unsigned char alpha = static_cast<unsigned char>(70.0f + 135.0f * getStateProgress());
        DrawLineEx(Vector2{x - 63.0f, GroundY - 74.0f}, Vector2{x + 63.0f, GroundY - 74.0f}, 5.0f, Color{255, 192, 62, alpha});
        DrawTriangle(Vector2{x - 74.0f, GroundY - 74.0f}, Vector2{x - 52.0f, GroundY - 86.0f}, Vector2{x - 52.0f, GroundY - 62.0f}, Color{255, 192, 62, alpha});
        DrawTriangle(Vector2{x + 74.0f, GroundY - 74.0f}, Vector2{x + 52.0f, GroundY - 86.0f}, Vector2{x + 52.0f, GroundY - 62.0f}, Color{255, 192, 62, alpha});
        DrawText("OUTWARD", static_cast<int>(x - 35.0f), static_cast<int>(GroundY - 210.0f), 13, Color{255, 210, 92, alpha});
    }

    DrawRectangleRec(Rectangle{left.x - cameraX + ShadowOffset.x, left.y + ShadowOffset.y, left.width, left.height}, Color{0, 0, 0, 78});
    DrawRectangleRec(Rectangle{right.x - cameraX + ShadowOffset.x, right.y + ShadowOffset.y, right.width, right.height}, Color{0, 0, 0, 78});
    DrawRectangleRec(Rectangle{left.x - cameraX, left.y, left.width, left.height}, state == TrapState::Active ? RED : color);
    DrawRectangleRec(Rectangle{right.x - cameraX, right.y, right.width, right.height}, state == TrapState::Active ? RED : color);
    DrawRectangle(static_cast<int>(left.x - cameraX + 7.0f), static_cast<int>(left.y + 8.0f), 8, static_cast<int>(left.height - 16.0f), Color{255, 255, 255, 70});
    DrawRectangle(static_cast<int>(right.x - cameraX + 7.0f), static_cast<int>(right.y + 8.0f), 8, static_cast<int>(right.height - 16.0f), Color{255, 255, 255, 70});

    if (state == TrapState::Spent) {
        DrawText("spent", static_cast<int>(x - 24.0f), static_cast<int>(GroundY - 184.0f), 16, Color{145, 145, 145, 255});
    }
}

bool PushDoorTrap::hitsPlayer(const PlayerBall& player) const
{
    if (state != TrapState::Active) {
        return false;
    }
    const Vector2 position = player.getPosition();
    const float radius = player.getRadius();
    return circleHitsRect(position, radius, leftDoor()) || circleHitsRect(position, radius, rightDoor());
}

TeeterRockTrap::TeeterRockTrap(Vector2 trapCenter)
    : Trap("Teeter Rock", trapCenter, 0.10f, 0.78f, 0.20f)
{
}

float TeeterRockTrap::rockDrop() const
{
    if (state == TrapState::Windup) {
        return 0.0f;
    }
    if (state == TrapState::Active) {
        return easeInOut(clampFloat(activeProgress() / 0.24f, 0.0f, 1.0f));
    }
    if (state == TrapState::Cooldown || state == TrapState::Spent) {
        return 1.0f;
    }
    return 0.0f;
}

float TeeterRockTrap::boardTilt() const
{
    const float p = state == TrapState::Active || state == TrapState::Cooldown || state == TrapState::Spent ? rockDrop() : 0.0f;
    return lerpFloat(-8.0f, 15.0f, p);
}

float TeeterRockTrap::boardSurfaceY(float worldX) const
{
    constexpr float boardCenterY = GroundY - 34.0f;
    const float radians = boardTilt() * Pi / 180.0f;
    return boardCenterY + std::tan(radians) * (worldX - center.x);
}

Rectangle TeeterRockTrap::boardArea() const
{
    return Rectangle{center.x - 215.0f, GroundY - 94.0f, 430.0f, 112.0f};
}

Rectangle TeeterRockTrap::rockRect() const
{
    const float drop = rockDrop();
    constexpr float rockWidth = 112.0f;
    constexpr float rockHeight = 84.0f;
    constexpr float rockCenterOffset = 120.0f;
    const float finalBottom = boardSurfaceY(center.x + rockCenterOffset) + 3.0f;
    const float y = lerpFloat(GroundY - 366.0f, finalBottom - rockHeight, drop);
    return Rectangle{center.x + rockCenterOffset - rockWidth * 0.5f, y, rockWidth, rockHeight};
}

bool TeeterRockTrap::playerOnLaunchSide(const PlayerBall& player) const
{
    const Vector2 position = player.getPosition();
    const Rectangle board = boardArea();
    const float surfaceY = boardSurfaceY(position.x);
    return position.x >= board.x + 18.0f && position.x <= center.x - 24.0f && position.y + player.getRadius() >= surfaceY - 24.0f;
}

void TeeterRockTrap::onTriggered(PlayerBall& player)
{
    launchQueued = playerOnLaunchSide(player);
}

void TeeterRockTrap::constrainPlayer(PlayerBall& player) const
{
    if (player.locksCamera()) {
        return;
    }

    const Vector2 position = player.getPosition();
    const Rectangle board = boardArea();
    if (position.x >= board.x && position.x <= board.x + board.width) {
        const float surfaceY = boardSurfaceY(position.x);
        if (position.y + player.getRadius() >= surfaceY - 30.0f) {
            player.standOnSurface(surfaceY);
        }
    }

    if (launchQueued && !launchedPlayer && state == TrapState::Active && rockDrop() >= 0.86f) {
        player.launch(Vector2{920.0f, -560.0f});
        launchedPlayer = true;
    }
}

void TeeterRockTrap::draw(float cameraX, bool highlighted) const
{
    (void)highlighted;
    drawRoadWarningSign(center.x - 302.0f, GroundY, cameraX);
    drawDangerSign(center.x, GroundY - 250.0f, cameraX, "TEETER ROCK", Color{255, 199, 64, 255});

    const float x = center.x - cameraX;
    const Rectangle rock = rockRect();
    constexpr float boardWidth = 430.0f;
    constexpr float boardThickness = 24.0f;
    constexpr float boardCenterY = GroundY - 34.0f;
    DrawEllipse(static_cast<int>(x), static_cast<int>(GroundY + 6.0f), 190.0f, 18.0f, Color{0, 0, 0, 68});
    DrawRectangle(static_cast<int>(x - 18.0f), static_cast<int>(GroundY - 42.0f), 36, 42, Color{39, 39, 45, 255});
    DrawTriangle(Vector2{x - 42.0f, GroundY}, Vector2{x + 42.0f, GroundY}, Vector2{x, GroundY - 60.0f}, Color{52, 53, 60, 255});

    DrawRectanglePro(Rectangle{x + ShadowOffset.x * 0.45f, boardCenterY + 28.0f, boardWidth, 22.0f}, Vector2{boardWidth * 0.5f, 11.0f}, boardTilt(), Color{0, 0, 0, 76});
    DrawRectanglePro(Rectangle{x, boardCenterY, boardWidth, boardThickness}, Vector2{boardWidth * 0.5f, boardThickness * 0.5f}, boardTilt(), Color{122, 90, 58, 255});
    DrawRectanglePro(Rectangle{x, boardCenterY - 5.0f, boardWidth - 24.0f, 6.0f}, Vector2{(boardWidth - 24.0f) * 0.5f, 3.0f}, boardTilt(), Color{198, 150, 92, 150});

    const Vector2 ropeTop{rock.x - cameraX + rock.width * 0.5f, GroundY - 382.0f};
    const Vector2 ropeBottom{rock.x - cameraX + rock.width * 0.5f, rock.y};
    DrawRectangle(static_cast<int>(ropeTop.x - 72.0f), static_cast<int>(GroundY - 398.0f), 146, 18, Color{56, 57, 64, 255});
    DrawRectangle(static_cast<int>(ropeTop.x - 9.0f), static_cast<int>(GroundY - 380.0f), 18, 42, Color{37, 38, 44, 255});
    if (state == TrapState::Idle || state == TrapState::Windup) {
        DrawLineEx(ropeTop, ropeBottom, 5.0f, Color{42, 31, 22, 255});
        DrawLineEx(Vector2{ropeTop.x + 7.0f, ropeTop.y}, Vector2{ropeBottom.x + 7.0f, ropeBottom.y}, 3.0f, Color{98, 70, 44, 255});
    } else {
        const Vector2 breakPoint{ropeTop.x, GroundY - 300.0f};
        DrawLineEx(ropeTop, Vector2{breakPoint.x - 12.0f, breakPoint.y - 12.0f}, 5.0f, Color{42, 31, 22, 255});
        DrawLineEx(Vector2{ropeBottom.x + 6.0f, ropeBottom.y}, Vector2{breakPoint.x + 18.0f, breakPoint.y + 26.0f}, 4.0f, Color{98, 70, 44, 255});
        DrawCircleV(breakPoint, 5.0f, Color{255, 214, 94, 220});
    }

    DrawRectangleRec(Rectangle{rock.x - cameraX + ShadowOffset.x * 0.55f, rock.y + ShadowOffset.y * 0.45f, rock.width, rock.height}, Color{0, 0, 0, 76});
    DrawRectangleRec(Rectangle{rock.x - cameraX, rock.y, rock.width, rock.height}, state == TrapState::Active ? Color{91, 83, 74, 255} : Color{112, 112, 118, 255});
    DrawRectangle(static_cast<int>(rock.x - cameraX + 10.0f), static_cast<int>(rock.y + 8.0f), static_cast<int>(rock.width - 20.0f), 10, Color{255, 255, 255, 45});
    DrawRectangle(static_cast<int>(rock.x - cameraX + rock.width - 18.0f), static_cast<int>(rock.y + 14.0f), 8, static_cast<int>(rock.height - 26.0f), Color{50, 50, 55, 110});
    DrawRectangleLinesEx(Rectangle{rock.x - cameraX, rock.y, rock.width, rock.height}, 2.0f, BLACK);

    if (state == TrapState::Windup) {
        const unsigned char alpha = static_cast<unsigned char>(85.0f + 135.0f * getStateProgress());
        DrawText("CUT", static_cast<int>(ropeTop.x - 15.0f), static_cast<int>(GroundY - 428.0f), 14, Color{255, 218, 94, alpha});
        DrawCircleV(Vector2{ropeTop.x, GroundY - 338.0f}, 12.0f, Color{255, 70, 55, alpha});
    }

    if (launchedPlayer) {
        DrawText("LAUNCH", static_cast<int>(x - 178.0f), static_cast<int>(GroundY - 82.0f), 14, Color{140, 255, 160, 230});
    } else if (state == TrapState::Spent) {
        DrawText("spent", static_cast<int>(x - 24.0f), static_cast<int>(GroundY - 226.0f), 16, Color{145, 145, 145, 255});
    }
}

bool TeeterRockTrap::hitsPlayer(const PlayerBall& player) const
{
    if (launchedPlayer || state != TrapState::Active || rockDrop() < 0.94f) {
        return false;
    }
    return player.getPosition().x > center.x + 18.0f && circleHitsRect(player.getPosition(), player.getRadius(), rockRect());
}
