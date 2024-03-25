#include "console.h"
#include <cstdlib>
#include <ctime>
#include <cstring>

// 맵의 크기 (최소 4, 최대 20)
#define BOARD_SIZE 10
// 뱀이 움직이는 딜레이
#define MOVE_DELAY 30

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

// 뱀의 이동 방향 열거형
enum class Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

Direction direction = Direction::UP;

// 게임 초기화 함수
void initializeGame() {
    // 랜덤 시드 설정
    std::srand(std::time(nullptr));

    // 뱀의 초기 위치 설정 (맵의 중앙)
    snake.x = BOARD_SIZE / 2;
    snake.y = BOARD_SIZE / 2;

    // 초기 뱀의 방향 설정
    direction = Direction::UP;

    // 초기화를 위한 화면 클리어
    console::clear();
}

// 사과 생성 함수
void generateApple() {
    appleExists = true;
    do {
        // 랜덤한 위치에 사과 생성
        appleX = std::rand() % (BOARD_SIZE - 2) + 1;
        appleY = std::rand() % (BOARD_SIZE - 2) + 1;
    } while (snake.x == appleX && snake.y == appleY);
}


// 뱀 이동 함수
void moveSnake() {
    // 이전 뱀 위치 저장
    int prevX = snake.x;
    int prevY = snake.y;

    // 이동 방향에 따라 뱀의 위치 조정
    switch (direction) {
        case Direction::UP:
            snake.y--;
            break;
        case Direction::DOWN:
            snake.y++;
            break;
        case Direction::LEFT:
            snake.x--;
            break;
        case Direction::RIGHT:
            snake.x++;
            break;
    }

    // 사과를 먹었을 경우
    if (snake.x == appleX && snake.y == appleY) {
        score += 10;
        generateApple();
    }
}

// 게임 오버 조건 확인 함수
bool isGameOver() {
    // 맵 밖으로 나갔을 경우
    if (snake.x < 0 || snake.x >= BOARD_SIZE || snake.y < 0 || snake.y >= BOARD_SIZE)
        return true;

    // 뱀이 자신의 몸통과 겹친 경우
    // (현재는 단일 뱀이므로 머리와 꼬리만 체크)
    if (snake.x == 0 && snake.y == 0)
        return true;

    return false;
}

// 게임 승리 조건 확인 함수
bool isWin() {
    // 뱀의 길이가 맵 전체를 차지한 경우
    if (score == (BOARD_SIZE - 2) * (BOARD_SIZE - 2))
        return true;

    return false;
}


// 게임 오버 텍스트 출력 함수
void drawGameOver() {
    int x = (console::SCREEN_WIDTH - strlen("Game Over!")) / 2;
    int y = console::SCREEN_HEIGHT / 2;
    console::draw(x, y, "Game Over!");
}

// 게임 승리 텍스트 출력 함수
void drawWin() {
    int x = (console::SCREEN_WIDTH - strlen("You Win!")) / 2;
    int y = console::SCREEN_HEIGHT / 2;
    console::draw(x, y, "You Win!");
}


// 맵 테두리 출력 함수
void drawBoard() {
    // 테두리 상단
    console::draw(0, 0, WALL_LEFT_TOP_STRING);
    for (int i = 1; i < BOARD_SIZE - 1; ++i) {
        console::draw(i, 0, WALL_HORIZONTAL_STRING);
    }
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
}

// 게임 상태에 따라 게임 화면 출력 함수
void drawGame() {
    console::clear();

    // 맵 테두리 출력
    drawBoard();

    // 뱀 이동
    moveSnake();
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
            console::cls();
            break;
        }

        // 뱀 이동 처리
        moveSnake();

        // 게임 오버 또는 승리 판정
        if (isGameOver()) {
            gameState = GameState::GAMEOVER;
            drawGameOver();
            console::wait();
            if (console::key(console::Key::K_ENTER)) {
                gameState = GameState::PLAYING;
                initializeGame();
            }
        } else if (isWin()) {
            gameState = GameState::WIN;
            drawWin();
            console::wait();
            if (console::key(console::Key::K_ENTER)) {
                gameState = GameState::PLAYING;
                initializeGame();
            }
        }

        // 화면 업데이트
        drawGame();
        for (int i =0; i < MOVE_DELAY; i++)
            console::wait();
    }
}

int main() {
    // 게임 초기화
    console::init();
    initializeGame();

    // 게임 루프
    gameLoop();

    return 0;
}
