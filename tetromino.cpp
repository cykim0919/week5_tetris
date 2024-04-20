#include "Tetromino.h"
#include "console/console.h"

// Tetromino 정적 멤버 변수를 초기화
Tetromino Tetromino::I("I", 4, "XXXXOOOOXXXXXXXX");
Tetromino Tetromino::O("O", 2, "OOOO");
Tetromino Tetromino::T("T", 3, "XOXOOOXXX");
Tetromino Tetromino::S("S", 3, "XOOOOXXXX");
Tetromino Tetromino::Z("Z", 3, "OOXXOOXXX");
Tetromino Tetromino::J("J", 3, "OXXOOOXXX");
Tetromino Tetromino::L("L", 3, "XXOOOOXXX");

// Tetromino 클래스의 생성자
Tetromino::Tetromino(std::string name, int size, std::string shape) : name_(name), size_(size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            shape_[i][j] = false;
        }
    }

    // 주어진 모양 문자열에서 블록의 위치를 파싱하여 shape_ 배열에 저장합니다.
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            int idx = i * size + j;

            if (shape[idx] == 'O')
            {
                shape_[i][j] = true;
            }

            else if (shape[idx] == 'X')
            {
                shape_[i][j] = false;
            }
        }
    }

    original_ = this;
}

Tetromino Tetromino::rotatedCW()
{
    bool newShape[MAX_SIZE][MAX_SIZE];

    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            newShape[j][size_ - 1 - i] = shape_[i][j];
        }
    }

    for (int i = 0; i < size_; i++) 
    {
        for (int j = 0; j < size_; j++) 
        {
            shape_[i][j] = newShape[i][j];
        }
    }

    return *this;
}

Tetromino Tetromino::rotatedCCW()
{
    bool newShape[MAX_SIZE][MAX_SIZE];

    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            newShape[size_ - 1 - j][i] = shape_[i][j];
        }
    }

    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            shape_[i][j] = newShape[i][j];
        }
    }

    return *this;
}


// Tetromino 블록을 지정된 위치에 그리기
void Tetromino::drawAt(std::string s, int x, int y)
{
    console::draw(x, y, s);
}