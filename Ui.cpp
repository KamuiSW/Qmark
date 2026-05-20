#include "Ui.h"
#include "Config.h"
#include "Helpers.h"

#include <algorithm>

void DrawTextFont(Font font, const std::string& text, Vector2 pos, float fontSize, Color color)
{
    DrawTextEx(font, text.c_str(), pos, fontSize, 2.0f, color);
}

void DrawCenteredTextFont(Font font, const std::string& text, float y, float fontSize, Color color)
{
    Vector2 size = MeasureTextEx(font, text.c_str(), fontSize, 2.0f);
    DrawTextEx(font, text.c_str(), { SCREEN_WIDTH / 2.0f - size.x / 2.0f, y }, fontSize, 2.0f, color);
}

bool DrawButton(Font font, Button button)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, button.rect);

    Color fill = hovered ? Color{ 70, 90, 130, 230 } : Color{ 25, 35, 60, 220 };
    Color border = hovered ? SKYBLUE : Color{ 140, 150, 170, 255 };

    DrawRectangleRounded(button.rect, 0.2f, 12, fill);
    DrawRectangleRoundedLines(button.rect, 0.2f, 12, border);

    float fontSize = 30.0f;
    Vector2 textSize = MeasureTextEx(font, button.text.c_str(), fontSize, 2.0f);
    DrawTextEx(font, button.text.c_str(), {
        button.rect.x + button.rect.width / 2.0f - textSize.x / 2.0f,
        button.rect.y + button.rect.height / 2.0f - textSize.y / 2.0f
    }, fontSize, 2.0f, WHITE);

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool DrawToggleButton(Font font, Rectangle rect, const char* label, bool value)
{
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);

    Color fill = hovered ? Color{ 55, 70, 105, 230 } : Color{ 25, 35, 60, 220 };
    Color border = value ? SKYBLUE : GRAY;

    DrawRectangleRounded(rect, 0.2f, 12, fill);
    DrawRectangleRoundedLines(rect, 0.2f, 12, border);

    std::string text = std::string(label) + ": " + (value ? "ON" : "OFF");
    float fontSize = 22.0f;
    Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, 2.0f);

    DrawTextEx(font, text.c_str(), {
        rect.x + rect.width / 2.0f - textSize.x / 2.0f,
        rect.y + rect.height / 2.0f - textSize.y / 2.0f
    }, fontSize, 2.0f, value ? SKYBLUE : LIGHTGRAY);

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool DrawSlider(Font font, Rectangle rect, const char* label, float& value, float minValue, float maxValue)
{
    Vector2 mouse = GetMousePosition();
    Rectangle fatFingerRect = { rect.x - 8, rect.y - 18, rect.width + 16, rect.height + 36 };
    bool touchingIt = CheckCollisionPointRec(mouse, fatFingerRect);
    bool changed = false;

    if (touchingIt && IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        float t = (mouse.x - rect.x) / rect.width;
        t = std::max(0.0f, std::min(1.0f, t));
        value = minValue + (maxValue - minValue) * t;
        changed = true;
    }

    DrawTextEx(font, label, { rect.x, rect.y - 35.0f }, 20.0f, 2.0f, WHITE);

    std::string rightText = FormatScore(value) + "s";
    Vector2 rightSize = MeasureTextEx(font, rightText.c_str(), 20.0f, 2.0f);
    DrawTextEx(font, rightText.c_str(), { rect.x + rect.width - rightSize.x, rect.y - 35.0f }, 20.0f, 2.0f, SKYBLUE);

    DrawRectangleRounded({ rect.x, rect.y + rect.height / 2.0f - 5.0f, rect.width, 10.0f }, 0.6f, 12, Color{ 20, 27, 45, 235 });
    DrawRectangleRoundedLines({ rect.x, rect.y + rect.height / 2.0f - 5.0f, rect.width, 10.0f }, 0.6f, 12, Color{ 120, 145, 180, 210 });

    float normalized = (value - minValue) / (maxValue - minValue);
    float knobX = rect.x + normalized * rect.width;
    DrawCircleV({ knobX, rect.y + rect.height / 2.0f }, touchingIt ? 15.0f : 12.0f, touchingIt ? SKYBLUE : LIGHTGRAY);

    return changed;
}
