#include <iostream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <Windows.h>
#include <random> 

using namespace std;


class Vector
{
private:
    int* m_data;          // Указатель на динамически выделенный массив данных вектора
    size_t m_size;        // Текущий размер вектора (количество элементов)
    size_t m_capacity;    // Емкость вектора (сколько элементов можно хранить без выделения новой памяти)

public:
    // Конструктор с указанием размера
    Vector(size_t size) : m_size(size), m_capacity(size), m_data(new int[size]) 
    {
        srand(time(0)); // Инициализируем генератор случайных чисел
        for (size_t i = 0; i < m_size; ++i) 
        {
            m_data[i] = rand() % 20; // Заполняем массив случайными числами от 0 до 99
        }
    }

    // Конструктор копирования
    Vector(const Vector& other) : m_size(other.m_size), m_capacity(other.m_capacity), m_data(new int[other.m_size])
    {
        copy(other.m_data, other.m_data + other.m_size, m_data); // Копируем данные из другого вектора
    }

    // Конструктор перемещения
    Vector(Vector&& other) noexcept : m_size(other.m_size), m_capacity(other.m_capacity), m_data(other.m_data)
    {
        other.m_data = nullptr; // Передаем владение данными другому объекту, а старый объект очищаем
        other.m_size = 0;
        other.m_capacity = 0;
    }

    // Оператор присваивания копированием
    Vector& operator=(const Vector& other)
    {
        if (this == &other) return *this; // Проверка на самоприсваивание
        delete[] m_data; // Освобождаем старую память
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        m_data = new int[m_size];
        copy(other.m_data, other.m_data + m_size, m_data); // Копируем данные
        return *this;
    }

    // Оператор присваивания перемещением
    Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other) return *this; // Проверка на самоприсваивание
        delete[] m_data; // Освобождаем старую память
        m_data = other.m_data; // Передаем указатель на данные
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        other.m_data = nullptr; // Очищаем старый объект
        other.m_size = 0;
        other.m_capacity = 0;
        return *this;
    }

    // Добавление элемента в конец вектора
    void push_back(int value)
    {
        if (m_size == m_capacity) // Если нет свободного места, увеличиваем емкость
        {
            m_capacity = (m_capacity == 0) ? 1 : m_capacity * 2;
            int* new_data = new int[m_capacity];
            copy(m_data, m_data + m_size, new_data); // Копируем старые данные в новый массив
            delete[] m_data; // Удаляем старый массив
            m_data = new_data; // Присваиваем новый массив
        }
        m_data[m_size++] = value; // Добавляем новый элемент
    }

    // Получение среза вектора
    Vector slice(size_t start, size_t end) const
    {
        if (start >= m_size || end > m_size || start > end)
        {
            throw out_of_range("Недопустимый диапазон срезов"); // Проверка диапазона
        }
        Vector result(end - start); // Создаем новый вектор для среза
        copy(m_data + start, m_data + end, result.m_data); // Копируем срез в новый вектор
        return result;
    }

    // Итераторы для поддержки range-based for
    int* begin() { return m_data; }                // Неконстантный begin
    const int* begin() const { return m_data; }    // Константный begin
    int* end() { return m_data + m_size; }
    const int* end() const { return m_data + m_size; }

    // Поиск элемента в векторе
    int find(int value) const
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            if (m_data[i] == value) return i; // Возвращаем индекс, если нашли элемент
        }
        return -1; // Элемент не найден
    }

    // Сумма элементов вектора
    int sum() const
    {
        return accumulate(m_data, m_data + m_size, 0); // Используем accumulate для суммирования
    }

    // Деструктор
    ~Vector()
    {
        delete[] m_data; // Освобождаем память
    }
};

class Matrix
{
private:
    vector<Vector> m_rows; // Матрица хранится в виде векторов строк
    size_t m_cols;         // Количество столбцов в матрице

public:
    // Конструктор с параметрами: количество строк и столбцов
    Matrix(size_t rows, size_t cols) : m_rows(rows, Vector(cols)), m_cols(cols) {}

    // Конструктор копирования
    Matrix(const Matrix& other) : m_rows(other.m_rows), m_cols(other.m_cols) {}

    // Конструктор перемещения
    Matrix(Matrix&& other) noexcept : m_rows(move(other.m_rows)), m_cols(other.m_cols)
    {
        other.m_cols = 0; // Очищаем исходную матрицу
    }

    // Оператор присваивания копированием
    Matrix& operator=(const Matrix& other)
    {
        if (this == &other) return *this; // Проверка на самоприсваивание
        m_rows = other.m_rows;
        m_cols = other.m_cols;
        return *this;
    }

    // Оператор присваивания перемещением
    Matrix& operator=(Matrix&& other) noexcept
    {
        if (this == &other) return *this; // Проверка на самоприсваивание
        m_rows = move(other.m_rows); // Перемещаем строки
        m_cols = other.m_cols;
        other.m_cols = 0; // Очищаем исходную матрицу
        return *this;
    }

