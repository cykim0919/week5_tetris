#include "game.h"
#include <cstring>
// Game 클래스의 생성자
Game::Game() : score_(0), gameOver_(false), linesRemaining(LINES)
{
    // 모든 칸을 0으로 설정
    std::memset(board_, false, sizeof(board_));
    startTime_ = std::chrono::steady_clock::now(); // 게임 시작 시간 초기화

    spawnBlock();
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
    Input();

    // 드롭 타이머 감소
    dropTimer -= 1; // 예시로 1 감소시킵니다. 이 값을 조절하여 드롭 속도를 변경할 수 있습니다.

    // 드롭 타이머가 0 이하일 때만 블록을 아래로 이동
    if (dropTimer <= 0 && currentBlock && isValidPosition(currentBlockX, currentBlockY + 1, currentBlock))
    {
        currentBlockY++; // 블록을 한 칸 아래로 이동
        dropTimer = DROP_DELAY; // 드롭 타이머 재설정
    }
    else if (!isValidPosition(currentBlockX, currentBlockY + 1, currentBlock))
    {
        fixBlock();      // 블록을 보드에 고정
        spawnBlock();    // 새로운 블록을 생성
        checkLines();    // 완성된 라인이 있는지 검사
    }   
}
void Game::drawShadow()
{
    // 그림자 블록을 그리기 위해 현재 블록의 위치를 계산
    int shadowY = currentBlockY;
    while (isValidPosition(currentBlockX, shadowY + 1, currentBlock))
    {
        shadowY++; // 그림자 블록의 Y 위치를 하나씩 증가시킴
    }

    // 쉐도우 블록을 그리기
    for (int i = 0; i < currentBlock->size(); i++)
    {
        for (int j = 0; j < currentBlock->size(); j++)
        {
            if (currentBlock->check(i, j))
            {
                console::draw(currentBlockX + i, shadowY + j, SHADOW_STRING);
            }
        }
    }

    // 원래 블록을 그리기
    for (int i = 0; i < currentBlock->size(); i++)
    {
        for (int j = 0; j < currentBlock->size(); j++)
        {
            if (currentBlock->check(i, j))
            {
                // 쉐도우와 겹치는 부분이면 원래 블록으로 그리기
                if (shadowY + j == currentBlockY + j) // Y 좌표 비교
                {
                    console::draw(currentBlockX + i, currentBlockY + j, BLOCK_STRING);
                }
            }
        }
    }
}

