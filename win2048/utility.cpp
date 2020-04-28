#include <windows.h>

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

unsigned long long getTickCount()
{
    return GetTickCount64();
}


int clamp(int min, int max, int val)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}
