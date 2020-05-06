// win2048.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <vector>
#include "win2048.h"
#include "C2048.h"
#include "block.h"
#include <chrono>
#include <thread>
#include <mutex>

static bool running = true;

const UINT TOP_GAP = 100;
const UINT SIDE_GAP = 200;
const UINT FONT_SIZE = 40;
const UINT BORDER_SIZE = 4;
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

    } 
    break;

    default: 
    {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }
    return result;
}
std::vector<block> initBoard(const int block_count, const Render_State &rs)
{
    std::vector<block> blocks;
    blocks.reserve(block_count*block_count);
    int border_size_total = BORDER_SIZE * (block_count + 1);
    int block_width = (rs.width - border_size_total - SIDE_GAP) / block_count;   // leave spave on sides
    int block_height = (rs.height - border_size_total - TOP_GAP) / block_count;  // space on top for score

    for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            int bx = (j + 1)*BORDER_SIZE + j * block_width + SIDE_GAP/2;
            int by = (i + 1)*BORDER_SIZE + i * block_height + TOP_GAP;
            int ID = j * block_count + i;
            //block b(block_width, block_height, bx, by, ID, &rs);
            blocks.emplace_back(block_width, block_height, bx, by, ID, &rs);
        }
    }
    return blocks;
}
void setBlockValues(std::vector<block> &blocks, const C2048 &board, const HDC hdc)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        int value = board.getVal(i);
        blocks[i].setValue(value);
    }
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
#include "strsafe.h"

void drawTime(const Render_State &rs, const HDC hdc)
{
    const std::lock_guard<std::mutex> lock(gmutex);
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

    long long m = diff / (1000 * 60);
    diff -= m * (1000 * 60);

    long long s = diff / 1000;
    diff -= s * 1000;

    wchar_t buffer[64];
    size_t sz = 0;
    StringCbPrintfEx(buffer, 64, NULL, &sz, STRSAFE_FILL_BEHIND_NULL, L"%ld:%ld.%03d", m, s, diff);
    DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | /*DT_CENTER |*/ DT_VCENTER);

    DeleteObject(SelectObject(hdc, hTmp));

}

void drawTimeThread(const Render_State &rs, const HDC hdc)
{
    while (running)
    {
        drawTime(rs, hdc);
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

void drawBlocks(std::vector<block> &blocks, C2048 &board, const Render_State &rs, const HDC hdc)
{

    const std::lock_guard<std::mutex> lock(gmutex);
    static int score = 0;
    if (score != board.getScore())
    {
        score = board.getScore();
        drawScore(score, rs, hdc);
    }

    if (board.addNewValue() == 0)
    {
        //drawWarning(L"FULL", rs, hdc);
    }
    // load board values into corresponding blocks
    setBlockValues(blocks, board, hdc);

    // draw tiles & text on all blocks
    for (auto &b : blocks)
    {
        b.drawValue(hdc);
    }
    //drawTime(rs, hdc);
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


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    ShowCursor(FALSE);
    // Create Window Class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = L"Game Window Class";
    window_class.lpfnWndProc = window_callback;

    // Register Class
    RegisterClass(&window_class);
    // Create Window
    HWND window = CreateWindow(window_class.lpszClassName, L"2048", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
    {
        //Fullscreen
        SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
        SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

    HDC hdc = GetDC(window);

    //Input input = {};
    int button_pressed = -1;
    float delta_time = 0.016666f;
    LARGE_INTEGER frame_begin_time;
    QueryPerformanceCounter(&frame_begin_time);

    float performance_frequency;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }

    const size_t size = 8;
    C2048 board(size);      // create size X size board
    if (board.addNewValue() == 0)
    {
        //drawWarning(L"FULL", render_state, hdc);
    }
    RECT rec;
    SetRect(&rec, 0, 0, render_state.width, render_state.height);
    clearBackGround(rec, hdc);

    std::vector<block> blocks = initBoard(size, render_state);
    for (auto &b : blocks)
    {
        b.drawTile(hdc);
    }

    drawBlocks(blocks, board, render_state, hdc);
    bool rightStatus = true, leftStatus = true, upStatus = true, downStatus = true;
    std::thread dt(drawTimeThread, render_state, hdc);
    int pause = 0;
    while (running) 
    {
        // Input
        MSG message;


        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) 
        {

            switch (message.message) 
            {
            case WM_LBUTTONDOWN:
                leftStatus = board.toLeft();
                button_pressed = BUTTON_LEFT;
                break;
            case WM_RBUTTONDOWN:
                rightStatus = board.toRight();
                button_pressed = BUTTON_RIGHT;
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
                    button_pressed = BUTTON_UP;
                }
                else
                {
                    downStatus= board.toDown();
                    button_pressed = BUTTON_DOWN;
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
                    button_pressed = BUTTON_UP;
                    break;
                case VK_DOWN:
                case 'S':
                    downStatus = board.toDown();
                    button_pressed = BUTTON_DOWN;
                    break;
                case VK_LEFT:
                case 'A':
                    leftStatus = board.toLeft();
                    button_pressed = BUTTON_LEFT;
                    break;
                case VK_RIGHT:
                case 'D':
                    rightStatus = board.toRight();
                    button_pressed = BUTTON_RIGHT;
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
        if (pause != 1)
        {
            int val = rand() % 4;
            int sleep_count = 1;
            switch (val)
            {
            case 0:
                board.toUp();
                Sleep(sleep_count);
                break;
            case 1:
                board.toRight();
                Sleep(sleep_count);
                break;
            case 2:
                board.toDown();
                Sleep(sleep_count);
                break;
            case 3:
                board.toLeft();
                Sleep(sleep_count);
                break;
            }
            button_pressed = 1;
        }
        if (button_pressed != -1)
        {
            button_pressed = -1;
            drawBlocks(blocks, board, render_state, hdc);
            //if (upStatus == 0 && downStatus == 0 && leftStatus == 0 && rightStatus == 0
            //    && checkForMoreMoves(board) == false)
            if(checkForMoreMoves(board) == false)
            {
                drawWarning(L"No more moves", render_state, hdc);
            }
        }

        LARGE_INTEGER frame_end_time;
        QueryPerformanceCounter(&frame_end_time);
        delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
        frame_begin_time = frame_end_time;
    }
    dt.join();
}
