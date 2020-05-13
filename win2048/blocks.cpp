#include "blocks.h"

Blocks::Blocks(int _count, const Render_State &_rs, HDC _hdc/*, std::mutex &gmutex*/) 
    :count(_count), rs(_rs), hdc(_hdc)/*, mutex(gmutex)*/
{
    //std::vector<block> blocks;
    setupBlocks(count, rs);
}
Blocks::Blocks(const Blocks &other)
    : count(other.count), rs(other.rs), hdc(other.hdc), /*mutex(other.mutex),*/ blocks(other.blocks)
{}
Blocks::Blocks(const Blocks &&other)
    : count(other.count), rs(other.rs), hdc(other.hdc), /*mutex(other.mutex),*/ blocks(other.blocks)
{}
Blocks &Blocks::operator=(const Blocks &other)
{
    if (this != &other)
    {
        count = other.count;
        rs = other.rs;
        hdc = other.hdc;
        blocks.clear();
        for (const auto &b : other.blocks)
        {
            blocks.emplace_back(b);
        }

    }
    return *this;
}
Blocks &Blocks::operator=(const Blocks &&other)
{
    if (this != &other)
    {
        count = other.count;
        rs = other.rs;
        hdc = other.hdc;
        blocks.clear();
        for (const auto &b : other.blocks)
        {
            blocks.emplace_back(b);
        }

    }
    return *this;
}

void Blocks::setupBlocks(int _count, const Render_State &_rs)
{
    blocks.reserve(count*count);
    int border_size_total = BORDER_SIZE * (count + 1);
    int block_width = (rs.width - border_size_total - SIDE_GAP) / count;   // leave spave on sides
    int block_height = (rs.height - border_size_total - TOP_GAP) / count;  // space on top for score

    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < count; j++)
        {
            int bx = (j + 1)*BORDER_SIZE + j * block_width + SIDE_GAP / 2;
            int by = (i + 1)*BORDER_SIZE + i * block_height + TOP_GAP;
            int ID = j * count + i;
            blocks.emplace_back(block_width, block_height, bx, by, ID, &rs);
        }
    }

}

void Blocks::setBlockValues(const C2048 &board)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        int value = board.getVal(i);
        blocks[i].setValue(value);
    }
}
void Blocks::drawValues()
{
    // draw tiles & text on all blocks
    for (auto &b : blocks)
    {
        //const std::lock_guard<std::mutex> lock(mutex);
        b.drawValue(hdc);
    }

}
void Blocks::drawBlocks(const C2048 &board)
{
    // load board values into corresponding blocks & draw them
    setBlockValues(board);
    drawValues();
}
