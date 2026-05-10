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
    ClearBackground(Color{226, 232, 236, 255});
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, Color{232, 238, 242, 255}, Color{124, 137, 151, 255});
    DrawTriangle(Vector2{0.0f, static_cast<float>(screenHeight - 190)}, Vector2{static_cast<float>(screenWidth), static_cast<float>(screenHeight - 260)}, Vector2{static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, Color{86, 80, 73, 255});
    DrawTriangle(Vector2{0.0f, static_cast<float>(screenHeight - 190)}, Vector2{static_cast<float>(screenWidth), static_cast<float>(screenHeight)}, Vector2{0.0f, static_cast<float>(screenHeight)}, Color{55, 54, 58, 255});
    DrawCircleGradient(132, 92, 140.0f, Color{255, 232, 134, 160}, Color{255, 232, 134, 0});
    DrawRectangle(0, screenHeight - 86, screenWidth, 86, Color{24, 24, 28, 245});

    drawCenteredText("TRAP MIND BALL", 118, 56, Color{24, 24, 28, 255}, screenWidth);
    drawCenteredText("read the feint, commit the trap", 188, 24, Color{54, 57, 64, 255}, screenWidth);
    drawCenteredText("P1  A/D or arrows      P2  P", 284, 28, Color{24, 24, 28, 255}, screenWidth);
    drawCenteredText("No jumping. Every trap has a tell, every press is a commitment.", 332, 22, Color{42, 43, 48, 255}, screenWidth);
    drawCenteredText("Press ENTER or SPACE", 438, 28, Color{24, 24, 28, 255}, screenWidth);
}

void UI::drawHud(const PlayerBall& player, const Level& level, float elapsedTime) const
{
    char buffer[160];
    DrawRectangle(18, 18, 560, 88, Color{22, 22, 26, 205});
    DrawRectangle(18, 18, 6, 88, Color{255, 204, 74, 255});
    DrawRectangleLines(18, 18, 560, 88, Color{0, 0, 0, 160});

    std::snprintf(buffer, sizeof(buffer), "TIME %.1f    SPEED %.0f", elapsedTime, player.getSpeed());
    DrawText(buffer, 36, 30, 20, RAYWHITE);

    const Trap* nearest = level.nearestTrap(player);
    if (nearest != nullptr) {
        std::snprintf(buffer, sizeof(buffer), "NEXT TRAP  %s", nearest->getName().c_str());
    } else {
        std::snprintf(buffer, sizeof(buffer), "NEXT TRAP  NONE");
    }
    DrawText(buffer, 36, 58, 20, Color{255, 222, 105, 255});
    DrawText("A/D MOVE    P TRAP    R RESET    ESC PAUSE", 36, 84, 14, Color{205, 214, 222, 255});

    const float progress = player.getPosition().x / level.getTrackMaxX();
    DrawRectangle(720, 32, 444, 18, Color{26, 26, 30, 220});
    DrawRectangle(724, 36, static_cast<int>(436.0f * progress), 10, Color{255, 204, 74, 255});
    DrawRectangleLines(720, 32, 444, 18, Color{0, 0, 0, 180});
    DrawText("RUN", 672, 29, 20, Color{25, 25, 28, 255});
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
