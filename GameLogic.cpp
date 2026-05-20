#include "GameLogic.h"
#include "Config.h"
#include "Helpers.h"

#include "raymath.h"

#include <algorithm>
#include <cmath>

void AddFloatingText(std::vector<FloatingText>& texts, Vector2 pos, const std::string& text, Color color, float now, float duration)
{
    texts.push_back(FloatingText{ pos, text, now, duration, color });
}

void AddPingVisual(std::vector<PingVisual>& pings, Vector2 pos, float now)
{
    pings.push_back(PingVisual{ pos, now, PING_VISUAL_DURATION });
}

float GetSpeedScoreMultiplier(const Settings& settings)
{
    float spawnInterval = settings.minionSpawnInterval;

    if (spawnInterval < FAST_MINION_SPAWN_INTERVAL) spawnInterval = FAST_MINION_SPAWN_INTERVAL;
    if (spawnInterval > SLOW_MINION_SPAWN_INTERVAL) spawnInterval = SLOW_MINION_SPAWN_INTERVAL;

    return DEFAULT_MINION_SPAWN_INTERVAL / spawnInterval;
}

float GetCorrectMissPingScore(const Settings& settings)
{
    float value = BASE_MISS_PING_SCORE;

    if (!settings.showMissFadeHint) value += HELPER_OFF_BONUS;
    if (!settings.showPingAreaHelper) value += HELPER_OFF_BONUS;

    return value * GetSpeedScoreMultiplier(settings);
}

float GetSuccessfulMinionScore(const Settings& settings)
{
    return SUCCESS_NOT_PINGED_SCORE * GetSpeedScoreMultiplier(settings);
}

void RegisterPing(Vector2 pingPos, std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, float& score, const Settings& settings, float now)
{
    AddPingVisual(pingVisuals, pingPos, now);

    MinionEvent* closest = nullptr;
    float closestDistance = 99999.0f;

    for (auto& m : minions)
    {
        if (!m.resolved) continue;
        if (m.pinged) continue;

        float age = now - m.resolvedTime;
        if (age > PING_WINDOW) continue;

        float distance = Vector2Distance(pingPos, m.pos);

        if (distance < PING_RADIUS && distance < closestDistance)
        {
            closest = &m;
            closestDistance = distance;
        }
    }

    if (closest == nullptr)
    {
        AddFloatingText(floatingTexts, pingPos, "MISS", GRAY, now);
        return;
    }

    closest->pinged = true;

    if (closest->success)
    {
        score += WRONG_PING_PENALTY;
        AddFloatingText(floatingTexts, pingPos, "-1", RED, now);
    }
    else
    {
        float gained = GetCorrectMissPingScore(settings);
        score += gained;
        AddFloatingText(floatingTexts, pingPos, "+" + FormatScore(gained), SKYBLUE, now);
    }
}

static bool IsGoodSpawnPosition(Vector2 pos, const std::vector<MinionEvent>& minions)
{
    for (const auto& m : minions)
    {
        float distance = Vector2Distance(pos, m.pos);
        if (distance < MINION_MIN_SPAWN_DISTANCE) return false;
    }

    return true;
}

Vector2 GenerateMinionSpawnPosition(const std::vector<MinionEvent>& minions)
{
    for (int attempt = 0; attempt < 40; attempt++)
    {
        Vector2 pos;
        pos.x = RandomFloat(150, SCREEN_WIDTH - 150);
        pos.y = RandomFloat(170, SCREEN_HEIGHT - 120);

        if (IsGoodSpawnPosition(pos, minions)) return pos;
    }

    return { RandomFloat(150, SCREEN_WIDTH - 150), RandomFloat(170, SCREEN_HEIGHT - 120) };
}

void ResetGame(std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, float& score, float& gameStartTime, float& nextSpawnTime, const Settings& settings)
{
    minions.clear();
    floatingTexts.clear();
    pingVisuals.clear();

    score = 0.0f;
    gameStartTime = (float)GetTime();
    nextSpawnTime = (float)GetTime() + settings.minionSpawnInterval;
}

