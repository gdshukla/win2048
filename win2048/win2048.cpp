// win2048.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <vector>
#include "win2048.h"
#include "C2048.h"

static bool running = true;

static Render_State render_state;
const UINT TOP_GAP = 100;
const UINT SIDE_GAP = 200;

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
class block
{
    int width, height, x, y, ID, value;
    Render_State *rs;
public:
    block() :width(0), height(0), x(0), y(0), ID(0), value(0), rs(nullptr) {}
    block(Render_State *render_state) :width(0), height(0), x(0), y(0), ID(0), value(0), rs(render_state) {}
    block(int w, int h, int _x, int _y, int id, Render_State *render_state) : 
        width(w), height(h), x(_x), y(_y), ID(id), rs(render_state) {}
    void set(int w, int h, int _x, int _y, int id, Render_State *render_state)
    {
        width = w;
        height = h;
        x = _x;
        y = _y;
        ID = id;
        rs = render_state;
    }
    void draw(HDC hdc)
    {
        RECT rec;
        SetRect(&rec, x, y, x + width, y + height);
        HBRUSH hBrush = CreateSolidBrush(getBGColor(value));
        FillRect(hdc, &rec, hBrush);
        DeleteObject(hBrush);
    }

    void setValue(int val)
    {
        value = val;
    }
    COLORREF getBGColor(int value)
    {
        COLORREF color = RGB(0, 0, 0);
        switch (value)
        {
        case 0:
            return RGB(204, 196, 136);
        case 2:
        case 4:
            return RGB(170, 170, 170);
        case 8:
        case 16:
            return RGB(170, 170, 70);
        case 32:
        case 64:
            return RGB(70, 170, 70);
        case 128:
        case 256:
            return RGB(70, 170, 170);
        case 512:
        case 1024:
            return RGB(70, 70, 170);
        case 1024 * 2:
        case 1024 * 4:
            return RGB(170, 70, 170);
        case 1024 * 8:
        case 1024 * 16:
            return RGB(170, 70, 70);
        case 1024 * 32:
        case 1024 * 64:
            return RGB(0, 0, 0);
        }
        return color;
    }
    int getFontSize(int value)
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

    void drawText(int val, HDC hdc)
    {
        int fsize = getFontSize(value);
        HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
        HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(0, 0, 0x48));
        SetBkMode(hdc, TRANSPARENT);
        RECT rec;
        SetRect(&rec, x, y, x+width, y+height);
        
        wchar_t buffer[64];
        wsprintfW(buffer, L"%d", val);
        DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        
        DeleteObject(SelectObject(hdc, hTmp));

    }
    void drawText(HDC hdc)
    {
        if (value != C2048::defaultValue)
        {
            drawText(value, hdc);
        }
    }
};

std::vector<block> initBoard(int block_count, Render_State *render_state)
{
    std::vector<block> blocks;
    int border_size = 10;
    int border_size_total = border_size * (block_count + 1);
    int block_width = (render_state->width - border_size_total - SIDE_GAP) / block_count;   // leave spave on sides
    int block_height = (render_state->height - border_size_total - TOP_GAP) / block_count;  // space on top for score

    for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            block b;
            int bx = (j + 1)*border_size + j * block_width + SIDE_GAP/2;
            int by = (i + 1)*border_size + i * block_height + TOP_GAP;
            int ID = j * block_count + i;
            b.set(block_width, block_height, bx, by, ID, render_state);
            blocks.push_back(b);
        }
    }
    return blocks;
}
void setBlockValues(std::vector<block> &blocks, C2048 &board, HDC hdc)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        int value = board.getVal(i);
        blocks[i].setValue(value);
    }
}
void drawScore(int score, Render_State &rs, HDC hdc)
{
    int fsize = 40;
    HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
    HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
    SetBkMode(hdc, TRANSPARENT);
    RECT rec;
    SetRect(&rec, 0, 0, rs.width, TOP_GAP);

    wchar_t buffer[64];
    wsprintfW(buffer, L"Score: %d", score);
    DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    DeleteObject(SelectObject(hdc, hTmp));

}

void drawWarning(std::wstring text, Render_State &rs, HDC hdc)
{
    int fsize = 40;
    HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
    HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(0xff, 0, 0));
    SetBkMode(hdc, TRANSPARENT);
    RECT rec;
    SetRect(&rec, rs.width/2, 0, rs.width, TOP_GAP);

    wchar_t buffer[64];
    wsprintfW(buffer, L"%s", text.c_str());
    DrawText(hdc, buffer, lstrlenW(buffer), &rec, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    DeleteObject(SelectObject(hdc, hTmp));

}

void drawBlocks(std::vector<block> &blocks, C2048 &board, Render_State &rs, HDC hdc)
{
    // fill black background
    RECT rec;
    SetRect(&rec, 0, 0, rs.width, rs.height+TOP_GAP);
    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
    FillRect(hdc, &rec, hBrush);
    DeleteObject(hBrush);

    if (board.addNewValue() == 0)
    {
        drawWarning(L"FULL", render_state, hdc);
    }
    // load board values into corresponding blocks
    setBlockValues(blocks, board, hdc);

    // draw background tiles for all blocks
    for (auto &b : blocks)
    {
        b.draw(hdc);
    }
    // draw text on all blocks
    for (auto &b : blocks)
    {
        b.drawText(hdc);
    }
    int score = board.getScore();
    drawScore(score, rs, hdc);

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
        drawWarning(L"FULL", render_state, hdc);
    }
    std::vector<block> blocks = initBoard(size, &render_state);
    for (auto &b : blocks)
    {
        b.draw(hdc);
    }

    drawBlocks(blocks, board, render_state, hdc);

    while (running) {
        // Input
        MSG message;


        while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) 
        {

            switch (message.message) 
            {
            case WM_KEYDOWN:
                break;
            case WM_KEYUP:
            {
                UINT32 vk_code = (UINT32)message.wParam;

                switch (vk_code)
                {
                case VK_UP:
                case 'W':
                    board.toUp();
                    button_pressed = BUTTON_UP;
                    break;
                case VK_DOWN:
                case 'S':
                    board.toDown();
                    button_pressed = BUTTON_DOWN;
                    break;
                case VK_LEFT:
                case 'A':
                    board.toLeft();
                    button_pressed = BUTTON_LEFT;
                    break;
                case VK_RIGHT:
                case 'D':
                    board.toRight();
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

        if (button_pressed != -1)
        {
            button_pressed = -1;
            drawBlocks(blocks, board, render_state, hdc);
        }

        LARGE_INTEGER frame_end_time;
        QueryPerformanceCounter(&frame_end_time);
        delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
        frame_begin_time = frame_end_time;
    }

}
