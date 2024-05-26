#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <Windows.h>
#include <string>
#include <codecvt>
#include <locale>
#include <thread>
#include <chrono>

//функция для обновления экрана без просадки FPS (моя фишка)
void setcur(int x1, int y1) {
    COORD coord;
    coord.X = x1;
    coord.Y = y1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

class Object;
struct Ocean;

struct Action {

    Action(Object* obj) : obj{ obj } {}

    virtual ~Action() = default;
    bool operator()(Ocean& f) { apply(f); }

protected:
    Object* obj;

private:
    virtual bool apply(Ocean&) const = 0;
};

struct NoAction : public Action {
    NoAction(Object* obj) : Action{ obj } {}
    bool apply(Ocean&) const { /* do literally nothing */ return true; };

};


struct Move : public Action {
    enum class Direction { N, S, W, E };
    Move(Object* obj, size_t count, Direction d) : Action{ obj } {}

    bool apply(Ocean&) const {
        /* your code */
    };

};


class Object {
public:
    std::string symbol;
    virtual ~Object() {};
    virtual void update() = 0;
    virtual std::string getSymbol() const = 0;
    int getFlag = 0;
    int not_eat = 0;
};

class Empty : public Object {
private:
    int flag;
public:
    Empty() : flag(0) { symbol = " "; }
    void update() override {}

    std::string getSymbol() const override { return symbol; }
};

class Stone : public Object {
private:
    int to_reef;
    int flag;
public:
    Stone() : to_reef(std::rand() % 3 + 5), flag(0) { symbol = "*"; }

    void update() override {
        if (to_reef == 0) {
            symbol = "&";
        }
        else
            to_reef--;
    }
    std::string getSymbol() const override { return symbol; }
};

class Reef : public Object {
private:
    int to_stone;
    int flag;
public:
    Reef() : to_stone(std::rand() % 3 + 5), flag(0) { symbol = "&"; }
    void update() override {
        if (to_stone == 0)
            symbol = "*";
        else
            to_stone--;
    }
    std::string getSymbol() const override { return symbol; }
};

class Prey : public Object {
private:
    int age;
    std::string symbol;
public:
    Prey() : age(0), symbol("~") {}
    void update() override {
        age++;
        if (age == 10) {
            symbol = ">";
            return;
        }
        else if (age == 20) {
            symbol = " ";
            return;
        }
    }
    std::string getSymbol() const override { return symbol; }
};

class Predator : public Object {
private:
    int age;
    std::string symbol; // Declare symbol member variable
public:
    Predator() : age(0), symbol("?") {}
    void update() override {
        age++;
        if (age == 12) {
            symbol = "@";
            return;
        }
        else if (age == 25) {
            symbol = " ";
            return;
        }
    }
    std::string getSymbol() const override { return symbol; }
};
class ApexPredator : public Object {
private:
    int age;
    int not_eat;
    std::string symbol; // Declare symbol member variable
public:
    ApexPredator() : age(0), symbol("?"), not_eat(0) {}
    void update() override {
        age++;
        if (age == 10) {
            symbol = "$";
            return;
        }
        else if (age == 25) {
            symbol = " ";
            return;
        }
    }
    std::string getSymbol() const override { return symbol; }
};


using Neighbourhood = Object*;


struct Ocean final {
public:
    using Cell = Object*;
    size_t iteration_Counter;

    Ocean(size_t r, size_t c) : rows{ r }, cols{ c }, data{ r * c }, iteration_Counter{ 1 } {
        srand(static_cast<unsigned int>(time(nullptr)));
        //Наполните океан случайными организмами
        for (size_t i = 0; i < r; i++) {
            for (size_t j = 0; j < c; j++)
                generate_neighbourhood(i, j);
        }
    }

    Neighbourhood generate_neighbourhood(int i, int j) {
        int randNum = rand() % 100;

        if (randNum < 30 || randNum > 50)
            return  data[i * cols + j] = new Empty();
        else if (randNum >= 30 && randNum < 32)
            return  data[i * cols + j] = new Stone();
        else if (randNum >= 32 && randNum < 35)
            return  data[i * cols + j] = new Reef();
        else if (randNum >= 35 && randNum < 45)
            return  data[i * cols + j] = new Prey();
        else if (randNum >= 45 && randNum < 48)
            return  data[i * cols + j] = new Predator();
        else
            return  data[i * cols + j] = new ApexPredator();
    }



    void tick() {
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i * cols + j]->update();
                if (data[i * cols + j]->getFlag == 0) {
                    move_prey(i, j);
                    move_apex(i, j);
                    move_pred(i, j);
                }
                createNewPreyIfPossible(i, j);
                createNewApexIfPossible(i, j);
                createNewPredatorIfPossible(i, j);
            }
        }

        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                data[i * cols + j]->getFlag = 0;
            }
        }


        iteration_Counter++;
    }

    void display() {
        setcur(0, 0);
        std::cout << "                     Iteration = " << iteration_Counter << std::endl;
        std::cout << "        +--------------------------------------+" << std::endl;
        std::cout << "        |~ - мальки добычи  > - Рыбка (Добыча) |" << std::endl;
        std::cout << "        |? - малёк хищника  @ - Хищник         |" << std::endl;
        std::cout << "        |? - малёк хищника  $ - Высшего Хищник |" << std::endl;
        std::cout << "        |* - камень на дне  & - риф морской    |" << std::endl;
        std::cout << "        +--------------------------------------+" << std::endl;
        for (size_t i = 0; i < rows; i++) {
            for (size_t j = 0; j < cols; j++) {
                std::cout << data[i * cols + j]->getSymbol() << " ";
            }
            std::cout << std::endl;
        }
        Sleep(500);
        std::cout << std::endl;
    }

