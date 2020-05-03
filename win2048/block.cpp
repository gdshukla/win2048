
#include <Windows.h>
#include "block.h"
#include "C2048.h"

//void block::set(int w, int h, int _x, int _y, int id, const Render_State *render_state)
//{
//    width = w;
//    height = h;
//    x = _x;
//    y = _y;
//    ID = id;
//    rs = render_state;
//}
void block::drawTile(const HDC hdc) const
{
    RECT rec;
    SetRect(&rec, x, y, x + width, y + height);
    HBRUSH hBrush = CreateSolidBrush(getBGColor(value));
    FillRect(hdc, &rec, hBrush);
    DeleteObject(hBrush);
    //drawRoundedTile(hdc);
}

void block::drawRoundedTile(const HDC hdc) const
{
    SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    COLORREF color = GetDCBrushColor(hdc);
    SetDCBrushColor(hdc, RGB(0,0,0)); // getBGColor(value)

    RECT rec;
    SetRect(&rec, x, y, x + width, y + height);
    const int roundness = 20;
    RoundRect(hdc, rec.left, rec.top, rec.right, rec.bottom, roundness, roundness);
    SetDCBrushColor(hdc, color);
}

void block::setValue(int val)
{
    if (val != value)
    {
        value = val;
        modified = 1;
    }
}

COLORREF block::getBGColor(int value) const
{
    COLORREF color = RGB(0, 0, 0);
    int col1 = 70;
    int col2 = 170;
    switch (value)
    {
    case 0:
        return RGB(204, 196, 136);
    case 2:
    case 8:
        return RGB(col2, col2, col2);
    case 4:
    case 16:
        return RGB(col2, col2, col1);
    case 32:
    case 128:
        return RGB(col1, col2, col1);
    case 64:
    case 256:
        return RGB(col1, col2, col2);
    case 512:
    case 1024 * 2:
        return RGB(col1, col1, col2);
    case 1024:
    case 1024 * 4:
        return RGB(col2, col1, col2);
    case 1024 * 8:
    case 1024 * 32:
        return RGB(col2, col1, col1);
    case 1024 * 16:
    case 1024 * 64:
        return RGB(0, 0, 0);
    }
    return color;
}
COLORREF block::getTextColor(int value) const
{
    COLORREF color = RGB(0, 0, 0);
    int col1 = 10;
    int col2 = 68;
    switch (value)
    {
    case 0:
        return RGB(204, 196, 136);
    case 2:
    case 8:
        return RGB(col2, col2, col2);
    case 4:
    case 16:
        return RGB(col2, col2, col1);
    case 32:
    case 128:
        return RGB(col1, col2, col1);
    case 64:
    case 256:
        return RGB(col1, col2, col2);
    case 512:
    case 1024 * 2:
        return RGB(col1, col1, col2);
    case 1024:
    case 1024 * 4:
        return RGB(col2, col1, col2);
    case 1024 * 8:
    case 1024 * 32:
        return RGB(col2, col1, col1);
    case 1024 * 16:
    case 1024 * 64:
        return RGB(0, 0, 0);
    }
    return color;
}
int block::getFontSize(int value) const
{
    int count = C2048::getDigitCount(value);    // get number of digits in value
    switch (count)
    {
    case 1:
        return defaultFontSize;
    case 2:
        return defaultFontSize - 5;
    case 3:
        return defaultFontSize - 10;
    case 4:
        return defaultFontSize - 15;
    case 5:
    default:
        return defaultFontSize - 20;
    }
}

void block::drawValue(int val, const HDC hdc) const
{
    if (value != C2048::defaultValue)
    {
        int fsize = getFontSize(value);
        HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
        HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
        SetTextColor(hdc, getTextColor(value));  // 0x48
        //SetTextColor(hdc, RGB(0,0,0x48));  // 0x48
        SetBkMode(hdc, TRANSPARENT);
        RECT rec;
        SetRect(&rec, x, y, x + width, y + height);

        wchar_t buffer[64];
        wsprintfW(buffer, L"%d", val);
        DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        DeleteObject(SelectObject(hdc, hTmp));

    }

}
void block::drawValue(const HDC hdc)
{
    if (modified == 1)
    {
        drawTile(hdc);
        drawValue(value, hdc);
        modified = 0;
    }
}

