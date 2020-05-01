#pragma once
#include "win2048.h"
class block
{
    int width, height, x, y, ID, value;
    Render_State *rs;
public:
    block() :width(0), height(0), x(0), y(0), ID(0), value(0), rs(nullptr) {}
    block(Render_State *render_state) :width(0), height(0), x(0), y(0), ID(0), value(0), rs(render_state) {}
    block(int w, int h, int _x, int _y, int id, Render_State *render_state) :
        width(w), height(h), x(_x), y(_y), ID(id), rs(render_state) {}
    void set(int w, int h, int _x, int _y, int id, Render_State *render_state);
    void draw(HDC hdc);
    void drawRounded(HDC hdc);
    void setValue(int val);
    COLORREF getColor(int value);
    int getFontSize(int value);
    void drawText(int val, HDC hdc);
    void drawText(HDC hdc);
};
