#include "game.h"
#include <cstring>

// Game 클래스의 생성자
Game::Game() : score_(0), gameOver_(false)
{
    // 모든 칸을 0으로 설정
    std::memset(board_, 0, sizeof(board_));
    startTime_ = std::chrono::steady_clock::now(); // 게임 시작 시간 초기화
}
std::string Game::formatPlayTime()//플레이타임 계산
{
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime_);

    int minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60;
    int seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60;
    int milliseconds = duration.count() % 1000;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"//분
        << std::setfill('0') << std::setw(2) << seconds << "."//초
        << std::setfill('0') << std::setw(2) << milliseconds / 10;

    return oss.str();
}

// 게임 상태를 업데이트하는 함수
void Game::update()
{
    // 블록을 한 칸 아래로 이동할 수 있는지 검사
    if (isValidPosition(currentBlockX, currentBlockY + 1, currentBlock))
    {
        currentBlockY++; // 가능하다면 블록을 한 칸 아래로 이동
    }
    else
    {
        fixBlock();      // 블록을 보드에 고정
        spawnBlock();    // 새로운 블록을 생성
        checkLines();    // 완성된 라인이 있는지 검사
    }
}

// 게임 보드와 현재 블록을 그리는 함수
void Game::draw()
{
    console::clear();

    // 보드를 순회하면서 블록이 있는 위치를 그림
    for (int x = 0; x < BOARD_WIDTH - 1; x++)
    {
        for (int y = 0; y < BOARD_HEIGHT - 1; y++)
        {
            if (board_[x][y]) // 블록이 있는 위치인 경우
            {
                console::draw(x, y, BLOCK_STRING); // 해당 위치에 블록을 그림
            }
        }
    }

    currentBlock->drawAt(BLOCK_STRING, currentBlockX, currentBlockY); // 현재 블록을 그림
    console::log("Play Time: " + formatPlayTime());
}

// 게임 종료 조건을 반환하는 함수
bool Game::shouldExit()
{
    return gameOver_;
}

// 블록을 보드에 고정하는 함수
void Game::fixBlock()
{
    for (int x = 0; x < currentBlock->size(); x++)
    {
        for (int y = 0; y < currentBlock->size(); y++)
        {
            if (currentBlock->check(x, y)) // 블록이 있는 경우
            {
                board_[currentBlockX + x][currentBlockY + y] = true; // 해당 위치에 블록을 고정
            }
        }
    }
}

// 블록이 보드를 벗어나지 않는지 검사하는 함수
bool Game::isValidPosition(int x, int y, Tetromino* block)
{
    for (int i = 0; i < block->size(); i++)
    {
        for (int j = 0; j < block->size(); j++)
        {
            if (block->check(i, j)) // 블록이 있는 경우
            {
                // 블록이 보드를 벗어나거나 다른 블록과 겹치는 경우
                if (x + i < 0 || x + i >= BOARD_WIDTH || y + j < 0 || y + j >= BOARD_HEIGHT || board_[x + i][y + j])
                {
                    return false; // 유효하지 않은 위치
                }
            }
        }
    }
    return true; // 유효한 위치
}

// 게임 초기화 함수
void Game::init()
{
    spawnBlock();
}

// 무작위 블록을 생성하는 함수
void Game::spawnBlock()
{
    int blockType = rand() % 7; // 0부터 6까지의 랜덤한 숫자를 얻고,
    Tetromino* blocks[7] = { &Tetromino::I, &Tetromino::O, &Tetromino::T, &Tetromino::S, &Tetromino::Z, &Tetromino::J, &Tetromino::L };

    currentBlock = blocks[blockType]; // 랜덤하게 선택된 블록을 현재 블록으로 설정
    currentBlockX = BOARD_WIDTH / 2 - currentBlock->size() / 2; // 블록의 시작 위치를 계산
    currentBlockY = 0;

    // 시작 위치가 유효하지 않으면 게임 오버
    if (!isValidPosition(currentBlockX, currentBlockY, currentBlock))
    {
        gameOver_ = true;
        console::log("Game Over!");
    }
}
// 게임이 승리 조건을 만족하는지 검사하는 함수
bool Game::checkWin()
{
    int linesCleared = 0; // 지워진 라인 수를 저장할 변수

    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool lineComplete = true; // 현재 라인이 완전히 채워졌는지 여부를 저장할 변수

        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (!board_[x][y]) // 라인에 빈 칸이 있는 경우
            {
                lineComplete = false;
                break;
            }
        }

        if (lineComplete) // 라인이 완전히 채워진 경우
        {
            linesCleared++; // 지워진 라인 수를 증가
        }
    }

    return linesCleared >= LINES; // 지워진 라인 수가 40 이상인 경우 승리
}

// 완성된 라인이 있는지 검사하고 처리하는 함수
void Game::checkLines()
{
    int linesCleared = 0; // 제거된 라인 수
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        bool lineComplete = true; // 현재 라인이 완성되었는지 여부를 저장할 변수
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (!board_[x][y]) // 라인에 빈 공간이 있는 경우
            {
                lineComplete = false;
                break;
            }
        }

        if (lineComplete) // 현재 라인이 완성된 경우
        {
            // 위의 라인들을 한 칸씩 아래로 내림
            for (int yy = y; yy > 0; yy--)
            {
                for (int xx = 0; xx < BOARD_WIDTH; xx++)
                {
                    board_[xx][yy] = board_[xx][yy - 1];
                }
            }

            // 맨 위의 라인을 초기화
            memset(board_[0], 0, BOARD_WIDTH * sizeof(bool));
            linesCleared++; // 제거된 라인 수를 증가
        }
    }

    if (linesCleared > 0) // 라인이 제거된 경우
    {
        // 남은 줄 수를 출력
        int linesRemaining = LINES - linesCleared;
        console::log(std::to_string(linesRemaining) + " lines left");

        // 게임 승리 조건 검사
        if (checkWin())
        {
            console::log("You Win!");
            gameOver_ = true;
        }
    }
}
