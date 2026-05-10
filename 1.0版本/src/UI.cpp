#include "UI.h"

#include "PlayerBall.h"

#include <cstdio>

void UI::drawCenteredText(const char* text, int y, int fontSize, Color color, int screenWidth) const
{
    const int width = MeasureText(text, fontSize);
    DrawText(text, (screenWidth - width) / 2, y, fontSize, color);
}

void UI::drawMenu(int screenWidth, int screenHeight) const
{
    ClearBackground(RAYWHITE);
    DrawRectangle(0, screenHeight - 110, screenWidth, 110, BLACK);
    drawCenteredText("TRAP MIND BALL", 118, 54, BLACK, screenWidth);
    drawCenteredText("2D duel prototype", 190, 28, BLACK, screenWidth);
    drawCenteredText("Player 1: A/D or arrow keys move, SPACE jumps", 265, 24, BLACK, screenWidth);
    drawCenteredText("Player 2: P triggers the nearest unused trap once", 302, 24, BLACK, screenWidth);
    drawCenteredText("Hammer kills on contact. Furnace only kills if the ball is sealed inside.", 356, 22, BLACK, screenWidth);
    drawCenteredText("Press ENTER or SPACE to start", 440, 28, BLACK, screenWidth);
}

void UI::drawHud(const PlayerBall& player, const Level& level, float elapsedTime) const
{
    char buffer[160];
    std::snprintf(buffer, sizeof(buffer), "Time %.1fs   Speed %.0f", elapsedTime, player.getSpeed());
    DrawText(buffer, 28, 24, 22, BLACK);

    const Trap* nearest = level.nearestTrap(player);
    if (nearest != nullptr) {
        std::snprintf(buffer, sizeof(buffer), "Nearest unused trap: %s", nearest->getName().c_str());
    } else {
        std::snprintf(buffer, sizeof(buffer), "Nearest unused trap: none");
    }
    DrawText(buffer, 28, 54, 22, BLACK);
    DrawText("P1 A/D SPACE   P2 P   R restart   ESC pause", 28, 86, 20, BLACK);

    const float progress = player.getPosition().x / level.getTrackMaxX();
    DrawRectangle(760, 34, 420, 14, Color{220, 220, 220, 255});
    DrawRectangle(760, 34, static_cast<int>(420.0f * progress), 14, BLACK);
    DrawRectangleLines(760, 34, 420, 14, BLACK);
}

void UI::drawPause(int screenWidth, int screenHeight) const
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{255, 255, 255, 210});
    drawCenteredText("PAUSED", screenHeight / 2 - 52, 48, BLACK, screenWidth);
    drawCenteredText("Press ESC to continue, R to restart", screenHeight / 2 + 10, 24, BLACK, screenWidth);
}

void UI::drawVictory(int screenWidth, int screenHeight, float elapsedTime) const
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{255, 255, 255, 230});
    drawCenteredText("VICTORY", screenHeight / 2 - 82, 58, BLACK, screenWidth);
    char buffer[96];
    std::snprintf(buffer, sizeof(buffer), "Final time: %.2fs", elapsedTime);
    drawCenteredText(buffer, screenHeight / 2 - 8, 28, BLACK, screenWidth);
    drawCenteredText("Press R to play again", screenHeight / 2 + 50, 24, BLACK, screenWidth);
}

void UI::drawDefeat(int screenWidth, int screenHeight) const
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{255, 255, 255, 220});
    drawCenteredText("FAILED", screenHeight / 2 - 82, 58, BLACK, screenWidth);
    drawCenteredText("The trap player found the timing.", screenHeight / 2 - 8, 26, BLACK, screenWidth);
    drawCenteredText("Press R to restart", screenHeight / 2 + 50, 24, BLACK, screenWidth);
}
