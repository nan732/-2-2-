#include "Game.h"

#include "MathUtil.h"

#include <cmath>

namespace {
constexpr int screenWidth = 1280;
constexpr int screenHeight = 720;
constexpr float worldZoom = 2.15f;
constexpr const char* musicCandidates[] = {
    "177840180060595.mp3",
    "../177840180060595.mp3",
    "../../177840180060595.mp3",
};
}

Game::Game()
{
    InitWindow(screenWidth, screenHeight, "Trap Mind Ball - Low Poly Duel");
    InitAudioDevice();
    loadMusic();
    SetTargetFPS(60);
}

Game::~Game()
{
    if (musicReady) {
        StopMusicStream(music);
        UnloadMusicStream(music);
    }
    CloseAudioDevice();
    CloseWindow();
}

void Game::run()
{
    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        if (musicReady) {
            UpdateMusicStream(music);
        }
        update(dt);
        draw();
    }
}

void Game::resetRun()
{
    level.reset();
    player.reset(level.getGroundY());
    shards.clear();
    elapsedTime = 0.0f;
    cameraX = 0.0f;
    state = GameState::Playing;
    updateCamera();
}

void Game::update(float dt)
{
    if (IsKeyPressed(KEY_R)) {
        resetRun();
        return;
    }

    if (state == GameState::Menu) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            resetRun();
        }
        return;
    }

    if (state == GameState::Victory || state == GameState::Defeat) {
        updateShards(dt);
        updateCamera();
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        state = state == GameState::Paused ? GameState::Playing : GameState::Paused;
    }

    if (state == GameState::Paused) {
        updateCamera();
        return;
    }

    elapsedTime += dt;
    player.update(dt, level.getGroundY());
    level.constrainPlayer(player);

    if (IsKeyPressed(KEY_P)) {
        level.triggerNearestTrap(player);
    }

    level.update(dt);
    level.constrainPlayer(player);

    if (level.hitsPlayer(player)) {
        spawnShards(player.getPosition());
        state = GameState::Defeat;
    } else if (level.reachedFinish(player)) {
        state = GameState::Victory;
    }

    updateCamera();
}

void Game::draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (state == GameState::Menu) {
        ui.drawMenu(screenWidth, screenHeight);
        EndDrawing();
        return;
    }

    Camera2D worldCamera{};
    worldCamera.offset = Vector2{0.0f, static_cast<float>(screenHeight)};
    worldCamera.target = Vector2{0.0f, static_cast<float>(screenHeight)};
    worldCamera.rotation = 0.0f;
    worldCamera.zoom = worldZoom;

    BeginMode2D(worldCamera);
    level.draw(player, cameraX, screenWidth, screenHeight);
    if (state != GameState::Defeat) {
        player.draw(cameraX);
    }
    level.drawForeground(cameraX);
    drawShards();
    EndMode2D();

    ui.drawHud(player, level, elapsedTime);

    if (state == GameState::Paused) {
        ui.drawPause(screenWidth, screenHeight);
    } else if (state == GameState::Victory) {
        ui.drawVictory(screenWidth, screenHeight, elapsedTime);
    } else if (state == GameState::Defeat) {
        ui.drawDefeat(screenWidth, screenHeight);
    }

    EndDrawing();
}

void Game::updateCamera()
{
    if (player.locksCamera()) {
        return;
    }

    const float visibleWidth = static_cast<float>(screenWidth) / worldZoom;
    const float desired = player.getPosition().x - 235.0f;
    const float maxCamera = level.getTrackMaxX() - visibleWidth + 80.0f;
    cameraX = lerpFloat(cameraX, clampFloat(desired, 0.0f, maxCamera), 0.22f);
}

void Game::loadMusic()
{
    for (const char* path : musicCandidates) {
        if (FileExists(path)) {
            music = LoadMusicStream(path);
            if (music.stream.buffer != nullptr) {
                music.looping = true;
                SetMusicVolume(music, 0.42f);
                PlayMusicStream(music);
                musicReady = true;
            }
            return;
        }
    }
}

void Game::spawnShards(Vector2 origin)
{
    shards.clear();
    for (int i = 0; i < 30; ++i) {
        const float angle = static_cast<float>(i) * 0.72f;
        const float speed = 160.0f + static_cast<float>(i % 7) * 22.0f;
        ShardParticle shard;
        shard.position = origin;
        shard.velocity = Vector2{std::cos(angle) * speed, std::sin(angle) * speed - 240.0f};
        shard.color = i % 2 == 0 ? Color{0, 116, 255, 255} : Color{115, 190, 255, 255};
        shard.life = 1.0f + static_cast<float>(i % 4) * 0.12f;
        shard.maxLife = shard.life;
        shards.push_back(shard);
    }
}

void Game::updateShards(float dt)
{
    for (ShardParticle& shard : shards) {
        if (shard.life <= 0.0f) {
            continue;
        }
        shard.life -= dt;
        shard.velocity.y += 1200.0f * dt;
        shard.position.x += shard.velocity.x * dt;
        shard.position.y += shard.velocity.y * dt;
        if (shard.position.y > level.getGroundY() - 4.0f) {
            shard.position.y = level.getGroundY() - 4.0f;
            shard.velocity.y *= -0.22f;
            shard.velocity.x *= 0.68f;
        }
    }
}

void Game::drawShards() const
{
    for (const ShardParticle& shard : shards) {
        if (shard.life <= 0.0f) {
            continue;
        }
        Color color = shard.color;
        color.a = static_cast<unsigned char>(255.0f * clampFloat(shard.life / shard.maxLife, 0.0f, 1.0f));
        DrawRectangle(static_cast<int>(shard.position.x - cameraX), static_cast<int>(shard.position.y), 8, 8, color);
    }
}