static void SpawnMinionIfNeeded(std::vector<MinionEvent>& minions, float& nextSpawnTime, const Settings& settings, float now)
{
    if (now < nextSpawnTime) return;

    MinionEvent m;
    m.pos = GenerateMinionSpawnPosition(minions);
    m.spawnTime = now;
    m.attackTime = now + RandomFloat(MINION_ATTACK_DELAY_MIN, MINION_ATTACK_DELAY_MAX);
    minions.push_back(m);

    nextSpawnTime = now + settings.minionSpawnInterval;
}

static void ResolveMinions(std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, float now)
{
    for (auto& m : minions)
    {
        if (!m.resolved && now >= m.attackTime)
        {
            m.resolved = true;
            m.resolvedTime = now;
            m.success = GetRandomValue(0, 100) < 65;

            if (m.success)
            {
                AddFloatingText(floatingTexts, m.pos, "+60", GOLD, now, SUCCESS_VISIBLE_TIME);
            }
        }
    }
}

static void GiveLazySuccessPoints(std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, float& score, const Settings& settings, float now)
{
    for (auto& m : minions)
    {
        if (!m.resolved) continue;
        if (!m.success) continue;
        if (m.pinged) continue;
        if (m.successRewarded) continue;

        float age = now - m.resolvedTime;

        if (age > PING_WINDOW)
        {
            float gained = GetSuccessfulMinionScore(settings);
            m.successRewarded = true;
            score += gained;
            AddFloatingText(floatingTexts, m.pos, "+" + FormatScore(gained), GREEN, now);
        }
    }
}

static void UpdatePingWheel(PingWheel& pingWheel, std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, float& score, const Settings& settings, float now)
{
    bool altDown = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);

    if (altDown && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        pingWheel.active = true;
        pingWheel.startPos = GetMousePosition();
        pingWheel.currentPos = pingWheel.startPos;
    }

    if (!pingWheel.active) return;

    pingWheel.currentPos = GetMousePosition();

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        Vector2 delta = Vector2Subtract(pingWheel.currentPos, pingWheel.startPos);
        bool draggedLeft = delta.x < -45.0f;
        bool notTooVertical = std::abs(delta.y) < 80.0f;

        if (draggedLeft && notTooVertical)
        {
            RegisterPing(pingWheel.startPos, minions, floatingTexts, pingVisuals, score, settings, now);
        }

        pingWheel.active = false;
    }

    if (!altDown)
    {
        pingWheel.active = false;
    }
}

static void ThrowAwayOldStuff(std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, float now)
{
    minions.erase(std::remove_if(minions.begin(), minions.end(), [now](const MinionEvent& m)
    {
        if (!m.resolved) return false;
        float age = now - m.resolvedTime;
        float keepTime = std::max(2.2f, std::max(PING_WINDOW + 0.3f, MISSED_MINION_FADE_TIME + 0.3f));
        return age > keepTime;
    }), minions.end());

    floatingTexts.erase(std::remove_if(floatingTexts.begin(), floatingTexts.end(), [now](const FloatingText& t)
    {
        return now - t.startTime > t.duration;
    }), floatingTexts.end());

    pingVisuals.erase(std::remove_if(pingVisuals.begin(), pingVisuals.end(), [now](const PingVisual& p)
    {
        return now - p.startTime > p.duration;
    }), pingVisuals.end());
}

void UpdatePlayingGame(std::vector<MinionEvent>& minions, std::vector<FloatingText>& floatingTexts, std::vector<PingVisual>& pingVisuals, PingWheel& pingWheel, float& score, float& nextSpawnTime, const Settings& settings, Vector2 npcPos, float now)
{
    (void)npcPos;

    SpawnMinionIfNeeded(minions, nextSpawnTime, settings, now);
    ResolveMinions(minions, floatingTexts, now);
    GiveLazySuccessPoints(minions, floatingTexts, score, settings, now);
    UpdatePingWheel(pingWheel, minions, floatingTexts, pingVisuals, score, settings, now);
    ThrowAwayOldStuff(minions, floatingTexts, pingVisuals, now);
}
