#pragma once

#include "Types.h"

#include <vector>

void AddFloatingText(std::vector<FloatingText>& texts, Vector2 pos, const std::string& text, Color color, float now, float duration = 0.8f);
void AddPingVisual(std::vector<PingVisual>& pings, Vector2 pos, float now);
float GetSpeedScoreMultiplier(const Settings& settings);
float GetCorrectMissPingScore(const Settings& settings);
float GetSuccessfulMinionScore(const Settings& settings);
void RegisterPing(Vector2 pingPos, std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, float& score, const Settings& settings, float now);
Vector2 GenerateMinionSpawnPosition(const std::vector<MinionEvent>& minions);
void ResetGame(std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, float& score, float& gameStartTime, float& nextSpawnTime, const Settings& settings);
void UpdatePlayingGame(std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, PingWheel& pingWheel, float& score, float& nextSpawnTime, const Settings& settings, Vector2 npcPos, float now);
