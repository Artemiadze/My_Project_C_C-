#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <random>
#include <initializer_list>
#include <cassert>
#include <cmath>


class Matrix {
private:
    int rows;
    int cols;
    double** data;

    // Дополнительная функция для выделения памяти
    void GetMemory() {
        data = new double* [rows];
        for (int i = 0; i < rows; ++i)
            data[i] = new double[cols];
    }

    // Дополнительная функция для освобождения памяти
    void deleteMemory() {
        for (int i = 0; i < rows; ++i)
            delete[] data[i];
        delete[] data;
    }

public:
    // Конструкторы

    //который дефолтный
    Matrix() : rows(0), cols(0), data(nullptr) {}

    //создает матрицу размера n x m
    Matrix(int n, int m) : rows(n), cols(m) {
        GetMemory();
    }

    //создает матрицу размера n x m, заполненную числом val
    Matrix(int n, int m, double val) : rows(n), cols(m) {
        GetMemory();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j)
                data[i][j] = val;
        }
    }
    //Плюсом будет конструктор вида (См. std::initializer_list):
    Matrix(std::initializer_list<std::initializer_list<double>> list) {
        rows = list.size();
        cols = list.begin()->size();
        GetMemory();
        int i = 0;
        for (auto row : list) {
            int j = 0;
            for (auto val : row) {
                data[i][j] = val;
                ++j;
            }
            ++i;
        }
    }
    // копирование матрицы  
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
        GetMemory();
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                data[i][j] = other.data[i][j];
        }
    }


    // Деструктор матрицы
    ~Matrix() {
        deleteMemory();
    }


    // Статические методы
    //возвращает матрицу с единицами по диагонали
    static Matrix Identity(int n, int m) {
        Matrix identity(n, m, 0.0);
        int minDim = std::min(n, m);
        for (int i = 0; i < minDim; ++i)
            identity.data[i][i] = 1.0;
        return identity;
    }

    // возвращает матрицу, заполненную нулями
    static Matrix Zero(int n, int m) {
        return Matrix(n, m, 0.0);
    }

    //возвращает матрицу, заполненную случайными числами 
    static Matrix Random(const Matrix& matrix) {
        int rows = matrix.rows;
        int cols = matrix.cols;
        double minVal = 0.0;
        double maxVal = 9.0;

        //генерация чисел
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(minVal, maxVal);

        Matrix random_Matrix(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                random_Matrix.data[i][j] = dis(gen);
        }
        return random_Matrix;
    }

    //парсит строку и возвращает матрицу.
    static Matrix FromString(std::string str) {
        std::vector<std::vector<double>> values;
        size_t pos1 = str.find("[[");
        size_t pos2 = str.find("]]", pos1);
        std::string dataStr = str.substr(pos1 + 2, pos2 - pos1 - 2);
        size_t pos3 = dataStr.find("], [");
        while (pos3 != std::string::npos) {
            std::vector<double> row;
            std::string rowStr = dataStr.substr(0, pos3);
            size_t pos4 = rowStr.find(", ");
            while (pos4 != std::string::npos) {
                double val = std::stod(rowStr.substr(0, pos4));
                row.push_back(val);
                rowStr = rowStr.substr(pos4 + 2);
                pos4 = rowStr.find(", ");
            }
            double val = std::stod(rowStr);
            row.push_back(val);
            values.push_back(row);
            dataStr = dataStr.substr(pos3 + 4);
            pos3 = dataStr.find("], [");
        }
        std::vector<double> row;
        size_t pos4 = dataStr.find(", ");
        while (pos4 != std::string::npos) {
            double val = std::stod(dataStr.substr(0, pos4));
            row.push_back(val);
            dataStr = dataStr.substr(pos4 + 2);
            pos4 = dataStr.find(", ");
        }
        double val = std::stod(dataStr);
        row.push_back(val);
        values.push_back(row);
        int rows = values.size();
        int cols = values[0].size();
        Matrix mat(rows, cols);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                mat.data[i][j] = values[i][j];
            }
        }

        return mat;

    }

    // Транспонирование матрицы
    static Matrix transpose(const Matrix& other) {
        int cols = other.cols;
        int rows = other.rows;
        Matrix result_matrix(cols, rows);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result_matrix.data[j][i] = other.data[i][j];
        }
        return result_matrix;
    }


    // Сумма всех элементов матрицы
    static double sum(const Matrix& other) {
        double sum_matrix = 0;
        for (int i = 0; i < other.rows; ++i) {
            for (int j = 0; j < other.cols; ++j)
                sum_matrix += other.data[i][j];
        }
        return sum_matrix;
    }


    //Операторы и операции
    // Оператор присваивания
    Matrix& operator=(Matrix other) {
        std::swap(*this, other);
        return *this;
    }

    // Оператор индексации
    const double& operator()(int i, int j) const {
        return data[i][j];
    }

    //Арифметические операции
    Matrix operator+(const Matrix& other) const {
        assert(rows == other.rows && cols == other.cols);
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.data[i][j] = data[i][j] + other.data[i][j];
        }
        return result;
    }

    Matrix operator+(double scalar) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.data[i][j] = data[i][j] + scalar;
        }
        return result;
    }

    Matrix operator-(const Matrix& other) const {
        assert(rows == other.rows && cols == other.cols);
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.data[i][j] = data[i][j] - other.data[i][j];
        }
        return result;
    }
    //умножение матриц
    Matrix operator*(const Matrix& other) const {
        assert(cols == other.rows);
        Matrix result(rows, other.cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < other.cols; ++j) {
                for (int k = 0; k < cols; ++k)
                    result.data[i][j] += data[i][k] * other.data[k][j];
            }
        }
        return result;
    }
    //умножение матриц на число
    Matrix operator*(double scalar) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.data[i][j] = data[i][j] * scalar;
        }
        return result;
    }

    //для матриц
    //нужно доработать. создать обратную матрицу и уже по правилу деления матриц доделать матрицу
    Matrix operator/(const Matrix& matrix_1) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.data[i][j] = data[i][j] / matrix_1.data[i][j];
        }
        return result;
    }

    //для матрицы и числа
    Matrix operator/(const double scalar) const {
        assert(scalar != 0.0);
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.data[i][j] = data[i][j] / scalar;
        }
        return result;
    }

    Matrix operator-() const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                result.data[i][j] = -data[i][j];
        }
        return result;
    }

    // Логические операторы
    bool operator==(const Matrix& matrix) const {
        if (rows != matrix.rows || cols != matrix.cols) {
            return false;
        }
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (data[i][j] != matrix.data[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Matrix& matrix) const {
        return !(*this == matrix);
    }


    // Перегрузка оператора '<<' для работы с std::cout
    friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
        os << "[";
        for (int i = 0; i < matrix.rows; ++i) {
            os << "[";
            for (int j = 0; j < matrix.cols; ++j) {
                os << matrix.data[i][j] << (j < matrix.cols - 1 ? ", " : "");
            }
            os << "]" << (i < matrix.rows - 1 ? ",\n" : "");
        }
        os << "]";
        return os;
    }
};

