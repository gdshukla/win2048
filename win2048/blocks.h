#pragma once
#include <Windows.h>
#include <vector>
#include "win2048.h"
#include "block.h"
#include "C2048.h"
#include <mutex>

class Blocks
{
    int count;
    /*const*/ Render_State rs;
    HDC hdc;
    //std::mutex &mutex;

    std::vector<block> blocks;
    void setBlockValues(const C2048 &board);
    void drawValues();

public:
    Blocks(int _count, const Render_State &_rs, HDC _hdc/*, std::mutex &gmutex*/);
    Blocks(const Blocks &other);
    Blocks(const Blocks &&other);
    Blocks &operator=(const Blocks &other);
    Blocks &operator=(const Blocks &&other);
    void drawBlocks(const C2048 &board);
    void setupBlocks(int _count, const Render_State &_rs);
};

