#include <random>
#include <string>
#include "C2048.h"
#include "utility.h"

C2048::C2048(size_t _size):size(_size), score(0)
{
    v.resize(size);
    for (auto &i : v)
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

int C2048::addNewValue()
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
    } while (v[y][x] != defaultValue);
    if (v[y][x] == defaultValue)
    {
        v[y][x] = generateValue(1, 2);
    }

    return 1;
}

int C2048::getDigitCount(int value)
{
    int digitCount = 1;
    while (value > 9)
    {
        digitCount++;
        value /= 10;
    }
    return digitCount;
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
    for (auto &i : v)
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

    for (auto &i : v)
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
    int value = v[y][x];

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
            if (v[i][pos] == 0) 
            {
                for (size_t k = pos; k < size-1; ++k) 
                {
                    v[i][k] = v[i][k + 1];
                }
                v[i][size - 1] = 0;
            }
            else 
            {
                ++pos;
            }
        }
    }

}
void C2048::toLeft()
{
    moveLeft();
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = size - 1; j > 0; j--)
        {
            // if there is some modification, dont iterate over it again
            if (modify(&v[i][j - 1], &v[i][j]) == 1 && j != 1)
            {
                j--;
            }
        }
    }
    moveLeft();
}

void C2048::moveRight()
{
    for (size_t i = 0; i < size; ++i)
    {
        size_t pos = size-1;
        for (size_t j = 0; j < size; ++j)
        {
            if (v[i][pos] == 0) 
            {
                for (size_t k = pos; k > 0; --k)
                {
                    v[i][k] = v[i][k - 1];
                }
                v[i][0] = 0;
            }
            else 
            {
                --pos;
            }
        }
    }
}
void C2048::toRight()
{
    moveRight();
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 1; j < size; j++)
        {
            if (modify(&v[i][j], &v[i][j - 1]) == 1 && j != size - 1)
            {
                j++;
            }
        }
    }
    moveRight();
}

void C2048::moveUp()
{
    for (size_t j = 0; j < size; ++j)
    {
        size_t pos = 0;
        for (size_t i = 0; i < size; ++i)
        {
            if (v[pos][j] == 0) 
            {
                for (size_t k = pos; k < size-1; ++k)
                {
                    v[k][j] = v[k + 1][j];
                }
                v[size-1][j] = 0;
            }
            else 
            {
                ++pos;
            }
        }
    }
}

void C2048::toUp()
{
    moveUp();
    for (size_t i = size - 1; i > 0; i--)
    {
        for (size_t j = 0; j < size; j++)
        {
            modify(&v[i -1][j], &v[i][j]);
        }
    }
    moveUp();
}

void C2048::moveDown()
{
    for (size_t j = 0; j < size; ++j)
    {
        size_t pos = size-1;
        for (size_t i = 0; i < size; ++i)
        {
            if (v[pos][j] == 0) 
            {
                for (size_t k = pos; k > 0; --k)
                {
                    v[k][j] = v[k - 1][j];
                }
                v[0][j] = 0;
            }
            else 
            {
                --pos;
            }
        }
    }
}
void C2048::toDown()
{
    moveDown();
    for (size_t i = 1; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            modify(&v[i][j], &v[i - 1][j]);
        }
    }
    moveDown();
}
