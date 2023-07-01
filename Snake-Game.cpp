#include <iostream>
#include <ctime>
#include <conio.h>
#include <windows.h>
#include <vector>
using namespace std;

const int width = 40;
const int height = 40;

class Timer
{
private:
    long long nextStop;
    long long pause;
public:
    Timer(long long pause) : pause(pause), nextStop(pause + clock()) {}

    Timer operator+(const Timer& other) const {
        long long newPause = this->pause + other.pause;
        return Timer(newPause);
    }

    bool IsPausePassed()
    {
        if (nextStop <= clock())
        {
            nextStop += pause;
            return true;
        }
        return false;
    }
};

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};


class SnakeBase
{
protected:
    vector<pair<int, int>> body;
    Direction dir;
public:
    SnakeBase() : dir(Direction::Right)
    {
        body.push_back({ 25,25 });
    }
    virtual void Move() = 0;

    vector<pair<int, int>>& GetBody()
    {
        return body;
    }

protected:
    void SetDirection(Direction newDir)
    {
        if ((newDir == Direction::Up && dir != Direction::Down) ||
            (newDir == Direction::Down && dir != Direction::Up) ||
            (newDir == Direction::Left && dir != Direction::Right) ||
            (newDir == Direction::Right && dir != Direction::Left))
        {
            dir = newDir;
        }
    }

    void ChangePositions()
    {
        switch (dir) {
        case Direction::Up:
            body.insert(body.begin(), { body.front().first, body.front().second - 1 });
            break;
        case Direction::Down:
            body.insert(body.begin(), { body.front().first, body.front().second + 1 });
            break;
        case Direction::Left:
            body.insert(body.begin(), { body.front().first - 1, body.front().second });
            break;
        case Direction::Right:
            body.insert(body.begin(), { body.front().first + 1, body.front().second });
            break;
        }
        body.pop_back();
    }

    friend ostream& operator<<(ostream& os, const SnakeBase& snake);
};

ostream& operator<<(ostream& os, const SnakeBase& snake)
{
    os << "Snake with " << snake.body.size() << " segments, facing ";
    switch (snake.dir) {
    case Direction::Up:
        os << "up";
        break;
    case Direction::Down:
        os << "down";
        break;
    case Direction::Left:
        os << "left";
        break;
    case Direction::Right:
        os << "right";
        break;
    }
    os << endl;
    return os;
}



class Snake : public SnakeBase
{
public:
    void Move() override
    {
        if (_kbhit()) {
            int key = _getch();
            switch (key) {
            case 'w':
                SetDirection(Direction::Up);
                break;
            case 's':
                SetDirection(Direction::Down);
                break;
            case 'a':
                SetDirection(Direction::Left);
                break;
            case 'd':
                SetDirection(Direction::Right);
                break;
            }
        }
        ChangePositions();
    }
};


class RandomSnake : public SnakeBase
{
public:
    void Move() override
    {
        int key = rand() % 4;
        switch (key) {
        case 0:
            SetDirection(Direction::Up);
            break;
        case 1:
            SetDirection(Direction::Down);
            break;
        case 2:
            SetDirection(Direction::Left);
            break;
        case 3:
            SetDirection(Direction::Right);
            break;
        }
        ChangePositions();
    }

    ~RandomSnake() {
        for (int i = 0; i < body.size(); i++)
        {
            body[i].first = 0;
            body[i].second = 0;
        }
    }
    friend ostream& operator<<(ostream& os, const SnakeBase& snake);
};

class Field
{
private:
public:
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    bool IsInside(int x, int y) const
    {
        return x >= 0 && x < width&& y >= 0 && y < height;
    }
};


class Fruit {
private:
    int x;
    int y;
public:
    void Spawn(const Field& field)
    {
        x = rand() % field.GetWidth();
        y = rand() % field.GetHeight();
    }
    int GetX() const { return x; }
    int GetY() const { return y; }
};


class Length {
private:
    int fruits_eaten = 1;
public:
    Length() : fruits_eaten(1) {}
    void AddLength(int length)
    {
        fruits_eaten += length;
    }
    int GetLength() const
    {
        return fruits_eaten;
    }
    ~Length()
    {
        cout << "Snake's length: " << fruits_eaten << " cm" << endl;
    }
};


class Game {
private:
    Timer timer;
    Field field;
    SnakeBase* snake;
    Fruit fruit;
    Length length;
public:
    Game(long long pause, SnakeBase *snake) : timer(pause)
    {
        this->snake = snake; // It can be changed to playable Snake or randomly moving Snake (RandomSnake)
    }
    ~Game()
    {
        delete snake;
    }

    void Run() {
        fruit.Spawn(field); // Spawn the initial fruit
        while (true)
        {
            if (timer.IsPausePassed())
            {
                system("cls"); // Clear console
                cout << "+";

                for (int x = 0; x < field.GetWidth(); x++)
                {
                    cout << "-";
                }
                cout << "+" << endl;

                for (int y = 0; y < field.GetHeight(); y++)
                {
                    cout << "|";
                    for (int x = 0; x < field.GetWidth(); x++)
                    {
                        if (x == snake->GetBody().front().first && y == snake->GetBody().front().second)
                        {
                            cout << "S";
                        }
                        else if (find(snake->GetBody().begin() + 1, snake->GetBody().end(), make_pair(x, y)) != snake->GetBody().end())
                        {
                            cout << "o";
                        }
                        else if (x == fruit.GetX() && y == fruit.GetY())
                        {
                            cout << "F";
                        }
                        else
                        {
                            cout << " ";
                        }
                    }
                    cout << "|" << endl;
                }

                cout << "+";
                for (int x = 0; x < field.GetWidth(); x++)
                {
                    cout << "-";
                }
                cout << "+" << endl;
                cout << *snake; //to show snake condition of output overloading operator
                snake->Move();
                if (snake->GetBody().front().first == fruit.GetX() && snake->GetBody().front().second == fruit.GetY())
                {
                    snake->GetBody().push_back({ -1, -1 }); // Add a new segment to the snake
                    fruit.Spawn(field); // Spawn a new fruit
                    length.AddLength(1); // update snake length
                }
                if (!field.IsInside(snake->GetBody().front().first, snake->GetBody().front().second))
                {
                    cout << "Game over!" << endl;
                    return;
                }
            }
        }
    }
};


class TimePassed //class that will count how many snake ate fruits
{
private:
    clock_t start_time;
public:
    TimePassed()
    {
        start_time = clock();
    }
    int GetTime() const
    {
        clock_t current_time = clock();
        return ((current_time - start_time) / CLOCKS_PER_SEC); // divide by CLOCKS_PER_SEC to get time in seconds
    }
    ~TimePassed()
    {
        cout << "Time passed: " << GetTime() << " seconds" << endl;
    }
};

int main()
{
    int select;    
    cout << "Select snake (1. Player, 2. Random): ";
    cin >> select;
    SnakeBase* snake;
    if (select == 1)
        snake = new Snake();
    else
        snake = new RandomSnake();

    srand(time(0));
    Game game(150, snake);
    TimePassed sec_counter;
    game.Run();
    sec_counter.GetTime();

    delete snake;
    return 0;
}
