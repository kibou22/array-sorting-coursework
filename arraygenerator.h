#ifndef ARRAYGENERATOR_H
#define ARRAYGENERATOR_H

#include <random>
#include <vector>

// Генерує масиви випадкових невід'ємних цілих чисел.
class ArrayGenerator
{
public:
    // Ініціалізує генератор випадкових чисел.
    ArrayGenerator();

    // size — кількість елементів.
    // maxValue — найбільше допустиме значення елемента.
    // Повертає масив зі значеннями від 0 до maxValue включно.
    std::vector<int> generate(int size, int maxValue = 50000);

private:
    std::mt19937 engine;
};

#endif // ARRAYGENERATOR_H