private:
    size_t rows, cols;
    std::vector<Cell> data;


    void createNewPreyIfPossible(size_t i, size_t j) {
        if (data[i * cols + j]->getSymbol() == ">") {
            bool maturePreyAdjacent = false; //зрелая добыча Рядом
            bool emptyCellAdjacent = false; // пустая ячейка Рядом
            bool emptyPredatorAdjacent = false; //нет рядом хищника
            int par_x, par_y; //координаты второй половинки
            int create_x, create_y; //координаты роддома
            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    size_t _x = i + x;
                    size_t _y = j + y;
                    if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                        //рыба еще не размножалась
                        if (data[_x * cols + _y]->getSymbol() == ">") {
                            maturePreyAdjacent = true;
                            par_x = _x;
                            par_y = _y;
                        }
                        //рядом есть место родить
                        else if (data[_x * cols + _y]->getSymbol() == " ") {
                            emptyCellAdjacent = true;
                            create_x = _x;
                            create_y = _y;
                        }
                        //рядом нет хищника
                        else if (data[_x * cols + _y]->getSymbol() == "$" || data[_x * cols + _y]->getSymbol() == "@") {
                            emptyPredatorAdjacent = true;
                        }
                    }
                }
            }

            if (maturePreyAdjacent && emptyCellAdjacent && !emptyPredatorAdjacent) {
                int start = 1;
                int end = 100;
                int x = rand() % (end - start + 1) + start;
                if (x % 4 == 0)
                    data[create_x * cols + create_y] = new Prey();
                return;
            }
        }
    }


    void move_prey(size_t i, size_t j) {
        if (data[i * cols + j]->getSymbol() == ">" || data[i * cols + j]->getSymbol() == "~") {
            //проверка на хищника
            for (int x = -1; x <= 1; ++x) {
                for (int y = -1; y <= 1; ++y) {
                    size_t _x = i + x;
                    size_t _y = j + y;
                    if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                        //если хищник , то даём дёру
                        if (data[_x * cols + _y]->getSymbol() == "$") {
                            if (data[i * cols + j - 1]->getSymbol() == " " && (i * cols + j - 1) < rows * cols) {
                                std::swap(data[i * cols + j - 1], data[i * cols + j]);
                                data[i * cols + j - 1]->getFlag = 1;
                                return;
                            }
                        }
                    }
                }
            }

            //сдвигаем в начало
            if (i * cols + j + 1 == rows * cols - 1 && data[0]->getSymbol() == " ") {
                std::swap(data[0], data[i * cols + j]);
                data[0]->getFlag = 1;
                return;
            }
            if (i * cols + j + 1 >= rows * cols)
                j = 0;
            else  if (i * cols + j + 2 >= rows * cols)
                j = 0;
            //передвигаем рыбку
            if (data[i * cols + j + 1]->getSymbol() == " ") {
                std::swap(data[i * cols + j + 1], data[i * cols + j]);
                if (i * cols + j + 1 < rows * cols) {
                    data[i * cols + j + 1]->getFlag = 1;
                }
                return;
            }
            //перепрыгиваем через препядствие
            else if (data[i * cols + j + 1]->getSymbol() == "*" || data[i * cols + j + 1]->getSymbol() == "&" || data[i * cols + j + 1]->getSymbol() == ">" || data[i * cols + j + 1]->getSymbol() == "?") {
                if (data[i * cols + j + 2]->getSymbol() == " ") {
                    std::swap(data[i * cols + j + 2], data[i * cols + j]);
                    data[i * cols + j + 2]->getFlag = 1;
                    return;
                }
            }
        }

    }


    bool reef_safe(size_t i, size_t j) {
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                size_t _x = i + x;
                size_t _y = j + y;
                if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                    if (data[_x * cols + _y]->getSymbol() == "&")
                        return true;
                }
            }
        }
        return false; //yoy can eat 
    }


    void move_apex(size_t i, size_t j) {
        if (data[i * cols + j]->getSymbol() == "$") {
            if (data[i * cols + j]->not_eat <= 3) {
                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        size_t _x = i + x;
                        size_t _y = j + y;
                        if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                            if ((data[_x * cols + _y]->getSymbol() == "~" || data[_x * cols + _y]->getSymbol() == ">") && reef_safe(_x, _y) == false) {
                                data[_x * cols + _y] = new Empty();
                                std::swap(data[_x * cols + _y], data[i * cols + j]);
                                data[_x * cols + _y]->getFlag = 1;
                                data[i * cols + j]->not_eat = 0;
                                return;
                            }
                        }
                    }
                }
            }
            //долго не ел, увеличиваем скорость
            else if (data[i * cols + j]->not_eat > 3 && data[i * cols + j]->not_eat <= 6) {
                for (int x = -2; x <= 2; ++x) {
                    for (int y = -2; y <= 2; ++y) {
                        size_t _x = i + x;
                        size_t _y = j + y;
                        if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                            //если хищник , то даём дёру
                            if ((data[_x * cols + _y]->getSymbol() == "~" || data[_x * cols + _y]->getSymbol() == ">" || data[_x * cols + _y]->getSymbol() == "@") && reef_safe(_x, _y) == false) {
                                data[_x * cols + _y] = new Empty();
                                std::swap(data[_x * cols + _y], data[i * cols + j]);
                                data[_x * cols + _y]->getFlag = 1;
                                data[i * cols + j]->not_eat = 0;
                                return;
                            }
                        }
                    }
                }
            }
            //очень долго не ел - умираем
            else if (data[i * cols + j]->not_eat < 7) {
                data[i * cols + j] = new Empty();
            }

            int it = 1;
            data[i * cols + j]->not_eat++;
            while (it < 8) {
                int start = -1;
                int end = 1;
                size_t _x = (rand() % (end - start + 1) + start) + i;
                size_t _y = (rand() % (end - start + 1) + start) + j;

                //сдвигаем в начало
                if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                    if (_x * cols + _y == rows * cols - 1 && data[0]->getSymbol() == " ") {
                        std::swap(data[0], data[i * cols + j]);
                        data[0]->getFlag = 1;
                        return;
                    }
                    if (_x * cols + _y >= rows * cols)
                        _y = 0;
                    //передвигаем рыбку
                    if (data[_x * cols + _y]->getSymbol() == " ") {
                        std::swap(data[_x * cols + _y], data[i * cols + j]);
                        if (_x * cols + _y < rows * cols) {
                            data[_x * cols + _y]->getFlag = 1;
                        }
                        return;
                    }
                }
                it++;
            }
            return;
        }
    }


    void createNewApexIfPossible(size_t i, size_t j) {
        if (data[i * cols + j]->getSymbol() == "$" && data[i * cols + j]->not_eat <= 3) { //если более-менее сыт
            bool matureApexAdjacent = false; //зрелая добыча Рядом
            bool emptyApexAdjacent = false; // пустая ячейка Рядом
            int par_x, par_y; //координаты второй половинки
            int create_x, create_y; //координаты роддома
            for (int x = -3; x <= 3; x++) {
                for (int y = -3; y <= 3; y++) {
                    size_t _x = i + x;
                    size_t _y = j + y;
                    if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                        //рыба еще не размножалась
                        if (data[_x * cols + _y]->getSymbol() == "$") {
                            matureApexAdjacent = true;
                            par_x = _x;
                            par_y = _y;
                        }
                        //рядом есть место родить
                        else if (data[_x * cols + _y]->getSymbol() == " ") {
                            emptyApexAdjacent = true;
                            create_x = _x;
                            create_y = _y;
                        }
                    }
                }
            }

            if (matureApexAdjacent && emptyApexAdjacent) {
                int start = 1;
                int end = 100;
                int x = rand() % (end - start + 1) + start;
                if (x % 4 == 0)
                    data[create_x * cols + create_y] = new ApexPredator();
                return;
            }
        }
    }


    void move_pred(size_t i, size_t j) {
        if (data[i * cols + j]->getSymbol() == "@") {
            if (data[i * cols + j]->not_eat <= 3) {
                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        size_t _x = i + x;
                        size_t _y = j + y;
                        if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                            if ((data[_x * cols + _y]->getSymbol() == "~" || data[_x * cols + _y]->getSymbol() == ">") && reef_safe(_x, _y) == false) {
                                data[_x * cols + _y] = new Empty();
                                std::swap(data[_x * cols + _y], data[i * cols + j]);
                                data[_x * cols + _y]->getFlag = 1;
                                data[i * cols + j]->not_eat = 0;
                                return;
                            }
                        }
                    }
                }
            }
            //долго не ел, увеличиваем скорость
            else if (data[i * cols + j]->not_eat > 3 && data[i * cols + j]->not_eat <= 6) {
                for (int x = -2; x <= 2; ++x) {
                    for (int y = -2; y <= 2; ++y) {
                        size_t _x = i + x;
                        size_t _y = j + y;
                        if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                            if ((data[_x * cols + _y]->getSymbol() == "~" || data[_x * cols + _y]->getSymbol() == ">") && reef_safe(_x, _y) == false) {
                                data[_x * cols + _y] = new Empty();
                                std::swap(data[_x * cols + _y], data[i * cols + j]);
                                data[_x * cols + _y]->getFlag = 1;
                                data[i * cols + j]->not_eat = 0;
                                return;
                            }
                        }
                    }
                }
            }
            //очень долго не ел - умираем
            else if (data[i * cols + j]->not_eat < 7) {
                data[i * cols + j] = new Empty();
            }

            int it = 1;
            data[i * cols + j]->not_eat++;
            while (it < 8) {
                int start = -1;
                int end = 1;
                size_t _x = (rand() % (end - start + 1) + start) + i;
                size_t _y = (rand() % (end - start + 1) + start) + j;

                //сдвигаем в начало
                if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                    if (_x * cols + _y == rows * cols - 1 && data[0]->getSymbol() == " ") {
                        std::swap(data[0], data[i * cols + j]);
                        data[0]->getFlag = 1;
                        return;
                    }
                    if (_x * cols + _y >= rows * cols)
                        _y = 0;
                    //передвигаем рыбку
                    if (data[_x * cols + _y]->getSymbol() == " ") {
                        std::swap(data[_x * cols + _y], data[i * cols + j]);
                        if (_x * cols + _y < rows * cols) {
                            data[_x * cols + _y]->getFlag = 1;
                        }
                        return;
                    }
                }
                it++;
            }
            return;
        }
    }


    void createNewPredatorIfPossible(size_t i, size_t j) {
        if (data[i * cols + j]->getSymbol() == "@" && data[i * cols + j]->not_eat <= 3) { //если более-менее сыт
            bool maturePredatorAdjacent = false; //зрелая добыча Рядом
            bool emptyCellAdjacent = false; // пустая ячейка Рядом
            bool emptyApexPredatorAdjacent = false; //нет рядом хищника
            int par_x, par_y; //координаты второй половинки
            int create_x, create_y; //координаты роддома
            for (int x = -2; x <= 2; x++) {
                for (int y = -2; y <= 2; y++) {
                    size_t _x = i + x;
                    size_t _y = j + y;
                    if (_x >= 0 && _x < rows && _y >= 0 && _y < cols) {
                        //рыба еще не размножалась
                        if (data[_x * cols + _y]->getSymbol() == "@") {
                            maturePredatorAdjacent = true;
                            par_x = _x;
                            par_y = _y;
                        }
                        //рядом есть место родить
                        else if (data[_x * cols + _y]->getSymbol() == " ") {
                            emptyCellAdjacent = true;
                            create_x = _x;
                            create_y = _y;
                        }
                        //рядом нет хищника
                        else if (data[_x * cols + _y]->getSymbol() == "$") {
                            emptyApexPredatorAdjacent = true;
                        }
                    }
                }
            }

            if (maturePredatorAdjacent && emptyCellAdjacent && !emptyApexPredatorAdjacent) {
                int start = 1;
                int end = 100;
                int x = rand() % (end - start + 1) + start;
                if (x % 4 == 0)
                    data[create_x * cols + create_y] = new Predator();
                return;
            }
        }
    }

};

int main() {
    setlocale(LC_ALL, "RU");
    int n, m;
    int iter;
    std::cout << "Введите размер для океана" << std::endl;
    std::cout << "Рекомендую размер океана 30 на 30" << std::endl;
    std::cout << "И обязательно сделайте полноэкранный режим для лучшего просмотра" << std::endl;
    std::cin >> n >> m;
    Ocean ocean(n, m);
    system("cls");
    std::cout << "Введите максимальное количество итераций, который хотите наблюдать" << std::endl;
    std::cout << "Программа может завершиться и раньше, если никаких тенденций нет" << std::endl;
    std::cin >> iter;
    //старт игры
    system("cls");
    while (true) {
        ocean.tick();
        ocean.display();
        if (ocean.iteration_Counter >= iter)
            break;
    }

    return 0;
}
