#include "Tetromino.h"

// Tetromino 정적 멤버 변수를 초기화
Tetromino Tetromino::I("I", 4, "XXXXOOOOXXXXXXXX");
Tetromino Tetromino::O("O", 2, "OOOO");
Tetromino Tetromino::T("T", 3, "XOXOOOXXX");
Tetromino Tetromino::S("S", 3, "XOOOOXXXX");
Tetromino Tetromino::Z("Z", 3, "OOXXOOXXX");
Tetromino Tetromino::J("J", 3, "OXXOOOXXX");
Tetromino Tetromino::L("L", 3, "XXOOOOXXX");

// Tetromino 클래스의 생성자
Tetromino::Tetromino(std::string name, int size, std::string shape) : name_(name), size_(size), original_(this)
{
    // 주어진 모양 문자열에서 블록의 위치를 파싱하여 shape_ 배열에 저장합니다.
    for (int i = 0; i < size; ++i) 
    {
        for (int j = 0; j < size; ++j) 
        {
            shape_[i][j] = (shape[i * size + j] == 'X'); // 'X'는 블록이 있는 곳, 'O'는 빈 곳
        }
    }
}

Tetromino Tetromino::rotatedCW()
{
    std::string newShape;

    // 시계 방향으로 회전한 모양을 생성
    for (int j = 0; j < size_; j++)
    {
        for (int i = size_ - 1; i >= 0; i--) 
        {
            newShape += shape_[i][j] ? 'X' : 'O';
        }
    }

    return Tetromino(name_, size_, newShape);
}

Tetromino Tetromino::rotatedCCW()
{
    std::string newShape;

    // 반시계 방향으로 회전한 모양을 생성
    for (int j = size_ - 1; j >= 0; j--)
    {
        for (int i = 0; i < size_; i++) 
        {
            newShape += shape_[i][j] ? 'X' : 'O';
        }
    }

    return Tetromino(name_, size_, newShape);
}

// Tetromino 블록을 지정된 위치에 그리기
void Tetromino::drawAt(std::string s, int x, int y)
{
    // 블록의 모양을 순회하면서 각 위치에 블록 또는 그림자 문자열을 출력
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            std::cout << (shape_[i][j] ? BLOCK_STRING : SHADOW_STRING); // 블록이 있는 위치는 BLOCK_STRING을 출력, 그렇지 않은 위치는 SHADOW_STRING을 출력
        }

        std::cout << std::endl;
    }
}