    // Метод для изменения размера матрицы
    void resize(size_t new_rows, size_t new_cols)
    {
        // Увеличиваем количество строк, если нужно
        if (new_rows > m_rows.size())
        {
            // Добавляем новые строки, инициализируем их нулями
            for (size_t i = m_rows.size(); i < new_rows; ++i)
            {
                m_rows.emplace_back(new_cols); // Создаем новые строки размером new_cols
                fill(m_rows.back().begin(), m_rows.back().end(), 0); // Заполняем нулями
            }
        }

        // Изменяем количество столбцов в каждой строке
        for (size_t i = 0; i < new_rows; ++i)
        {
            Vector& row = m_rows[i];
            size_t current_size = row.end() - row.begin();

            if (new_cols > current_size)
            {
                // Если новые размеры больше старых, добавляем нули в конец строки
                for (size_t j = current_size; j < new_cols; ++j)
                {
                    row.push_back(0); // Добавляем 0 в конец строки
                }
            }
            else if (new_cols < current_size)
            {
                // Если новые размеры меньше, пересоздаем строку с уменьшенным размером
                row = Vector(new_cols);
                fill(row.begin(), row.end(), 0); // Заполняем нулями
            }
        }

        // Обновляем количество столбцов
        m_cols = new_cols;
    }

    // Метод для транспонирования матрицы
    void transpose()
    {
        vector<Vector> transposed(m_cols, Vector(m_rows.size())); // Создаем новую матрицу с транспонированными размерами

        for (size_t i = 0; i < m_rows.size(); ++i)
        {
            for (size_t j = 0; j < m_cols; ++j)
            {
                transposed[j].push_back(m_rows[i].begin()[j]); // Меняем местами строки и столбцы
            }
        }

        *this = Matrix(transposed.size(), m_rows.size()); // Присваиваем транспонированную матрицу
        m_rows = transposed;
        m_cols = transposed.size();
    }

    // Метод для получения среза матрицы
    Matrix slice(size_t row_start, size_t row_end, size_t col_start, size_t col_end) const
    {
        if (row_start >= m_rows.size() || row_end > m_rows.size() || row_start > row_end || col_start >= m_cols || col_end > m_cols || col_start > col_end)
        {
            throw out_of_range("Недопустимый диапазон срезов"); // Проверка диапазона
        }

        Matrix result(row_end - row_start, col_end - col_start); // Создаем новую матрицу для среза
        for (size_t i = row_start; i < row_end; ++i)
        {
            result.m_rows[i - row_start] = m_rows[i].slice(col_start, col_end); // Копируем строки среза
        }
        return result;
    }

    // Поиск элемента в матрице
    pair<int, int> find(int value) const
    {
        for (size_t i = 0; i < m_rows.size(); ++i)
        {
            int col = m_rows[i].find(value); // Поиск в строке
            if (col != -1) return { i, col }; // Если элемент найден, возвращаем его координаты
        }
        return { -1, -1 }; // Элемент не найден
    }

    // Метод для вывода матрицы
    void print() const
    {
        for (const auto& row : m_rows)
        {
            for (size_t i = 0; i < m_cols; ++i)
            {
                cout << row.begin()[i] << " "; // Выводим элементы строки через пробел
            }
            cout << endl; // Переход на новую строку после каждой строки
        }
    }


    // Деструктор
    ~Matrix() = default; // Не нужен явный деструктор, т.к. вектор сам освобождает память
};

// Пример использования
int main()
{
    SetConsoleCP(1251); // Устанавливаем кодировку консоли для корректного вывода текста
    SetConsoleOutputCP(1251);

    Matrix m1(3, 3);  // Создаем матрицу 3x3
    cout << "Матрица m1:" << endl;
    m1.print();  // Выводим матрицу m1

    m1.resize(5, 5);  // Изменяем размер матрицы на НxН

    cout << "Матрица m1(увеличенная):" << endl;
    m1.print();  // Выводим матрицу m1

    Matrix m2 = m1.slice(1, 3, 1, 3);  // Создаем подматрицу
    cout << "Матрица m2 (срез m1):" << endl;
    m2.print();  // Выводим матрицу m2


    Matrix m3 = m2;  // Копируем матрицу
    Matrix m4 = move(m2);  // Перемещаем матрицу

    m4.transpose();  // Транспонируем матрицу
    cout << "Матрица m4 (транспонированная):" << endl;
    m4.print();  // Выводим транспонированную матрицу m4

    pair<int, int> pos = m4.find(9);  // Ищем элемент со значением ...
    cout << "Позиция элемента: (" << pos.first +1 << ", " << pos.second + 1 << ")" << endl;

    return 0;
}