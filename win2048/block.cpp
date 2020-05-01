
#include <Windows.h>
#include "block.h"
#include "C2048.h"

void block::set(int w, int h, int _x, int _y, int id, Render_State *render_state)
{
    width = w;
    height = h;
    x = _x;
    y = _y;
    ID = id;
    rs = render_state;
}
void block::draw(HDC hdc)
{
    RECT rec;
    SetRect(&rec, x, y, x + width, y + height);
    HBRUSH hBrush = CreateSolidBrush(getColor(value));
    FillRect(hdc, &rec, hBrush);
    DeleteObject(hBrush);

    //SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    //COLORREF color = GetDCPenColor(hdc);
    //SetDCPenColor(hdc, getColor(value));

    //RoundRect(hdc, rec.left, rec.top, rec.right, rec.bottom, 20, 20);
    //SetDCPenColor(hdc, color);
}
void block::drawRounded(HDC hdc)
{
    int radius = 10;

    //PolyBezier(hdc)
}

void block::setValue(int val)
{
    value = val;
}
COLORREF block::getColor(int value)
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
int block::getFontSize(int value)
{
    int count = C2048::getDigitCount(value);
    switch (count)
    {
    case 1:
        return 50;
    case 2:
        return 45;
    case 3:
        return 40;
    case 4:
        return 35;
    case 5:
    default:
        return 30;
    }
}

void block::drawText(int val, HDC hdc)
{
    int fsize = getFontSize(value);
    HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
    HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 0, getColor(value)));  // 0x48
    SetBkMode(hdc, TRANSPARENT);
    RECT rec;
    SetRect(&rec, x, y, x + width, y + height);

    wchar_t buffer[64];
    wsprintfW(buffer, L"%d", val);
    DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    DeleteObject(SelectObject(hdc, hTmp));

}
void block::drawText(HDC hdc)
{
    if (value != C2048::defaultValue)
    {
        drawText(value, hdc);
    }
}

