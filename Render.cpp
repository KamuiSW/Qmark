#include "Render.h"
#include "Config.h"
#include "Helpers.h"

#include <algorithm>
#include <cmath>

static void DrawTextureCentered(Texture2D texture, Vector2 pos, float size, Color tint)
{
    float largestSide = (float)std::max(texture.width, texture.height);
    float scale = size / largestSide;
    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dest = { pos.x, pos.y, texture.width * scale, texture.height * scale };
    Vector2 origin = { dest.width / 2.0f, dest.height / 2.0f };

    DrawTexturePro(texture, source, dest, origin, 0.0f, tint);
}

static void DrawTextureFullscreen(Texture2D texture)
{
    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dest = { 0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
    DrawTexturePro(texture, source, dest, { 0, 0 }, 0.0f, WHITE);
}

void DrawBackground(const GameAssets& assets)
{
    DrawTextureFullscreen(assets.background);
}

void DrawNPCSprite(const GameAssets& assets, Vector2 pos)
{
    DrawTextureCentered(assets.npc, pos, NPC_DRAW_SIZE, WHITE);
}

void DrawMinionSprite(const GameAssets& assets, Vector2 pos)
{
    DrawTextureCentered(assets.minion, pos, MINION_DRAW_SIZE, WHITE);
}

void DrawMissedMinionSprite(const GameAssets& assets, Vector2 pos, float alpha)
{
    DrawTextureCentered(assets.minion, pos, MINION_DRAW_SIZE, Fade(WHITE, alpha * 0.45f));
}

void DrawQuestionPingSprite(const GameAssets& assets, Vector2 pos, float alpha)
{
    DrawTextureCentered(assets.pingQuestion, pos, PING_QUESTION_DRAW_SIZE, Fade(WHITE, alpha));
}

void DrawPingArea(Vector2 pos, float alpha)
{
    Color c = Color{ 80, 180, 255, (unsigned char)(100 * alpha) };
    DrawCircleLines((int)pos.x, (int)pos.y, PING_RADIUS, c);
}

void DrawPingWheel(Font font, PingWheel wheel)
{
    Vector2 center = wheel.startPos;
    Vector2 current = wheel.currentPos;

    DrawCircleV(center, 82, Color{ 20, 25, 35, 175 });
    DrawCircleLines((int)center.x, (int)center.y, 82, Color{ 100, 160, 210, 230 });
    DrawCircleLines((int)center.x, (int)center.y, 36, Color{ 120, 180, 240, 170 });

    DrawTextEx(font, "?", { center.x - 74, center.y - 29 }, 48, 2.0f, SKYBLUE);
    DrawTextEx(font, "!", { center.x + 55, center.y - 29 }, 44, 2.0f, LIGHTGRAY);
    DrawTextEx(font, "^", { center.x - 12, center.y - 90 }, 40, 2.0f, LIGHTGRAY);
    DrawTextEx(font, "v", { center.x - 10, center.y + 37 }, 40, 2.0f, LIGHTGRAY);

    DrawLineEx(center, current, 4, SKYBLUE);
    DrawCircleV(current, 8, SKYBLUE);
    DrawTextEx(font, "Drag LEFT for ? ping", { center.x - 130, center.y + 104 }, 20, 2.0f, WHITE);
}

void DrawFloatingTexts(Font font, const std::vector<FloatingText>& floatingTexts, float now)
{
    for (const auto& t : floatingTexts)
    {
        float age = now - t.startTime;
        float alpha = 1.0f - age / t.duration;
        if (alpha < 0.0f) alpha = 0.0f;

        Color c = t.color;
        c.a = (unsigned char)(255 * alpha);

        Vector2 pos = t.pos;
        pos.y -= age * 45.0f;
        Vector2 size = MeasureTextEx(font, t.text.c_str(), 24.0f, 2.0f);
        DrawTextEx(font, t.text.c_str(), { pos.x - size.x / 2.0f, pos.y - size.y / 2.0f }, 24.0f, 2.0f, c);
    }
}

void DrawHud(Font font, float timeLeft, float score)
{
    int seconds = (int)std::ceil(timeLeft);
    if (seconds < 0) seconds = 0;

    bool finalFive = seconds <= 5;
    float timerFontSize = 78.0f;
    Color timerColor = WHITE;

    if (finalFive)
    {
        timerColor = RED;
        float fractional = timeLeft - std::floor(timeLeft);
        float pulse = std::sin((1.0f - fractional) * PI);
        timerFontSize += pulse * 28.0f;

        std::string timerText = std::to_string(seconds);
        Vector2 glowSize = MeasureTextEx(font, timerText.c_str(), timerFontSize + 10.0f, 2.0f);
        DrawTextEx(font, timerText.c_str(), { SCREEN_WIDTH / 2.0f - glowSize.x / 2.0f, 28.0f - pulse * 10.0f }, timerFontSize + 10.0f, 2.0f, Color{ 255, 0, 0, 70 });
    }

    std::string timerText = std::to_string(seconds);
    Vector2 timerSize = MeasureTextEx(font, timerText.c_str(), timerFontSize, 2.0f);
    DrawTextEx(font, timerText.c_str(), { SCREEN_WIDTH / 2.0f - timerSize.x / 2.0f, 35.0f }, timerFontSize, 2.0f, timerColor);

    std::string scoreText = "Score " + FormatScore(score);
    Vector2 scoreSize = MeasureTextEx(font, scoreText.c_str(), 28.0f, 2.0f);
    DrawTextEx(font, scoreText.c_str(), { SCREEN_WIDTH / 2.0f - scoreSize.x / 2.0f, 120.0f }, 28.0f, 2.0f, GOLD);
}
