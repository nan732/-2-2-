#include "Level.h"

#include "MathUtil.h"
#include "PlayerBall.h"

#include <cmath>
#include <limits>

namespace {
void drawArenaColumn(float worldX, float groundY, float cameraX, float height, Color face, Color side)
{
    const float x = worldX - cameraX;
    const float baseY = groundY;
    DrawEllipse(static_cast<int>(x + 18.0f), static_cast<int>(baseY + 12.0f), 32.0f, 9.0f, Color{0, 0, 0, 55});
    DrawRectangle(static_cast<int>(x - 15.0f), static_cast<int>(baseY - height), 30, static_cast<int>(height), face);
    DrawRectangle(static_cast<int>(x + 15.0f), static_cast<int>(baseY - height + 8.0f), 14, static_cast<int>(height - 8.0f), side);
    DrawRectangle(static_cast<int>(x - 20.0f), static_cast<int>(baseY - height - 10.0f), 55, 12, Color{68, 70, 76, 255});
    DrawRectangle(static_cast<int>(x - 25.0f), static_cast<int>(baseY - 12.0f), 62, 14, Color{44, 45, 51, 255});
    DrawLine(static_cast<int>(x - 13.0f), static_cast<int>(baseY - height + 18.0f), static_cast<int>(x - 13.0f), static_cast<int>(baseY - 18.0f), Color{255, 255, 255, 36});
}

void drawArenaTile(float worldX, float groundY, float cameraX, Color color)
{
    const float x = worldX - cameraX;
    DrawPoly(Vector2{x + 55.0f, groundY + 26.0f}, 6, 48.0f, 0.0f, color);
    DrawPolyLines(Vector2{x + 55.0f, groundY + 26.0f}, 6, 48.0f, 0.0f, Color{0, 0, 0, 32});
}

}

Level::Level()
{
    reset();
}

void Level::reset()
{
    traps.clear();
    traps.emplace_back(std::make_unique<HammerTrap>(Vector2{700.0f, 345.0f}));
    traps.emplace_back(std::make_unique<FurnaceTrap>(Vector2{1080.0f, 0.0f}));
    traps.emplace_back(std::make_unique<LaserPortTrap>(Vector2{1750.0f, groundY}));
    traps.emplace_back(std::make_unique<DepthRamTrap>(Vector2{2600.0f, groundY}));
    traps.emplace_back(std::make_unique<DropTileTrap>(Vector2{3460.0f, groundY}));
}

void Level::update(float dt)
{
    for (const auto& trap : traps) {
        trap->update(dt);
    }
}