int main() {
    setlocale(LC_ALL, "RU");
    std::string line;
    std::cout << "Введите матрицу A в формате: [[1, 2, 3], [4, 5, 6], [7, 8, 9] ...]: " << std::endl;
    std::cout << "Обратите внимание, после запятой обязательно ставьте запятую, если не хотите быть дураком!!!" << std::endl;
    std::getline(std::cin, line);
    Matrix A = Matrix::FromString(line);
    Matrix AT = Matrix::transpose(A);
    // Вывести матрицу A
    std::cout << "Матрица A:\n" << A << std::endl;
    std::cout << "Транспонированная Матрица AТ:\n" << AT << std::endl;

    //вывод суммы
    double sumA = Matrix::sum(A);
    std::cout << "Сумма элементов в матрице А\n" << sumA << std::endl;


    std::cout << std::endl;
    //создание рандомной матрицы B
    Matrix B = Matrix::Random(A);
    Matrix BT = Matrix::transpose(B);
    double sumB = Matrix::sum(B);
    std::cout << "Матрица B:\n" << B << std::endl;
    std::cout << "Транспонированная Матрица BТ:\n" << BT << std::endl;
    std::cout << "Сумма элементов в матрице B\n" << sumB << std::endl;
    std::cout << std::endl;


    //((A * B) - (B * AT) * <сумма элементов A>) + (BT * A * <сумма элементов B>)
    std::cout << "((A * B) - (B * AT) * <сумма элементов A>) + (BT * A + <сумма элементов B>) :" << std::endl;
    std::cout << ((A * B) - (B * AT) + sumA) + ((BT * A) + sumB) << std::endl;
    std::cout << std::endl;


    return 0;
}