// 게임 보드와 현재 블록을 그리는 함수
void Game::draw()
{
    int minH = BOARD_HEIGHT;

    console::clear();
    console::drawBox(0, 0, 11, 20); // board 칸을 그린다
    console::drawBox(13, 0, 18, 5); // Next 칸을 그린다
    console::drawBox(19, 0, 24, 5); // Hold 칸을 그린다
    console::draw(14, 0, "Next");
    console::draw(20, 0, "Hold");
    

    for (int i = 0; i < currentBlock->size(); i++)
    {
        for (int j = 0; j < currentBlock->size(); j++)
        {
            if (currentBlock->check(i, j))
            {
                for (int k = currentBlockY + i; k < BOARD_HEIGHT; k++)
                {
                    if (!board_[currentBlockX + j][k])
                    {
                        if(minH > (k - currentBlockY - i)){
                            minH = k - currentBlockY - i;
                        }
                        else{
                            minH = minH;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < currentBlock->size(); i++)
    {
        for (int j = 0; j < currentBlock->size(); j++)
        {
            if (currentBlock->check(i, j)) 
            {
                currentBlock->drawAt(BLOCK_STRING, currentBlockX + i, currentBlockY + j);
            }
        }
    }

    for (int i = 0; i < BOARD_WIDTH; i++) 
    {
        for (int j = 0; j < BOARD_HEIGHT; j++) 
        {
            if (board_[i][j]) 
            {
                console::draw(i, j, BLOCK_STRING);
            }
        }
    }
    drawShadow();// 그림자 그리기
    int playTimeX = ((BOARD_WIDTH - 10) / 2)+2;
    int playTimeY = BOARD_HEIGHT+2;
    // 남은 라인수 출력
    console::draw(playTimeX - 2, playTimeY - 1, (std::to_string(linesRemaining) + " lines left"));
    // Play Time을 게임판 아래쪽 중앙에 출력
    
    console::draw(playTimeX, playTimeY, formatPlayTime());
}

// 게임 종료 조건을 반환하는 함수
bool Game::shouldExit()
{
    return gameOver_;
}

// 블록을 보드에 고정하는 함수
void Game::fixBlock()
{
    for (int i = 0; i < currentBlock->size(); i++)
    {
        for (int j = 0; j < currentBlock->size(); j++)
        {
            if (currentBlock->check(i, j))
            {
                int posX = currentBlockX + i;
                int posY = currentBlockY + j;

                if (posX >= 0 && posX < BOARD_WIDTH && posY >= 0 && posY < BOARD_HEIGHT)
                {
                    board_[posX][posY] = true;
                }
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
                if (x + i < 1 || x + i >= BOARD_WIDTH || y + j < 0 || y + j >= BOARD_HEIGHT || board_[x + i][y + j])
                {
                    return false; // 유효하지 않은 위치
                }
            }
        }
    }

    return true; // 유효한 위치
}

void Game::Input()
{
    if (console::key(console::K_LEFT))
    {
        if (currentBlock && isValidPosition(currentBlockX - 1, currentBlockY, currentBlock))
        {
            currentBlockX--; // 가능하다면 블록을 한 칸 아래로 이동
        }
    }

    else if (console::key(console::K_RIGHT))
    {
        if (currentBlock && isValidPosition(currentBlockX + 1, currentBlockY, currentBlock))
        {
            currentBlockX++; // 가능하다면 블록을 한 칸 아래로 이동
        }
    }

    else if (console::key(console::K_DOWN))
    {
        if (isValidPosition(currentBlockX, currentBlockY + 1, currentBlock))
        {
            currentBlockY++;
        }
    }
    else if (console::key(console::K_UP)) // 윗방향키를 눌렀을 때
    {
        // 현재 블록을 맨 아래까지 빠르게 이동시킴
        while (isValidPosition(currentBlockX, currentBlockY + 1, currentBlock))
        {
            currentBlockY++;
        }
    }
    else if (console::key(console::K_X))
    {
        currentBlock->rotatedCW();
    }

    else if (console::key(console::K_Z))
    {
        currentBlock->rotatedCCW();
    }
    else if (console::key(console::K_ESC)) // ESC 키를 눌렀을 때
    {
        gameOver_ = true; // 게임 오버 상태로 설정
    }
}

bool Game::CheckBlock()
{
    return false;
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
        int gameOverX = (BOARD_WIDTH - 9) / 2;
        int gameOverY = (BOARD_HEIGHT - 1) / 2;
        console::draw(gameOverX, gameOverY, "You Lost");
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

    // 남은 라인 수를 계산
    linesRemaining = linesRemaining - linesCleared;

    // 남은 라인 수가 0 이하일 경우 승리
    return linesRemaining <= 0;
}

// 완성된 라인이 있는지 검사하고 처리하는 함수
void Game::checkLines()
{
    int linesCleared = 0; // 제거된 라인 수
    for (int y = BOARD_HEIGHT -1; y >= 0; y--)
    {
        bool lineComplete = true; // 현재 라인이 완성되었는지 여부를 저장할 변수
        for (int x = 1; x < BOARD_WIDTH; x++)
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
            linesRemaining --;
            y++;
            linesCleared++;
        }
    }

    if (linesCleared > 0) // 라인이 제거된 경우
    {
        // 게임 승리 조건 검사
        if (checkWin())
        {
            int WinX = (BOARD_WIDTH - 9) / 2;
            int WinY = (BOARD_HEIGHT - 1) / 2;
            console::draw(WinX, WinY, "You Win");
            gameOver_ = true;
        }
    }
}