void Level::draw(const PlayerBall& player, float cameraX, int screenWidth, int screenHeight) const
{
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{229, 235, 239, 255}, Color{155, 170, 184, 255});
    DrawCircleGradient(92, 82, 118.0f, Color{255, 239, 168, 190}, Color{255, 239, 168, 0});
    DrawCircle(92, 82, 19.0f, Color{255, 230, 116, 245});

    DrawRectangle(0, 350, screenWidth, 136, Color{208, 214, 218, 160});
    DrawLineEx(Vector2{0.0f, 386.0f}, Vector2{static_cast<float>(screenWidth), 350.0f}, 3.0f, Color{255, 255, 255, 78});

    const float parallax = cameraX * 0.32f;
    for (float x = -180.0f; x < static_cast<float>(screenWidth) + 240.0f; x += 150.0f) {
        const float px = x - std::fmod(parallax, 150.0f);
        DrawRectangle(static_cast<int>(px), 226, 28, 176, Color{242, 244, 242, 205});
        DrawRectangle(static_cast<int>(px + 28.0f), 236, 12, 166, Color{198, 205, 204, 185});
        DrawRectangle(static_cast<int>(px - 10.0f), 213, 58, 16, Color{252, 253, 250, 210});
        DrawEllipse(static_cast<int>(px + 21.0f), 408, 34.0f, 8.0f, Color{0, 0, 0, 32});
    }

    DrawRectangle(0, static_cast<int>(groundY + 20.0f), screenWidth, 72, Color{0, 0, 0, 58});
    DrawRectangleGradientV(0, static_cast<int>(groundY - 4.0f), screenWidth, 96, Color{67, 63, 58, 255}, Color{23, 23, 27, 255});
    DrawTriangle(Vector2{0.0f, groundY - 96.0f}, Vector2{static_cast<float>(screenWidth), groundY - 134.0f}, Vector2{static_cast<float>(screenWidth), groundY + 15.0f}, Color{112, 104, 91, 255});
    DrawTriangle(Vector2{0.0f, groundY - 96.0f}, Vector2{static_cast<float>(screenWidth), groundY + 15.0f}, Vector2{0.0f, groundY + 32.0f}, Color{87, 81, 76, 255});

    for (float x = trackMinX - 80.0f; x < trackMaxX + 120.0f; x += 120.0f) {
        const int tone = static_cast<int>(std::fmod(std::fabs(x), 3.0f));
        const Color tile = tone == 0 ? Color{128, 119, 101, 120} : tone == 1 ? Color{95, 91, 86, 105} : Color{146, 132, 104, 95};
        drawArenaTile(x, groundY - 52.0f, cameraX, tile);
    }

    DrawRectangle(static_cast<int>(trackMinX - cameraX), static_cast<int>(groundY - 9.0f), static_cast<int>(trackMaxX - trackMinX), 8, Color{0, 0, 0, 90});
    DrawRectangle(static_cast<int>(trackMinX - cameraX), static_cast<int>(groundY - 15.0f), static_cast<int>(trackMaxX - trackMinX), 6, Color{25, 24, 26, 255});
    DrawRectangle(static_cast<int>(trackMinX - cameraX), static_cast<int>(groundY - 19.0f), static_cast<int>(trackMaxX - trackMinX), 2, Color{255, 232, 144, 120});

    for (float x = 230.0f; x < trackMaxX; x += 470.0f) {
        drawArenaColumn(x, groundY - 18.0f, cameraX, 178.0f + std::fmod(x, 90.0f), Color{236, 238, 235, 255}, Color{176, 184, 184, 255});
    }

    DrawRectangle(static_cast<int>(trackMinX - cameraX), static_cast<int>(groundY - 110.0f), 11, 110, Color{31, 31, 35, 255});
    DrawRectangle(static_cast<int>(trackMaxX - cameraX), static_cast<int>(groundY - 155.0f), 11, 155, Color{31, 31, 35, 255});
    DrawRectangle(static_cast<int>(trackMaxX - cameraX - 82.0f), static_cast<int>(groundY - 188.0f), 92, 36, Color{34, 35, 39, 240});
    DrawRectangleLines(static_cast<int>(trackMaxX - cameraX - 82.0f), static_cast<int>(groundY - 188.0f), 92, 36, Color{255, 220, 95, 255});
    DrawText("FINISH", static_cast<int>(trackMaxX - cameraX - 70.0f), static_cast<int>(groundY - 179.0f), 18, RAYWHITE);

    const int nearest = nearestTrapIndex(player);
    for (int i = 0; i < static_cast<int>(traps.size()); ++i) {
        traps[i]->draw(cameraX, i == nearest);
    }
}

void Level::drawForeground(float cameraX) const
{
    for (const auto& trap : traps) {
        trap->drawForeground(cameraX);
    }
}

void Level::constrainPlayer(PlayerBall& player) const
{
    player.clampToTrack(trackMinX, trackMaxX);
    for (const auto& trap : traps) {
        trap->constrainPlayer(player);
    }
    player.clampToTrack(trackMinX, trackMaxX);
}

bool Level::triggerNearestTrap(const PlayerBall& player)
{
    for (const auto& trap : traps) {
        if (trap->isBusy()) {
            return false;
        }
    }

    const int index = nearestTrapIndex(player);
    if (index < 0) {
        return false;
    }
    return traps[static_cast<size_t>(index)]->trigger();
}

bool Level::hitsPlayer(const PlayerBall& player) const
{
    for (const auto& trap : traps) {
        if (trap->hitsPlayer(player)) {
            return true;
        }
    }
    return false;
}

bool Level::reachedFinish(const PlayerBall& player) const
{
    return player.getPosition().x >= trackMaxX - 36.0f;
}

int Level::nearestTrapIndex(const PlayerBall& player) const
{
    float bestDistance = std::numeric_limits<float>::max();
    int bestIndex = -1;
    for (int i = 0; i < static_cast<int>(traps.size()); ++i) {
        if (!traps[static_cast<size_t>(i)]->isAvailable()) {
            continue;
        }
        const float distance = distanceX(player.getPosition(), traps[static_cast<size_t>(i)]->getCenter());
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = i;
        }
    }
    return bestIndex;
}

const Trap* Level::nearestTrap(const PlayerBall& player) const
{
    const int index = nearestTrapIndex(player);
    if (index < 0) {
        return nullptr;
    }
    return traps[static_cast<size_t>(index)].get();
}

float Level::getTrackMinX() const
{
    return trackMinX;
}

float Level::getTrackMaxX() const
{
    return trackMaxX;
}

float Level::getGroundY() const
{
    return groundY;
}
