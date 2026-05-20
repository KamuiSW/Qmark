#pragma once

#include "raylib.h"

#include <string>

float RandomFloat(float minValue, float maxValue);
std::string FormatScore(float value);
float LoadBestScore();
void SaveBestScore(float score);
void EnsureAssetFolderExists();
bool TextureIsValid(Texture2D texture);
bool FontIsValid(Font font);
