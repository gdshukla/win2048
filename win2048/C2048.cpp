#include <random>
#include <string>
#include "C2048.h"
#include "utility.h"

C2048::C2048(size_t _size):size(_size), score(0)
{
    grid.resize(size);
    for (auto &i : grid)
    {
        i.resize(size);
        for (auto &j : i)
        {
            j = defaultValue;     // all empty
        }
    }
}



int C2048::generateValue(int min, int max)
{
    int val = rand() % max;
    val++;
    return val * 2;
}

bool C2048::addNewValue()
{
    int val = rand() % 2;       // valid values are 2 & 4
    size_t x = 0;
    size_t y = 0;
    unsigned long long count = 0;
    unsigned long long maxCount = getTickCount() + 50;

    do
    {
        count++;
        x = rand() % size;
        y = rand() % size;
        //if (count > 1000)
        //    return 0;
        if (getTickCount() > maxCount)
        {
            return 0;       // enough tries, failed to find empty spot
        }
    } while (grid[y][x] != defaultValue);
    if (grid[y][x] == defaultValue)
    {
        grid[y][x] = generateValue(1, 2);
    }

    return 1;
}

std::string C2048::getOutString(int digitCount)
{
    std::string s;
    for (int i = 0; i < digitCount; i++)
    {
        s += " ";
    }
    return s;
}

void C2048::print()
{
    int digitCount = 1;
    for (auto &i : grid)
    {
        for (auto &j : i)
        {
            int val = getDigitCount(j);
            if (digitCount < val)
            {
                digitCount = val;
            }
        }
    }

    for (auto &i : grid)
    {
        std::cout << "| ";
        for (auto &j : i)
        {
            if (j == defaultValue)
            {
                // prepare output string
                std::string s = getOutString(digitCount) + " | ";
                std::cout << s;
            }
            else
            {
            int count = getDigitCount(j);
            int spaceCount = digitCount - count;
            std::string s = getOutString(spaceCount);
            std::cout << j << s << " | ";
            }
        }
        std::cout << "\n";
    }

}

int C2048::getVal(size_t index) const 
{
    if (index > size*size)
        return defaultValue;
    size_t y = index / size;
    size_t x = index % size;
    int value = grid[y][x];

    return value;
}

int C2048::modify(int *first, int *second)
{
    if (*first != defaultValue && *second != defaultValue && *first == *second)
    {
        *first += *second;
        score += *second;
        *second = defaultValue;
        return 1;
    }
    return 0;
}
unsigned long C2048::getScore() const
{
    return score;
}
void C2048::moveLeft()
{
    for (size_t i = 0; i < size; ++i) 
    {
        size_t pos = 0;
        for (size_t j = 0; j < size; ++j) 
        {
            if (grid[i][pos] == 0)
            {
                for (size_t k = pos; k < size-1; ++k) 
                {
                    grid[i][k] = grid[i][k + 1];
                }
                grid[i][size - 1] = 0;
            }
            else 
            {
                ++pos;
            }
        }
    }

}
bool C2048::toLeft()
{
    bool status = false;
    moveLeft();
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size -1; j++)
        {
            // if there is some modification, dont iterate over it again
            if (modify(&grid[i][j], &grid[i][j+1]) == 1 && j < size-2)
            {
                status = true;
                //j++;
            }
        }
    }
    moveLeft();
    return status;
}

void C2048::moveRight()
{
    for (size_t i = 0; i < size; ++i)
    {
        size_t pos = size-1;
        for (size_t j = 0; j < size; ++j)
        {
            if (grid[i][pos] == 0)
            {
                for (size_t k = pos; k > 0; --k)
                {
                    grid[i][k] = grid[i][k - 1];
                }
                grid[i][0] = 0;
            }
            else 
            {
                --pos;
            }
        }
    }
}
bool C2048::toRight()
{
    bool status = false;
    moveRight();
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = size -1; j > 0; j--)
        {
            if (modify(&grid[i][j], &grid[i][j -1]) == 1 && j > 1)
            {
                status = true;
                //j--;
            }
        }
    }
    moveRight();
    return status;
}

void C2048::moveUp()
{
    for (size_t j = 0; j < size; ++j)
    {
        size_t pos = 0;
        for (size_t i = 0; i < size; ++i)
        {
            if (grid[pos][j] == 0)
            {
                for (size_t k = pos; k < size-1; ++k)
                {
                    grid[k][j] = grid[k + 1][j];
                }
                grid[size-1][j] = 0;
            }
            else 
            {
                ++pos;
            }
        }
    }
}

bool C2048::toUp()
{
    bool status = false;
    moveUp();
    int incrementI = 0;
    for (size_t i = 0; i < size -1; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (modify(&grid[i][j], &grid[i + 1][j]) == 1 && i != size - 1)
            {
                status = true;
                //incrementI = 1;
            }
        }
        if (incrementI == 1)
        {
            i++; incrementI = 0;
        }
    }
    moveUp();
    return status;
}

void C2048::moveDown()
{
    for (size_t j = 0; j < size; ++j)
    {
        size_t pos = size-1;
        for (size_t i = 0; i < size; ++i)
        {
            if (grid[pos][j] == 0)
            {
                for (size_t k = pos; k > 0; --k)
                {
                    grid[k][j] = grid[k - 1][j];
                }
                grid[0][j] = 0;
            }
            else 
            {
                --pos;
            }
        }
    }
}
bool C2048::toDown()
{
    bool status = false;
    moveDown();
    int decrementI = 0;
    for (size_t i = size -1; i > 0; i--)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (modify(&grid[i][j], &grid[i - 1][j]) == 1 && i > 1)
            {
                status = true;
                //decrementI = 1;
            }
        }
        if (decrementI == 1)
        {
            i--;
            decrementI = 0;
        }
    }
    moveDown();
    return status;
}
