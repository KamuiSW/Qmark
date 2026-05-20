#pragma once

static const int SCREEN_WIDTH = 1280;
static const int SCREEN_HEIGHT = 720;

static const float GAME_LENGTH = 60.0f;
static const float COUNTDOWN_LENGTH = 3.0f;

static const float DEFAULT_MINION_SPAWN_INTERVAL = 0.85f;
static const float FAST_MINION_SPAWN_INTERVAL = 0.35f;
static const float SLOW_MINION_SPAWN_INTERVAL = 1.40f;
static const float MINION_ATTACK_DELAY_MIN = 0.8f;
static const float MINION_ATTACK_DELAY_MAX = 1.5f;

static const float PING_WINDOW = 1.5f;
static const float SUCCESS_VISIBLE_TIME = 0.9f;
static const float MISSED_MINION_FADE_TIME = 1.5f;

static const float PING_RADIUS = 85.0f;
static const float MINION_MIN_SPAWN_DISTANCE = 130.0f;

static const float MINION_DRAW_SIZE = 72.0f;
static const float NPC_DRAW_SIZE = 96.0f;
static const float PING_QUESTION_DRAW_SIZE = 120.0f;
static const float PING_VISUAL_DURATION = 1.35f;

static const float BASE_MISS_PING_SCORE = 1.0f;
static const float HELPER_OFF_BONUS = 0.5f;
static const float SUCCESS_NOT_PINGED_SCORE = 1.0f;
static const float WRONG_PING_PENALTY = -1.0f;

static const char* BEST_SCORE_FILE = "best_score.txt";
static const char* ASSET_FOLDER = "assets";

static const char* BACKGROUND_PATH = "assets/background.png";
static const char* MINION_PATH = "assets/minion.png";
static const char* NPC_PATH = "assets/npc.png";
static const char* PING_QUESTION_PATH = "assets/ping_question.png";
static const char* FONT_PATH = "assets/Minecraft.ttf";
