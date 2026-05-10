#pragma once

#include "Types.h"
#include "raylib.h"

#include <string>

class PlayerBall;

class Trap {
public:
    Trap(std::string trapName, Vector2 trapCenter, float windupTime, float activeTime, float cooldownTime);
    virtual ~Trap() = default;

    virtual void update(float dt);
    virtual void draw(float cameraX, bool highlighted) const = 0;
    virtual void drawForeground(float cameraX) const;
    virtual bool hitsPlayer(const PlayerBall& player) const = 0;
    virtual void constrainPlayer(PlayerBall& player) const;
    virtual void onTriggered(PlayerBall& player);

    bool trigger();
    bool isAvailable() const;
    bool isBusy() const;
    bool isSpent() const;
    TrapState getState() const;
    Vector2 getCenter() const;
    const std::string& getName() const;
    float getStateProgress() const;

protected:
    float activeProgress() const;
    Color stateColor(bool highlighted) const;

    std::string name;
    Vector2 center{};
    TrapState state = TrapState::Idle;
    float timer = 0.0f;
    float windupDuration = 0.0f;
    float activeDuration = 0.0f;
    float cooldownDuration = 0.0f;
};

class HammerTrap final : public Trap {
public:
    explicit HammerTrap(Vector2 trapCenter);
    void draw(float cameraX, bool highlighted) const override;
    bool hitsPlayer(const PlayerBall& player) const override;

private:
    float hammerAngle() const;
    Vector2 hammerHead() const;
};

class FurnaceTrap final : public Trap {
public:
    explicit FurnaceTrap(Vector2 trapCenter);
    void draw(float cameraX, bool highlighted) const override;
    bool hitsPlayer(const PlayerBall& player) const override;
    void constrainPlayer(PlayerBall& player) const override;

private:
    float wallDrop() const;
    bool wallsClosed() const;
    Rectangle chamber() const;
    Rectangle leftWall() const;
    Rectangle rightWall() const;
};

class LaserPortTrap final : public Trap {
public:
    explicit LaserPortTrap(Vector2 trapCenter);
    void draw(float cameraX, bool highlighted) const override;
    void drawForeground(float cameraX) const override;
    bool hitsPlayer(const PlayerBall& player) const override;

private:
    Vector2 laserPoint(float progress) const;
    bool trailHitsPlayer(const PlayerBall& player) const;
};

class PushDoorTrap final : public Trap {
public:
    explicit PushDoorTrap(Vector2 trapCenter);
    void draw(float cameraX, bool highlighted) const override;
    bool hitsPlayer(const PlayerBall& player) const override;

private:
    float pushAmount() const;
    Rectangle leftDoor() const;
    Rectangle rightDoor() const;
};

class TeeterRockTrap final : public Trap {
public:
    explicit TeeterRockTrap(Vector2 trapCenter);
    void draw(float cameraX, bool highlighted) const override;
    bool hitsPlayer(const PlayerBall& player) const override;
    void constrainPlayer(PlayerBall& player) const override;
    void onTriggered(PlayerBall& player) override;

private:
    float rockDrop() const;
    float boardTilt() const;
    float boardSurfaceY(float worldX) const;
    Rectangle boardArea() const;
    Rectangle rockRect() const;
    bool playerOnLaunchSide(const PlayerBall& player) const;
    bool launchQueued = false;
    mutable bool launchedPlayer = false;
};
