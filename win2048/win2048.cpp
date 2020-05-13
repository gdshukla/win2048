// win2048.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <strsafe.h>

#include "win2048.h"
#include "C2048.h"
#include "blocks.h"

static bool running = true;
static bool resize = false;
static Render_State render_state;
std::mutex gmutex;


LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    LRESULT result = 0;

    switch (uMsg) 
    {
    case WM_CLOSE:
    case WM_DESTROY: 
    {
        running = false;
    } 
    break;

    case WM_SIZE: 
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        render_state.width = rect.right - rect.left;
        render_state.height = rect.bottom - rect.top;
        resize = true;
    } 
    break;

    default: 
    {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return result;
}
void clearBackGround(RECT &rec, const HDC hdc)
{
    // fill black background
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 20));
    FillRect(hdc, &rec, hBrush);
    DeleteObject(hBrush);
}

void drawScore(int score, const Render_State &rs, const HDC hdc)
{
    const std::lock_guard<std::mutex> lock(gmutex);
    int fsize = FONT_SIZE;
    HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
    HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
    SetBkMode(hdc, TRANSPARENT);
    RECT rec;
    SetRect(&rec, SIDE_GAP/2, 0, rs.width/2, TOP_GAP);
    clearBackGround(rec, hdc);

    wchar_t buffer[32];
    static long count = 0;
    wsprintfW(buffer, L"Score: %d (%d)", score, count++);
    DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | /*DT_CENTER |*/ DT_VCENTER);

    DeleteObject(SelectObject(hdc, hTmp));

}

void drawTime(const Render_State &rs, const HDC hdc)
{
    static std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
    auto current = std::chrono::system_clock::now();
    long long diff = (std::chrono::duration_cast<std::chrono::milliseconds>(current - start).count());

    int fsize = FONT_SIZE;
    HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
    HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0, 0xff, 0));
    SetBkMode(hdc, TRANSPARENT);
    RECT rec;
    SetRect(&rec, rs.width / 2, 0, rs.width, TOP_GAP);
    clearBackGround(rec, hdc);

    int m = static_cast<int>(diff / (1000 * 60));
    diff -= m * (1000 * 60);

    int s = static_cast<int>(diff / 1000);
    diff -= s * 1000;

    wchar_t buffer[64];
    size_t sz = 0;
    //StringCbPrintfEx(buffer, sizeof(buffer), NULL, &sz, STRSAFE_FILL_BEHIND_NULL, L"%ld:%ld.%03ld", m, s, diff);
    StringCbPrintf(buffer, sizeof(buffer), L"%d:%d.%03d", m, s, static_cast<int>(diff));
    DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | /*DT_CENTER |*/ DT_VCENTER);

    DeleteObject(SelectObject(hdc, hTmp));

}

void drawTimeThread(const HDC hdc)
{
    while (running)
    {
        {
            const std::lock_guard<std::mutex> lock(gmutex);
            drawTime(render_state, hdc);
        }
        Sleep(111);
    }
}


void drawWarning(const std::wstring text, const Render_State &rs, const HDC hdc)
{
    const std::lock_guard<std::mutex> lock(gmutex);
    int fsize = FONT_SIZE;
    HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
    HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0xff, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
    RECT rec;
    SetRect(&rec, rs.width/2, 0, rs.width, TOP_GAP);
    clearBackGround(rec, hdc);

    wchar_t buffer[64];
    wsprintfW(buffer, L"%s", text.c_str());
    DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    DeleteObject(SelectObject(hdc, hTmp));

}


bool checkForMoreMoves(const C2048 board)
{
    C2048 temp(board);
    bool n = temp.addNewValue();
    bool r = temp.toRight();
    bool l = temp.toLeft();
    bool u = temp.toUp();
    bool d = temp.toDown();
    //r = temp.toRight();
    if (n == false && r == false && l == false && u == false && d == false)
    {
        return false;
    }
    return true;
}

#define FULL_SCREEN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef FULL_SCREEN
    ShowCursor(false);
#else
    ShowCursor(true);
