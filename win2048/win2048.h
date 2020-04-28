#pragma once
#include <Windows.h>

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

