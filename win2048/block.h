#pragma once
#include "win2048.h"
class block
{
    int width, height, x, y, value;
    int modified;
    const Render_State *rs;
    const int defaultFontSize = 50;
    COLORREF getBGColor(int value) const;
    COLORREF getTextColor(int value) const;
    int getFontSize(int value) const;
    void drawValue(int val, const HDC hdc) const;

public:
    block() :width(0), height(0), x(0), y(0), value(0), rs(nullptr) {}
    block(int w, int h, int _x, int _y, int id, const Render_State *render_state) :
        width(w), height(h), x(_x), y(_y), rs(render_state), value(0), modified(1) {}
    void drawTile(const HDC hdc) const;
    void drawRoundedTile(const HDC hdc) const;
    void setValue(int val);
    void drawValue(const HDC hdc);
};
