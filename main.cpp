#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <time.h>

using namespace std;

struct vec2{
    int x, y;
};

vec2 screenSize = {20, 20};
vec2 snake[40] = { NULL, NULL };
int snakeSize = 6;
int snakeDir = 3;
vec2 applePos;
bool appleReady = false;
vec2 obstaclePos;
bool obstacleReady = false;
char screen[20][20];
char background[20][20];
int gameSpeed = 1;
bool gameOver = false;

void SpawnApple(){
    while(!gameOver){
        if (!appleReady){
            this_thread::sleep_for(chrono::milliseconds(4000));
            bool touchingSnake = true;
            int x, y;
            // Set apple pos
            while(touchingSnake){
                // Random seed
                srand(time(NULL));
                x = rand() % screenSize.x;
                y = rand() % screenSize.y;
                touchingSnake = false;
                for (int i = snakeSize; i > 0; i--){
                    if (x == snake[i].x && y == snake[i].y){
                        touchingSnake = true;
                        break;
                    }
                }
            }

            applePos = {x, y};
            appleReady = true;
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void DestroyObstacleDelay(){
    this_thread::sleep_for(chrono::milliseconds(6000));
    if (!gameOver && obstacleReady){
        obstacleReady = false;
    }
}

void SpawnObstacle(){
    while(!gameOver){
        if (!obstacleReady){
            this_thread::sleep_for(chrono::milliseconds(9000));
            bool touchingSnake = true;
            int x, y;
            // Set apple pos
            while(touchingSnake){
                // Random seed
                srand(time(NULL));
                x = 1 + (rand() % (screenSize.x - 1));
                y = 1 + (rand() % (screenSize.y - 1));
                touchingSnake = false;
                for (int i = snakeSize; i > 0; i--){
                    if (x == snake[i].x && y == snake[i].y){
                        touchingSnake = true;
                        break;
                    }
                }
            }

            obstaclePos = {x, y};
            obstacleReady = true;
            thread t_destroy(DestroyObstacleDelay);
            t_destroy.join();
        }
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

void OutputScreen(){

    // Set console size and pos
    HWND hwnd = GetConsoleWindow();
    RECT rect = {100, 100, 310, 310};
    MoveWindow(hwnd, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,TRUE);

    while(!gameOver){
        // Clear screen
        system("cls");
        // Output screen
        for(int y = 0; y < screenSize.y; y++){
            for(int x = 0; x < screenSize.x; x++){
                cout << screen[x][y];
            }
            cout << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(30));
    }
}

void Snake(){
    while(!gameOver){
        // User controls
        if(GetKeyState('W') & 0x8000 && snakeDir != 0)
        {
            snakeDir = 2;
        }
        if(GetKeyState('S') & 0x8000 && snakeDir != 2)
        {
            snakeDir = 0;
        }
        if(GetKeyState('D') & 0x8000 && snakeDir != 3)
        {
            snakeDir = 1;
        }
        if(GetKeyState('A') & 0x8000 && snakeDir != 1)
        {
            snakeDir = 3;
        }
        // Snake movement
        switch (snakeDir){
            case 0:
                snake[0] = {snake[0].x, snake[0].y + 1};
                break;
            case 1:
                snake[0] = {snake[0].x + 1, snake[0].y};
                break;
            case 2:
                snake[0] = {snake[0].x, snake[0].y - 1};
                break;
            case 3:
                snake[0] = {snake[0].x - 1, snake[0].y};
                break;
        }
        // Check if touches itself
        for (int i = snakeSize; i > 0; i--){
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y){
                gameOver = true;
                break;
            }
        }
        // Keep in boundries
        if (snake[0].x > screenSize.x-1){
            snake[0].x = 0;
        }
        if (snake[0].x < 0){
            snake[0].x = screenSize.x-1;
        }
        if (snake[0].y > screenSize.y-1){
            snake[0].y = 0;
        }
        if (snake[0].y < 0){
            snake[0].y = screenSize.y-1;
        }
        // Update snake parts
        for (int i = snakeSize; i > 0; i--){
            snake[i] = snake[i-1];
            // Collect apple
            if (appleReady && applePos.x == snake[i].x && applePos.y == snake[i].y){
                appleReady = false;
                snakeSize++;
            }
            // Hit obstacle
            if (obstacleReady && obstaclePos.x - 2 < snake[i].x && obstaclePos.x + 2 > snake[i].x && obstaclePos.y == snake[i].y){
                obstacleReady = false;
                gameOver = true;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100 / gameSpeed));
    }
}

void Draw(){
    while(!gameOver){
        // Clear screen buffer
        for(int x = 0; x < screenSize.x; x++){
            for(int y = 0; y < screenSize.y; y++){
                screen[x][y] = NULL;
            }
        }

        // Draw backgroud
        for(int x = 0; x < screenSize.x; x++){
            for(int y = 0; y < screenSize.y; y++){
                screen[x][y] = background[x][y];
            }
        }

        // Draw apple
        if (appleReady){
            screen[applePos.x][applePos.y] = 'A';
        }
        // Draw snake
        for (int i = 0; i < snakeSize; i++){
            screen[snake[i].x][snake[i].y] = 'O';
        }
         // Draw obstacle
        if (obstacleReady){
            // Horizontal
            screen[obstaclePos.x][obstaclePos.y] = '|';
            screen[obstaclePos.x-1][obstaclePos.y] = '|';
            screen[obstaclePos.x+1][obstaclePos.y] = '|';
        }

        this_thread::sleep_for(chrono::milliseconds(100 / gameSpeed));
    }
}

int main()
{
    // Inital snake pos
    for (int i = 0; i < snakeSize; i++){
        snake[i] = {10, 8+i};
    }

    // Generate background
    for(int x = 0; x < screenSize.x; x++){
        for(int y = 0; y < screenSize.y; y++){
            int rnd = rand() % 80;
            if (rnd == 0){
                background[x][y] = '`';
            }
            else if (rnd == 1){
                background[x][y] = '.';
            }
        }
    }

    thread t_draw(Draw);
    thread t_snake(Snake);
    thread t_apple(SpawnApple);
    thread t_obstacle(SpawnObstacle);
    OutputScreen();

    system("cls");
    cout << "GAME OVER!" << endl;
    cout << "Score: " << snakeSize-6 << endl << endl;

    t_draw.join();
    t_snake.join();
    t_apple.join();
    t_obstacle.join();

    system("pause");

    return 0;
}
