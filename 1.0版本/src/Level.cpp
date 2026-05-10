#include "Level.h"

#include "MathUtil.h"
#include "PlayerBall.h"

#include <limits>

Level::Level()
{
    reset();
}

void Level::reset()
{
    traps.clear();
    traps.emplace_back(std::make_unique<HammerTrap>(Vector2{720.0f, 420.0f}));
    traps.emplace_back(std::make_unique<FurnaceTrap>(Vector2{1390.0f, 0.0f}));
}

void Level::update(float dt)
{
    for (const auto& trap : traps) {
        trap->update(dt);
    }
}

void Level::draw(const PlayerBall& player, float cameraX, int screenWidth, int screenHeight) const
{
    (void)screenHeight;
    DrawRectangle(0, static_cast<int>(groundY), screenWidth, 80, BLACK);
    DrawRectangle(static_cast<int>(trackMinX - cameraX), static_cast<int>(groundY - 5.0f), static_cast<int>(trackMaxX - trackMinX), 5, BLACK);

    DrawRectangle(static_cast<int>(trackMinX - cameraX), static_cast<int>(groundY - 95.0f), 8, 95, BLACK);
    DrawRectangle(static_cast<int>(trackMaxX - cameraX), static_cast<int>(groundY - 145.0f), 8, 145, BLACK);
    DrawText("FINISH", static_cast<int>(trackMaxX - cameraX - 78.0f), static_cast<int>(groundY - 180.0f), 24, BLACK);

    const int nearest = nearestTrapIndex(player);
    for (int i = 0; i < static_cast<int>(traps.size()); ++i) {
        traps[i]->draw(cameraX, i == nearest);
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
