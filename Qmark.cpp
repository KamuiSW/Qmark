#include "Assets.h"
#include "Config.h"
#include "GameLogic.h"
#include "Helpers.h"
#include "Render.h"
#include "Ui.h"

#include "raylib.h"

#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

static bool DrawMenu(const GameAssets& assets, Settings& settings, float bestScore, GameState& state, float& countdownStartTime, float now)
{
    Button playButton = { { SCREEN_WIDTH / 2.0f - 150, 285, 300, 65 }, "PLAY" };
    Button quitButton = { { SCREEN_WIDTH / 2.0f - 150, 370, 300, 65 }, "QUIT" };
    Rectangle missHintToggle = { SCREEN_WIDTH / 2.0f - 220, 465, 440, 50 };
    Rectangle pingAreaToggle = { SCREEN_WIDTH / 2.0f - 220, 525, 440, 50 };
    Rectangle spawnSlider = { SCREEN_WIDTH / 2.0f - 220, 630, 440, 34 };

    DrawBackground(assets);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 135 });

    DrawCenteredTextFont(assets.font, "Qmark pinging simulator", 95, 42, SKYBLUE);

    std::string best = "Best Score: " + FormatScore(bestScore);
    DrawCenteredTextFont(assets.font, best, 185, 28, GOLD);

    if (DrawButton(assets.font, playButton))
    {
        countdownStartTime = now;
        state = GameState::Countdown;
    }

    if (DrawButton(assets.font, quitButton))
    {
        return true;
    }

    if (DrawToggleButton(assets.font, missHintToggle, "Miss Fade Hint", settings.showMissFadeHint))
    {
        settings.showMissFadeHint = !settings.showMissFadeHint;
    }

    if (DrawToggleButton(assets.font, pingAreaToggle, "Ping Area Helper", settings.showPingAreaHelper))
    {
        settings.showPingAreaHelper = !settings.showPingAreaHelper;
    }

    DrawSlider(assets.font, spawnSlider, "Next minion spawn", settings.minionSpawnInterval, FAST_MINION_SPAWN_INTERVAL, SLOW_MINION_SPAWN_INTERVAL);

    std::string multiplier = "Points x" + FormatScore(GetSpeedScoreMultiplier(settings));
    DrawCenteredTextFont(assets.font, multiplier, 675, 18, GOLD);

    return false;
}

static void DrawCountdown(const GameAssets& assets, float countdownStartTime, float now)
{
    DrawBackground(assets);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 160 });

    float elapsed = now - countdownStartTime;
    int number = 3 - (int)elapsed;
    if (number < 1) number = 1;

    DrawCenteredTextFont(assets.font, "Get ready...", 205, 40, LIGHTGRAY);
    DrawCenteredTextFont(assets.font, std::to_string(number), 300, 120, SKYBLUE);
}

static void DrawPlaying(const GameAssets& assets, const Settings& settings, const std::vector<MinionEvent>& minions, const std::vector<FloatingText>& floatingTexts, const std::vector<PingVisual>& pingVisuals, PingWheel pingWheel, Vector2 npcPos, float timeLeft, float score, float now)
{
    DrawBackground(assets);
    DrawNPCSprite(assets, npcPos);

    for (const auto& m : minions)
    {
        if (!m.resolved)
        {
            DrawLineEx(npcPos, m.pos, 2, Color{ 255, 120, 120, 80 });
            DrawMinionSprite(assets, m.pos);
            continue;
        }

        float age = now - m.resolvedTime;

        if (!m.success)
        {
            if (settings.showMissFadeHint && age <= MISSED_MINION_FADE_TIME)
            {
                float alpha = 1.0f - age / MISSED_MINION_FADE_TIME;
                if (alpha < 0.0f) alpha = 0.0f;
                DrawMissedMinionSprite(assets, m.pos, alpha);
            }

            if (settings.showPingAreaHelper && age <= PING_WINDOW)
            {
                float alpha = 1.0f - age / PING_WINDOW;
                if (alpha < 0.0f) alpha = 0.0f;
                DrawPingArea(m.pos, alpha);
            }
        }
    }

    for (const auto& p : pingVisuals)
    {
        float age = now - p.startTime;
        float alpha = 1.0f - age / p.duration;
        if (alpha < 0.0f) alpha = 0.0f;
        DrawQuestionPingSprite(assets, p.pos, alpha);
    }

    DrawFloatingTexts(assets.font, floatingTexts, now);

    if (pingWheel.active)
    {
        DrawPingWheel(assets.font, pingWheel);
    }

    DrawHud(assets.font, timeLeft, score);
}

