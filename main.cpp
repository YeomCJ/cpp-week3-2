#include <iostream>
#include "console.h"
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

// 맵의 크기 (최소 4, 최대 20)
#define BOARD_SIZE 5
// 뱀이 움직이는 딜레이
#define MOVE_DELAY 15

// 문자열 상수 정의
#define WALL_VERTICAL_STRING u8"┃"
#define WALL_HORIZONTAL_STRING u8"━"
#define WALL_RIGHT_TOP_STRING u8"┓"
#define WALL_LEFT_TOP_STRING u8"┏"
#define WALL_RIGHT_BOTTOM_STRING u8"┛"
#define WALL_LEFT_BOTTOM_STRING u8"┗"
#define SNAKE_STRING u8"■"
#define SNAKE_BODY_STRING u8"■"
#define APPLE_STRING u8"●"
#define MAX_SNAKE_LENGTH ((BOARD_SIZE - 2) * (BOARD_SIZE - 2))

// 게임 상태 열거형
enum class GameState {
    PLAYING,
    GAMEOVER,
    WIN
};

// 뱀 구조체 정의
struct Snake {
    int x, y;
};

// 전역 변수 선언
Snake snake;
int score = 0;
bool appleExists = false;
int appleX, appleY;
GameState gameState = GameState::PLAYING;

Snake snakeBody[(BOARD_SIZE - 2) * (BOARD_SIZE - 2)];
int snakeLength = 1;

// 뱀의 이동 방향 열거형
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

Direction direction = Direction::RIGHT;

// 게임 초기화 함수
void initializeGame() {
    
    // 뱀의 초기 위치 설정 (맵의 중앙)
    snake.x = BOARD_SIZE / 2;
    snake.y = BOARD_SIZE / 2;

    // 초기 뱀의 방향 설정
    direction = Direction::RIGHT;

    snakeLength = 1;
    snakeBody[0] = snake;

    score = 0;

    // 초기화를 위한 화면 클리어
    console::clear();
}

//겹치는지 확인
bool isSnakeOnPosition(int x, int y) {
    // 뱀의 위치와 겹치는지 확인
    if (snake.x == x && snake.y == y)
        return true;
    for (int i = 0; i < snakeLength; ++i) {
        if (snakeBody[i].x == x && snakeBody[i].y == y)
            return true;
    }
    return false;
}
// 사과 생성 함수
void generateApple() {
    appleExists = true;

    // 빈 공간을 찾을 때까지 반복
    while (true) {
        // 빈 공간을 저장할 배열 초기화
        int emptySpaces[(BOARD_SIZE - 2) * (BOARD_SIZE - 2)][2];
        int emptySpaceCount = 0;

        // 빈 공간 찾기
        for (int x = 1; x < BOARD_SIZE - 1; ++x) {
            for (int y = 1; y < BOARD_SIZE - 1; ++y) {
                if (!isSnakeOnPosition(x, y)) {
                    emptySpaces[emptySpaceCount][0] = x;
                    emptySpaces[emptySpaceCount][1] = y;
                    emptySpaceCount++;
                }
            }
        }

        // 빈 공간이 있는 경우에만 사과 위치 선택
        if (emptySpaceCount > 0) {
            int randomIndex = rand() % emptySpaceCount;
            appleX = emptySpaces[randomIndex][0];
            appleY = emptySpaces[randomIndex][1];
            break; // 사과를 생성했으므로 반복문 종료
        } else {
            // 빈 공간이 없는 경우는 사과를 생성할 수 없으므로 appleExists를 false로 설정하고 종료
            appleExists = false;
            break;
        }
    }
}



void moveSnake() {

    // 이전 뱀 위치 저장
    Snake prevHead = snake;
    Snake newHead = snake;

    // 이동 방향에 따라 뱀의 머리 위치 조정
    switch (direction) {
        case Direction::UP:
            newHead.y--;
            break;
        case Direction::DOWN:
            newHead.y++;
            break;
        case Direction::LEFT:
            newHead.x--;
            break;
        case Direction::RIGHT:
            newHead.x++;
            break;
    }

    // 사과를 먹었을 경우
    if (newHead.x == appleX && newHead.y == appleY) {
        score += 10;
        generateApple();
        // 뱀의 길이 증가
        if (snakeLength < MAX_SNAKE_LENGTH)
            snakeLength++;
    }

    // 뱀의 머리 위치 갱신
    snake = newHead;

    // 뱀의 몸통 갱신
    for (int i = snakeLength - 1; i > 0; --i)
        snakeBody[i] = snakeBody[i - 1];
    snakeBody[0] = snake;
}

// 게임 오버 조건 확인 함수
bool isGameOver() {
    // 맵 밖으로 나갔을 경우
   if (snake.x <= 0 || snake.x >= BOARD_SIZE - 1 || snake.y <= 0 || snake.y >= BOARD_SIZE - 1)
        return true;

    // 뱀이 자신의 몸통과 겹친 경우
    for (int i = 1; i < snakeLength; ++i) {
        if (snake.x == snakeBody[i].x && snake.y == snakeBody[i].y) {
            return true;
        }
    }

    return false;
}

