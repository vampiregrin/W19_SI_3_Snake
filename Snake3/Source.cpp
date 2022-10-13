#include <iostream>
#include <vector>
#include <thread>
#include <Windows.h>
#include <conio.h>
#include <random>

using namespace std;

const char EMPTY_SPACE = '.';
const char FRUIT_CHAR = '*';

std::random_device r; 
std::default_random_engine e1(r());


struct Board {
    int width, height;
    vector < vector < char > > f; // fields (pola planszy)
    Board(int _width, int _height) :width(_width), height(_height),
        f(_height, vector<char>(_width, EMPTY_SPACE)) {

    }
    void draw() {
        for (int y = 0; y < f.size(); y++) {
            for (int x = 0; x < f[y].size(); x++) {
                if (f[y][x] == '1') cout << 'O';
                else if (f[y][x] == EMPTY_SPACE) cout << EMPTY_SPACE;
                else if (f[y][x] == FRUIT_CHAR) cout << FRUIT_CHAR;
                else cout << 'o';
            }
            cout << '\n';
        }
    }

    void drawDebug () {
        for (int y = 0; y < f.size(); y++) {
            for (int x = 0; x < f[y].size(); x++) {
                cout << f[y][x];
            }
            cout << '\n';
        }
    }
};




enum DIRECTION {
    RIGHT, DOWN, LEFT, UP
};

struct Player {
    int length;
    string name;
    int x, y; // 3, 2

    DIRECTION dir; // aktualny kierunek (direction)
    Player() :x(0), y(0), dir(RIGHT), length(5) { }

    void move(int hei, int wid, bool& isColiding) {
        switch (dir) {
        case RIGHT:
            if (++x >= wid) isColiding = true;
            break;
        case LEFT:
            if (--x < 0) isColiding = true;
            break;
        case DOWN:
            if (++y >= hei) isColiding = true;
            break;
        case UP:
            if (--y < 0) isColiding = true;
            break;
        }
    }

    bool isColiding(int wid, int hei) {

        if (x >= wid || x < 0) return true;
        if (x <= 0) return true;
        if (y >= hei) return true;
        if (--y >= wid) return true;

    }
};

struct Game {
    Board b;
    Player pl[1];
    bool gameOver;

    Game() :b(70, 20), gameOver(false) {}

    void resetCursor() {
        COORD tl = { 0,0 };
        CONSOLE_SCREEN_BUFFER_INFO s;
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(console, tl);
    }
    void mainLoop() {
        while (!gameOver) {
            putPlayersOnBoard();
            b.draw();
            this_thread::sleep_for(100ms);
            resetCursor();
            //pl[0].move();
            pl[0].move(b.f.size(), b.f[0].size(), gameOver);
            addFruit();
            moveTail();
            
        }
    }
    void addFruit() {
        std::uniform_int_distribution <int> uniform_distY(0, b.f.size() -1);
        std::uniform_int_distribution <int> uniform_distX(0, b.f[0].size()-1);
        auto px = uniform_distX(e1);
        auto py = uniform_distY(e1);

        b.f[py][px] = FRUIT_CHAR;
    }

    void moveTail() {
        for (int y = 0; y < b.f.size(); y++) {
            for (int x = 0; x < b.f[0].size(); x++) {
                if (b.f[y][x] != EMPTY_SPACE && b.f[y][x] != FRUIT_CHAR) {
                    b.f[y][x] = b.f[y][x] + 1;
                    if (b.f[y][x] == '0' + pl[0].length) b.f[y][x] = EMPTY_SPACE;
                }
            }
        }
    }

    void putPlayersOnBoard() {
        auto px = pl[0].x;
        auto py = pl[0].y;
        if (b.f[py][px] == FRUIT_CHAR) {
            pl[0].length+= 3;
        }
        else if (b.f[py][px] != EMPTY_SPACE) {
            gameOver = true;
            return;
        }
     
        b.f[py][px] = '1';
       
    }
};

int main()
{
    Game game;

    thread inputThread([&game]() {
        int x;
        do {
            x = _getch();
            if (x == 's') game.pl[0].dir = DOWN;
            // dodać żeby się nie dało cofać
            if (x == 'd') game.pl[0].dir = RIGHT;
            if (x == 'a') game.pl[0].dir = LEFT;
            if (x == 'w') game.pl[0].dir = UP;
        } while (x != 27);
        game.gameOver = true;
        });

    game.mainLoop();

    inputThread.join();
}