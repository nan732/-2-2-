#pragma once

#include "Level.h"
#include "Types.h"

class PlayerBall;

class UI {
public:
    void drawMenu(int screenWidth, int screenHeight) const;
    void drawHud(const PlayerBall& player, const Level& level, float elapsedTime) const;
    void drawPause(int screenWidth, int screenHeight) const;
    void drawVictory(int screenWidth, int screenHeight, float elapsedTime) const;
    void drawDefeat(int screenWidth, int screenHeight) const;

private:
    void drawCenteredText(const char* text, int y, int fontSize, Color color, int screenWidth) const;
};
