// win2048.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <vector>
#include "win2048.h"
#include "C2048.h"

static bool running = true;

static Render_State render_state;


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

        int size = render_state.width * render_state.height * sizeof(unsigned int);

        //if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
        //render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        //render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
        //render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
        //render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
        //render_state.bitmap_info.bmiHeader.biPlanes = 1;
        //render_state.bitmap_info.bmiHeader.biBitCount = 32;
        //render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

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
    //int modified = 0;
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
        //if (value != 0)
        {
            RECT rec;
            SetRect(&rec, x, y, x + width, y + height);
            HBRUSH hBrush = CreateSolidBrush(getBGColor(value));
            FillRect(hdc, &rec, hBrush);
            DeleteObject(hBrush);

            //COLORREF color = getBGColor(value);    // RGB(204, 196, 136);    //
            //draw_rect_in_pixels(x, y, x + width, y + height, color, rs);

        }
        //drawText();
    }
    //void draw(COLORREF color)
    //{
    //    draw_rect_in_pixels(x, y, x + width, y + height, color, rs);
    //}
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
            color = RGB(204, 196, 136);
            break;
        case 2:
        case 4:
            color = RGB(170, 170, 170);
            break;
        case 8:
        case 16:
            color = RGB(170, 170, 70);
            break;
        case 32:
        case 64:
            color = RGB(70, 170, 70);
            break;
        case 128:
        case 256:
            color = RGB(70, 170, 170);
            break;
        case 512:
        case 1024:
            color = RGB(70, 70, 170);
            break;
        case 1024 * 2:
        case 1024 * 4:
            color = RGB(170, 70, 170);
            break;
        case 1024 * 8:
        case 1024 * 16:
            color = RGB(170, 70, 70);
            break;
        case 1024 * 32:
        case 1024 * 64:
            color = RGB(0, 0, 0);
            break;
        }
        return color;
    }
    int getFontSize(int value)
    {
        int count = C2048::getDigitCount(value);
        switch (count)
        {
        case 1:
            return 60;
        case 2:
            return 50;
        case 3:
            return 40;
        case 4:
            return 30;
        case 5:
        default:
            return 20;
        }
    }
    void drawText(HDC hdc, int val)
    {
        int fsize = getFontSize(value);
        HFONT hFont = CreateFont(fsize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 2, 0, L"SYSTEM_FIXED_FONT");
        HFONT hTmp = (HFONT)SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(0, 0, 0x88));
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
            drawText(hdc, value);
        }
    }
};

std::vector<block> initBoard(int block_count, Render_State *render_state)
{
    std::vector<block> blocks;
    int border_size = 10;
    int border_size_total = border_size * (block_count + 1);
    int block_width = (render_state->width - border_size_total) / block_count;
    int block_height = (render_state->height - border_size_total) / block_count;

    for (int i = 0; i < block_count; i++)
    {
        for (int j = 0; j < block_count; j++)
        {
            block b;
            int bx = (j + 1)*border_size + j * block_width;
            int by = (i + 1)*border_size + i * block_height;
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
        //if (value != 0)
        //{
        //    blocks[i].drawText(hdc);
        //}
    }
}

void printText(std::vector<block> &blocks, C2048 &board, Render_State &rs, HDC hdc)
{
    //initBoard(8, &render_state);
    if (board.addNewValue() == 0)
    {
    }
    setBlockValues(blocks, board, hdc);
    for (auto &b : blocks)
    {
        b.draw(hdc);
    }
    // Render
    //StretchDIBits(hdc, 0, 0, rs.width, rs.height, 0, 0, rs.width, rs.height, rs.memory, &rs.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    //static int index = 0;
    for (auto &b : blocks)
    {
        b.drawText(hdc);
    }
    //for (size_t i = 0; i < blocks.size(); i++)
    //{
    //    int value = board.getVal(i);
    //    if (value != board.defaultValue)
    //    {
    //        blocks[i].drawText(hdc);
    //    }

    //}
    //for (auto &b : blocks)
    //{
    //    int value = board.getVal(index);
    //    index++;
    //    if (value != board.defaultValue)
    //    {
    //        b.drawText(hdc, value);
    //    }
    //    //b.draw();
    //}

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
    C2048 board(size);
    if (board.addNewValue() == 0)
    {
    }
    std::vector<block> blocks = initBoard(size, &render_state);
    for (auto &b : blocks)
    {
        b.draw(hdc);
    }

    // Render
    //StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    printText(blocks, board, render_state, hdc);

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

        // Simulate
        //simulate_game(&input, delta_time, &render_state);
        if (button_pressed != -1)
        {
            button_pressed = -1;
            printText(blocks, board, render_state, hdc);
        }
        //Sleep(1000);
        //for (int i = 0; i < 64; i++)
        //{
        //    blocks[i].drawText(hdc, 2);
        //    blocks[i].draw();
        //}



        LARGE_INTEGER frame_end_time;
        QueryPerformanceCounter(&frame_end_time);
        delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
        frame_begin_time = frame_end_time;
    }

}