static void DrawGameOver(const GameAssets& assets, float score, float bestScore, GameState& state, float& countdownStartTime, float now)
{
    DrawBackground(assets);
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 155 });

    DrawCenteredTextFont(assets.font, "GAME OVER", 135, 62, SKYBLUE);
    DrawCenteredTextFont(assets.font, "Your Score: " + FormatScore(score), 245, 34, WHITE);
    DrawCenteredTextFont(assets.font, "Best Score: " + FormatScore(bestScore), 295, 30, GOLD);

    Button againButton = { { SCREEN_WIDTH / 2.0f - 150, 390, 300, 70 }, "PLAY AGAIN" };
    Button menuButton = { { SCREEN_WIDTH / 2.0f - 150, 480, 300, 70 }, "MENU" };

    if (DrawButton(assets.font, againButton))
    {
        countdownStartTime = now;
        state = GameState::Countdown;
    }

    if (DrawButton(assets.font, menuButton))
    {
        state = GameState::Menu;
    }
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Qmark pinging simulator");
    SetTargetFPS(60);
    srand((unsigned int)time(nullptr));

    EnsureAssetFolderExists();

    GameAssets assets;

    if (!LoadGameAssets(assets))
    {
        DrawMissingAssetsScreen();

        while (!WindowShouldClose())
        {
            BeginDrawing();
            EndDrawing();
        }

        CloseWindow();
        return 1;
    }

    Settings settings;
    GameState state = GameState::Menu;

    float score = 0.0f;
    float bestScore = LoadBestScore();
    float countdownStartTime = 0.0f;
    float gameStartTime = 0.0f;
    float nextSpawnTime = 0.0f;

    std::vector<MinionEvent> minions;
    std::vector<FloatingText> floatingTexts;
    std::vector<PingVisual> pingVisuals;

    PingWheel pingWheel;
    Vector2 npcPos = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    while (!WindowShouldClose())
    {
        float now = (float)GetTime();
        bool askedToQuit = false;

        if (state == GameState::Countdown)
        {
            float elapsed = now - countdownStartTime;

            if (elapsed >= COUNTDOWN_LENGTH)
            {
                ResetGame(minions, floatingTexts, pingVisuals, score, gameStartTime, nextSpawnTime, settings);
                state = GameState::Playing;
            }
        }

        if (state == GameState::Playing)
        {
            float gameElapsed = now - gameStartTime;
            float timeLeft = GAME_LENGTH - gameElapsed;

            if (timeLeft <= 0.0f)
            {
                if (score > bestScore)
                {
                    bestScore = score;
                    SaveBestScore(bestScore);
                }

                state = GameState::GameOver;
            }
            else
            {
                UpdatePlayingGame(minions, floatingTexts, pingVisuals, pingWheel, score, nextSpawnTime, settings, npcPos, now);
            }
        }

        BeginDrawing();

        if (state == GameState::Menu)
        {
            askedToQuit = DrawMenu(assets, settings, bestScore, state, countdownStartTime, now);
        }
        else if (state == GameState::Countdown)
        {
            DrawCountdown(assets, countdownStartTime, now);
        }
        else if (state == GameState::Playing)
        {
            float timeLeft = GAME_LENGTH - (now - gameStartTime);
            DrawPlaying(assets, settings, minions, floatingTexts, pingVisuals, pingWheel, npcPos, timeLeft, score, now);
        }
        else if (state == GameState::GameOver)
        {
            DrawGameOver(assets, score, bestScore, state, countdownStartTime, now);
        }

        EndDrawing();

        if (askedToQuit)
        {
            break;
        }
    }

    UnloadGameAssets(assets);
    CloseWindow();

    return 0;
}
