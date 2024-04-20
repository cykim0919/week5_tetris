#ifndef GAME_H
#define GAME_H
#include "tetromino.h"
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "console/console.h"
#include <chrono>
#include <cstdio>
#include <deque>
#include <vector>
#include <iomanip>
#include <sstream>
#define BOARD_WIDTH 11
#define BOARD_HEIGHT 20
#define LINES 40

#define DROP_DELAY 60

class Game
{
private:
	bool m_isPlay;

	int score_;
	bool gameOver_;
	bool checkWin();
	Tetromino* currentBlock;
	int currentBlockX;
	int currentBlockY;
	int linesRemaining=LINES;
	std::chrono::steady_clock::time_point startTime_; // 게임 시작 시간
	std::string formatPlayTime(); // 게임 플레이 타임을 포맷하는 함수

	int dropTimer = DROP_DELAY;

	// 게임 판을 나타내는 배열
	// board[x][y]가 true 인 경우 x, y 위치에 고정된 블록이 존재하는 것을 의미한다
	bool board_[BOARD_WIDTH][BOARD_HEIGHT];
	void init();
	void spawnBlock();

	void checkLines();
	void drawShadow();
	void fixBlock();

	bool isValidPosition(int x, int y, Tetromino* block);

	void Input();

	bool CheckBlock();
public:
	Game();
	// 게임의 한 프레임을 처리한다.
	void update();

	// 게임 화면을 그린다.
	void draw();
	// 게임 루프가 종료되어야 하는지 여부를 반환한다.
	bool shouldExit();

};
#endif