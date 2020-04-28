#pragma once
#include <Windows.h>
//struct Button_State
//{
//    bool is_down;
//    bool changed;
//};

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
//struct Input
//{
//    Button_State buttons[BUTTON_COUNT];
//
//};

struct Render_State
{
    int height, width;
    //void* memory;

    //BITMAPINFO bitmap_info;
};

//inline int
//clamp(int min, int val, int max)
//{
//    if (val < min) return min;
//    if (val > max) return max;
//    return val;
//}

//void simulate_game(Input* input, float dt, Render_State *render_state);
//void draw_arena_borders(float arena_x, float arena_y, UINT32 color, Render_State *render_state);
//void draw_rect(float x, float y, float half_size_x, float half_size_y, UINT32 color, Render_State *render_state);
//void draw_text(const char *text, float x, float y, float size, UINT32 color, Render_State *render_state);
//void draw_number(int number, float x, float y, float size, UINT32 color, Render_State *render_state);
//void draw_rect_in_pixels(int x0, int y0, int x1, int y1, UINT32 color, Render_State *render_state);
//
