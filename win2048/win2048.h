#pragma once
#include <Windows.h>
#include <strsafe.h>
enum
{
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_W,
    BUTTON_S,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_ENTER,

    BUTTON_COUNT, // Should be the last item
};

struct Render_State
{
    int height, width;
};
const UINT TOP_GAP = 100;
const UINT SIDE_GAP = 200;
const UINT FONT_SIZE = 40;
const UINT BORDER_SIZE = 4;

void clearBackGround(RECT &rec, const HDC hdc);
void drawScore(int score, const Render_State &rs, const HDC hdc);