#endif

    // Create Window Class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = L"Game Window Class";
    window_class.lpfnWndProc = window_callback;

    // Register Class
    RegisterClass(&window_class);
    // Create Window
    HWND window = CreateWindow(window_class.lpszClassName, L"2048", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        CW_USEDEFAULT, CW_USEDEFAULT, 1200, 600, 0, 0, hInstance, 0);
    {
#ifdef FULL_SCREEN
        //Fullscreen
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
        SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
#endif
    }

    HDC hdc = GetDC(window);

    //Input input = {};
    bool update = true;
    float delta_time = 0.016666f;
    LARGE_INTEGER frame_begin_time;
    QueryPerformanceCounter(&frame_begin_time);

    float performance_frequency;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }

    const size_t size = 10;
    C2048 board(size);      // create size X size board
    if (board.addNewValue() == 0)
    {
        //drawWarning(L"FULL", render_state, hdc);
    }
    RECT rec;
    SetRect(&rec, 0, 0, render_state.width, render_state.height);
    clearBackGround(rec, hdc);

    Blocks blocks(size, render_state, hdc/*, gmutex*/);
    if (board.addNewValue() == 0)
    {
        //drawWarning(L"FULL", rs, hdc);
    }
    int score = 0;
    if (score != board.getScore())
    {
        score = board.getScore();
        drawScore(score, render_state, hdc);
    }

    blocks.drawBlocks(board);

    bool rightStatus = true, leftStatus = true, upStatus = true, downStatus = true;
    std::thread dt(drawTimeThread, hdc);
    int pause = 1;
    while (running) 
    {
        //if (score != board.getScore())
        //{
        //    score = board.getScore();
        //    drawScore(score, render_state, hdc);
        //}

        // Input
        MSG message;
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) 
        {

            switch (message.message) 
            {
            case WM_LBUTTONDOWN:
                leftStatus = board.toLeft();
                update = true;
                break;
            case WM_RBUTTONDOWN:
                rightStatus = board.toRight();
                update = true;
                break;
            case WM_MBUTTONDOWN:
                running = false;
                break;
            case WM_MOUSEWHEEL:
            {
                int delta = (int)message.wParam;
                if (delta >> 16 > 0)
                {
                    upStatus = board.toUp();
                    update = true;
                }
                else
                {
                    downStatus= board.toDown();
                    update = true;
                }
            }
                break;
            case WM_KEYUP:
                break;
            case WM_KEYDOWN:
            {
                UINT32 vk_code = (UINT32)message.wParam;

                switch (vk_code)
                {
                case VK_PAUSE:
                case VK_HOME:
                    pause = !pause;
                    break;
                case VK_UP:
                case 'W':
                    upStatus = board.toUp();
                    update = true;
                    break;
                case VK_DOWN:
                case 'S':
                    downStatus = board.toDown();
                    update = true;
                    break;
                case VK_LEFT:
                case 'A':
                    leftStatus = board.toLeft();
                    update = true;
                    break;
                case VK_RIGHT:
                case 'D':
                    rightStatus = board.toRight();
                    update = true;
                    break;
                case VK_ESCAPE:
                case 'Q':
                    running = false;
                    break;
                }
            } break;

                default:
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
            }

        }

        // in case window size changes, recalculate size and coorinates of blocks and resize the screen
#ifndef FULL_SCREEN
        if (resize == true)
        {
            const std::lock_guard<std::mutex> lock(gmutex);
            RECT rec;
            SetRect(&rec, 0, 0, render_state.width, render_state.height);
            clearBackGround(rec, hdc);
            Blocks b(size, render_state, hdc/*, gmutex*/);
            blocks = b;
            //blocks.setupBlocks(size, render_state);
            //blocks = initBoard(size, render_state);
            resize = false;
            update = true;
        }
#endif
        if (pause != 1)
        {
            int val = rand() % 4;
            int sleep_count = 1;
            switch (val)
            {
            case 0:
                board.toUp();
                //Sleep(sleep_count);
                break;
            case 1:
                board.toRight();
                //Sleep(sleep_count);
                break;
            case 2:
                board.toDown();
                //Sleep(sleep_count);
                break;
            case 3:
                board.toLeft();
                //Sleep(sleep_count);
                break;
            }
            update = true;
        }
        if (update == true)
        {
            update = false;
            drawScore(board.getScore(), render_state, hdc);

            const std::lock_guard<std::mutex> lock(gmutex);
            blocks.drawBlocks(board);
            if(checkForMoreMoves(board) == false)
            {
                drawWarning(L"No more moves", render_state, hdc);
            }
            board.addNewValue();
            //board.addNewValue();

        }

        //LARGE_INTEGER frame_end_time;
        //QueryPerformanceCounter(&frame_end_time);
        //delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
        //frame_begin_time = frame_end_time;
    }
    dt.join();
}

