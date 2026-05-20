#include "Assets.h"
#include "Config.h"
#include "Helpers.h"

static Texture2D LoadRequiredTexture(const char* path, bool& success)
{
    if (!FileExists(path))
    {
        TraceLog(LOG_ERROR, "Missing required asset: %s", path);
        success = false;
        return Texture2D{};
    }

    Texture2D texture = LoadTexture(path);

    if (!TextureIsValid(texture))
    {
        TraceLog(LOG_ERROR, "Failed to load texture: %s", path);
        success = false;
        return Texture2D{};
    }

    return texture;
}

static Font LoadRequiredFont(const char* path, bool& success)
{
    if (!FileExists(path))
    {
        TraceLog(LOG_ERROR, "Missing required font: %s", path);
        success = false;
        return Font{};
    }

    Font font = LoadFontEx(path, 96, nullptr, 0);

    if (!FontIsValid(font))
    {
        TraceLog(LOG_ERROR, "Failed to load font: %s", path);
        success = false;
        return Font{};
    }

    SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);

    return font;
}

bool LoadGameAssets(GameAssets& assets)
{
    EnsureAssetFolderExists();

    bool success = true;

    assets.background = LoadRequiredTexture(BACKGROUND_PATH, success);
    assets.minion = LoadRequiredTexture(MINION_PATH, success);
    assets.npc = LoadRequiredTexture(NPC_PATH, success);
    assets.pingQuestion = LoadRequiredTexture(PING_QUESTION_PATH, success);
    assets.font = LoadRequiredFont(FONT_PATH, success);

    assets.loaded = success;
    return success;
}

void UnloadGameAssets(GameAssets& assets)
{
    if (TextureIsValid(assets.background)) UnloadTexture(assets.background);
    if (TextureIsValid(assets.minion)) UnloadTexture(assets.minion);
    if (TextureIsValid(assets.npc)) UnloadTexture(assets.npc);
    if (TextureIsValid(assets.pingQuestion)) UnloadTexture(assets.pingQuestion);
    if (FontIsValid(assets.font)) UnloadFont(assets.font);
}

void DrawMissingAssetsScreen()
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("ERROR: Missing required assets.", 40, 40, 32, RED);
    DrawText("Required files:", 40, 90, 24, WHITE);
    DrawText("assets/background.png", 40, 125, 22, LIGHTGRAY);
    DrawText("assets/minion.png", 40, 155, 22, LIGHTGRAY);
    DrawText("assets/npc.png", 40, 185, 22, LIGHTGRAY);
    DrawText("assets/ping_question.png", 40, 215, 22, LIGHTGRAY);
    DrawText("assets/Minecraft.ttf", 40, 245, 22, LIGHTGRAY);
    DrawText("Close the window, add the files, then run again.", 40, 300, 22, GOLD);
    EndDrawing();
}
