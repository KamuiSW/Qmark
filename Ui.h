#pragma once

#include "Types.h"

void DrawTextFont(Font font, const std::string& text, Vector2 pos, float fontSize, Color color);
void DrawCenteredTextFont(Font font, const std::string& text, float y, float fontSize, Color color);
bool DrawButton(Font font, Button button);
bool DrawToggleButton(Font font, Rectangle rect, const char* label, bool value);
bool DrawSlider(Font font, Rectangle rect, const char* label, float& value, float minValue, float maxValue);
