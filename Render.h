#pragma once

#include "Types.h"

#include <vector>

void DrawBackground(const GameAssets& assets);
void DrawNPCSprite(const GameAssets& assets, Vector2 pos);
void DrawMinionSprite(const GameAssets& assets, Vector2 pos);
void DrawMissedMinionSprite(const GameAssets& assets, Vector2 pos, float alpha);
void DrawQuestionPingSprite(const GameAssets& assets, Vector2 pos, float alpha);
void DrawPingArea(Vector2 pos, float alpha);
void DrawPingWheel(Font font, PingWheel wheel);
void DrawFloatingTexts(Font font, const std::vector<FloatingText>& floatingTexts, float now);
void DrawHud(Font font, float timeLeft, float score);
