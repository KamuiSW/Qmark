#pragma once

#include "raylib.h"
#include "Config.h"

#include <string>

enum class GameState
{
    Menu,
    Countdown,
    Playing,
    GameOver
};

struct Button
{
    Rectangle rect;
    std::string text;
};

struct GameAssets
{
    Texture2D background{};
    Texture2D minion{};
    Texture2D npc{};
    Texture2D pingQuestion{};
    Font font{};
    bool loaded = false;
};

struct Settings
{
    bool showMissFadeHint = true;
    bool showPingAreaHelper = true;
    float minionSpawnInterval = DEFAULT_MINION_SPAWN_INTERVAL;
};

struct MinionEvent
{
    Vector2 pos;
    float spawnTime = 0.0f;
    float attackTime = 0.0f;
    float resolvedTime = 0.0f;
    bool resolved = false;
    bool success = false;
    bool pinged = false;
    bool successRewarded = false;
};

struct FloatingText
{
    Vector2 pos;
    std::string text;
    float startTime;
    float duration;
    Color color;
};

struct PingVisual
{
    Vector2 pos;
    float startTime;
    float duration;
};

struct PingWheel
{
    bool active = false;
    Vector2 startPos = { 0, 0 };
    Vector2 currentPos = { 0, 0 };
};
