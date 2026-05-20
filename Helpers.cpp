#include "Helpers.h"
#include "Config.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

float RandomFloat(float minValue, float maxValue)
{
    float t = (float)GetRandomValue(0, 10000) / 10000.0f;
    return minValue + (maxValue - minValue) * t;
}

std::string FormatScore(float value)
{
    std::ostringstream stream;

    if (std::fabs(value - std::round(value)) < 0.001f)
    {
        stream << (int)std::round(value);
    }
    else
    {
        stream << std::fixed << std::setprecision(1) << value;
    }

    return stream.str();
}

float LoadBestScore()
{
    std::ifstream file(BEST_SCORE_FILE);
    float score = 0.0f;

    if (file.is_open())
    {
        file >> score;
    }

    return score;
}

void SaveBestScore(float score)
{
    std::ofstream file(BEST_SCORE_FILE);

    if (file.is_open())
    {
        file << score;
    }
}

void EnsureAssetFolderExists()
{
    if (!DirectoryExists(ASSET_FOLDER))
    {
        MakeDirectory(ASSET_FOLDER);
    }
}

bool TextureIsValid(Texture2D texture)
{
    return texture.id > 0;
}

bool FontIsValid(Font font)
{
    return font.texture.id > 0;
}
