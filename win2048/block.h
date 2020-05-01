#pragma once
#include "win2048.h"
class block
{
    int width, height, x, y, ID, value;
    const Render_State *rs;
    COLORREF getColor(int value) const;
    int getFontSize(int value) const;
    void drawText(int val, const HDC hdc) const;

public:
    block() :width(0), height(0), x(0), y(0), ID(0), value(0), rs(nullptr) {}
    block(const Render_State *render_state) :width(0), height(0), x(0), y(0), ID(0), value(0), rs(render_state) {}
    block(int w, int h, int _x, int _y, int id, const Render_State *render_state) :
        width(w), height(h), x(_x), y(_y), ID(id), rs(render_state) {}
    void set(int w, int h, int _x, int _y, int id, const Render_State *render_state);
    void draw(const HDC hdc) const;
    void drawRounded(const HDC hdc) const;
    void setValue(int val);
    void drawText(const HDC hdc) const;
};
