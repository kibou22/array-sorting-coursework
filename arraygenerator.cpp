#include "arraygenerator.h"

#include <stdexcept>

ArrayGenerator::ArrayGenerator()
    : engine(std::random_device{}())
{
}

std::vector<int> ArrayGenerator::generate(int size, int maxValue)
{
    if (size < 100 || size > 50000) {
        throw std::invalid_argument(
            "Кількість елементів має бути від 100 до 50000."
        );
    }

    if (maxValue < 0 || maxValue > 50000) {
        throw std::invalid_argument(
            "Максимальне значення має бути від 0 до 50000."
        );
    }

    std::uniform_int_distribution<int> distribution(0, maxValue);

    std::vector<int> array;
    array.reserve(size);

    for (int i = 0; i < size; ++i) {
        array.push_back(distribution(engine));
    }

    return array;
}