// 게임 승리 조건 확인 함수
bool isWin() {
    // 뱀의 길이가 맵 전체를 차지한 경우
    if (score == ((BOARD_SIZE - 2) * (BOARD_SIZE - 2)) * 10)
        return true;

    return false;
}


// 게임 오버 텍스트 출력 함수
void drawGameOver() {
    int x = (BOARD_SIZE/2);
    int y = BOARD_SIZE/2;
    console::draw(x - (strlen("Game Over!") /2), y, "Game Over!");
    console::draw(x - (strlen("Try again?(Enter)") /2),y+1,"Try again?(Enter)");
}

// 게임 승리 텍스트 출력 함수
void drawWin() {
    int x = (BOARD_SIZE/2);
    int y = BOARD_SIZE/2;
    console::draw(x - (strlen("You Win!") /2), y, "You Win!");
    console::draw(x - (strlen("Try again?(Enter)") /2),y+1,"Try again?(Enter)");
}


// 맵 테두리 출력 함수
void drawBoard() {
    
    // 테두리 상단
    console::draw(0, 0, WALL_LEFT_TOP_STRING);
    for (int i = 1; i < BOARD_SIZE - 1; ++i) {
        console::draw(i, 0, WALL_HORIZONTAL_STRING);
    }
    console::draw(BOARD_SIZE - 1, 0, WALL_RIGHT_TOP_STRING);

    // 테두리 하단
    console::draw(0, BOARD_SIZE - 1, WALL_LEFT_BOTTOM_STRING);
    for (int i = 1; i < BOARD_SIZE - 1; ++i) {
        console::draw(i, BOARD_SIZE - 1, WALL_HORIZONTAL_STRING);
    }
    console::draw(BOARD_SIZE - 1, BOARD_SIZE - 1, WALL_RIGHT_BOTTOM_STRING);

    // 테두리 좌우
    for (int i = 1; i < BOARD_SIZE - 1; ++i) {
        console::draw(0, i, WALL_VERTICAL_STRING);
        console::draw(BOARD_SIZE - 1, i, WALL_VERTICAL_STRING);
    }

    console::draw(appleX, appleY, APPLE_STRING);

    // 뱀의 몸통 출력
    for (int i = 1; i < snakeLength; ++i)
        console::draw(snakeBody[i].x, snakeBody[i].y, SNAKE_BODY_STRING);
    console::draw(snake.x, snake.y, SNAKE_STRING);

   
    string s = "Score: " + to_string(score);
    console::draw(BOARD_SIZE/2 - s.length()/2,BOARD_SIZE, s);
    
}

// 게임 상태에 따라 게임 화면 출력 함수
void drawGame() {
    console::clear();
    // 맵 테두리 출력
    drawBoard();

    console::draw(snake.x,snake.y, " ");
    // 뱀 이동
    moveSnake();
    console::draw(snake.x,snake.y, SNAKE_STRING);
}

void gameLoop() {

    while (true) {

        // 입력 받기 (방향키 입력 처리)
        if (console::key(console::Key::K_UP) && direction != Direction::DOWN)
            direction = Direction::UP;
        else if (console::key(console::Key::K_DOWN) && direction != Direction::UP)
            direction = Direction::DOWN;
        else if (console::key(console::Key::K_LEFT) && direction != Direction::RIGHT)
            direction = Direction::LEFT;
        else if (console::key(console::Key::K_RIGHT) && direction != Direction::LEFT)
            direction = Direction::RIGHT;
        else if (console::key(console::Key::K_ESC)) {
            exit(0);
            break;
        }

        if (!appleExists)
            generateApple();
        // 뱀 이동 처리
        
        drawGame();
        for (int i = 0; i < MOVE_DELAY; i++)
            console::wait();
        // 게임 오버 또는 승리 판정
        if (isGameOver()) {
            gameState = GameState::GAMEOVER;
            drawGameOver();
            while (true) {
                
                if (console::key(console::Key::K_ESC)) exit(0);
                if (console::key(console::Key::K_ENTER)) {
                    gameState = GameState::PLAYING;
                    initializeGame();
                    break;
                }
                console::wait();
            }
        } else if (isWin()) {
            gameState = GameState::WIN;
            drawWin();
            while (true) {
                
                if (console::key(console::Key::K_ESC)) exit(0);
                if (console::key(console::Key::K_ENTER)) {
                    gameState = GameState::PLAYING;
                    initializeGame();
                    break;
                }
                console::wait();
            }
        }

    }
}

int main() {
    std::srand(std::time(nullptr));
    // 게임 초기화
    console::init();
    


    initializeGame();

    // 게임 루프
    gameLoop();

    return 0;
}