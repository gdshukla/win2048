#pragma once
#include <iostream>
#include <vector>

class C2048
{
    std::vector<std::vector<int>> grid;
    size_t size;
    unsigned long score;

    int generateValue(int min, int max);
    int modify(int *first, int *second);
    std::string getOutString(int digitCount);
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();

public:
    static const int defaultValue = 0;
    C2048() :size(0), score(0){}
    C2048(size_t _size);
    bool addNewValue();
    void print();
    bool toLeft();
    bool toRight();
    bool toUp();
    bool toDown();
    unsigned long getScore() const;
    int getVal(size_t index) const;

};

