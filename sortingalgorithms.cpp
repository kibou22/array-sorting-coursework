#include "sortingalgorithms.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

// Перевіряє допустимість значень масиву.
void SortingAlgorithms::validateValues(
    const std::vector<int>& array
)
{
    for (int value : array) {
        if (value < 0 || value > 50000) {
            throw std::invalid_argument(
                "Значення елементів мають бути від 0 до 50000."
            );
        }
    }
}

// Записує опис етапу та відповідні числа.
// У звичайному режимі steps == nullptr, тому запис вимкнений.
void SortingAlgorithms::record(
    std::vector<SortStep>* steps,
    const std::string& description,
    const std::vector<int>& array
)
{
    if (steps) {
        steps->push_back({description, array});
    }
}

// Блочне сортування із можливістю запису етапів.
void SortingAlgorithms::bucketSortImpl(
    std::vector<int>& array,
    std::vector<SortStep>* steps
)
{
    validateValues(array);

    if (array.size() < 2) {
        return;
    }

    const auto bounds =
        std::minmax_element(array.begin(), array.end());

    const int minValue = *bounds.first;
    const int maxValue = *bounds.second;

    if (minValue == maxValue) {
        return;
    }

    const std::size_t bucketCount = std::max(
        std::size_t{2},
        static_cast<std::size_t>(
            std::sqrt(static_cast<double>(array.size()))
        )
    );

    // Блоки для окремих частин діапазону значень.
    std::vector<std::vector<int>> buckets(bucketCount);

    const long long range =
        static_cast<long long>(maxValue) - minValue + 1;

    for (int value : array) {
        const std::size_t index = static_cast<std::size_t>(
            (static_cast<long long>(value) - minValue)
            * static_cast<long long>(bucketCount) / range
        );

        buckets[index].push_back(value);
    }

    if (steps) {
        for (std::size_t i = 0; i < buckets.size(); ++i) {
            record(
                steps,
                "Блок " + std::to_string(i + 1)
                    + " після розподілу "
                      "(показано вміст цього блоку)",
                buckets[i]
            );
        }
    }

    std::size_t position = 0;

    for (auto& bucket : buckets) {
        std::sort(bucket.begin(), bucket.end());

        record(
            steps,
            "Упорядкований блок (показано вміст блоку)",
            bucket
        );

        for (int value : bucket) {
            array[position++] = value;
        }
    }
}

// Сортування підрахунком із можливістю запису етапів.
void SortingAlgorithms::countingSortImpl(
    std::vector<int>& array,
    std::vector<SortStep>* steps
)
{
    validateValues(array);

    if (array.empty()) {
        return;
    }

    const int maxValue =
        *std::max_element(array.begin(), array.end());

    // count[value] — кількість входжень числа value.
    std::vector<int> count(maxValue + 1, 0);

    for (int value : array) {
        ++count[value];
    }

    if (steps) {
        std::string frequencies = "Кількості входжень: ";

        for (int value = 0; value <= maxValue; ++value) {
            if (count[value] > 0) {
                frequencies +=
                    std::to_string(value) + " → "
                    + std::to_string(count[value]) + "; ";
            }
        }

        record(steps, frequencies, array);
    }

    std::size_t position = 0;

    for (int value = 0; value <= maxValue; ++value) {
        for (int occurrence = 0;
             occurrence < count[value];
             ++occurrence) {
            array[position++] = value;
        }
    }
}

// Стабільно впорядковує числа за одним десятковим розрядом.
void SortingAlgorithms::countingSortByDigit(
    std::vector<int>& array,
    int exponent
)
{
    std::array<std::size_t, 10> count{};
    std::vector<int> output(array.size());

    for (int value : array) {
        const int digit = (value / exponent) % 10;
        ++count[digit];
    }

    // Накопичені кількості визначають позиції цифр.
    for (std::size_t digit = 1; digit < count.size(); ++digit) {
        count[digit] += count[digit - 1];
    }

    // Обхід справа наліво забезпечує стабільність проходу.
    for (std::size_t i = array.size(); i > 0; --i) {
        const int value = array[i - 1];
        const int digit = (value / exponent) % 10;

        output[--count[digit]] = value;
    }

    array.swap(output);
}

// Порозрядне сортування із можливістю запису етапів.
void SortingAlgorithms::radixSortImpl(
    std::vector<int>& array,
    std::vector<SortStep>* steps
)
{
    validateValues(array);

    if (array.size() < 2) {
        return;
    }

    const int maxValue =
        *std::max_element(array.begin(), array.end());

    for (int exponent = 1;
         maxValue / exponent > 0;
         exponent *= 10) {
        countingSortByDigit(array, exponent);

        record(
            steps,
            "Після обробки розряду "
                + std::to_string(exponent),
            array
        );
    }
}

// Допоміжне сортування вставками.
void SortingAlgorithms::insertionSort(std::vector<int>& array)
{
    for (std::size_t i = 1; i < array.size(); ++i) {
        const int currentValue = array[i];
        std::size_t position = i;

        while (position > 0 &&
               array[position - 1] > currentValue) {
            array[position] = array[position - 1];
            --position;
        }

        array[position] = currentValue;
    }
}

// Флеш-сортування із можливістю запису етапів.
void SortingAlgorithms::flashSortImpl(
    std::vector<int>& array,
    std::vector<SortStep>* steps
)
{
    validateValues(array);

    if (array.size() < 2) {
        return;
    }

    const std::size_t size = array.size();

    int minValue = array[0];
    int maxValue = array[0];
    std::size_t maxIndex = 0;

    for (std::size_t i = 1; i < size; ++i) {
        minValue = std::min(minValue, array[i]);

        if (array[i] > maxValue) {
            maxValue = array[i];
            maxIndex = i;
        }
    }

    // Однакові значення вже впорядковані.
    if (minValue == maxValue) {
        return;
    }

    const std::size_t classCount = std::max(
        std::size_t{2},
        static_cast<std::size_t>(
            0.45 * static_cast<double>(size)
        )
    );

    // Визначає номер класу для заданого числа.
    const auto classIndex = [=](int value) -> std::size_t {
        return static_cast<std::size_t>(
            (static_cast<long long>(value) - minValue)
            * static_cast<long long>(classCount - 1)
            / (static_cast<long long>(maxValue) - minValue)
        );
    };

    // Кількості елементів, які перетворюються на межі класів.
    std::vector<std::size_t> boundaries(classCount, 0);

    for (int value : array) {
        ++boundaries[classIndex(value)];
    }

    for (std::size_t i = 1; i < classCount; ++i) {
        boundaries[i] += boundaries[i - 1];
    }

    if (steps) {
        std::string classes =
            "Класи початкових чисел (нумерація від 0): ";

        for (int value : array) {
            classes +=
                std::to_string(value) + " → "
                + std::to_string(classIndex(value)) + "; ";
        }

        record(steps, classes, array);
    }

    std::swap(array[0], array[maxIndex]);

    std::size_t moved = 0;
    std::size_t position = 0;
    std::size_t currentClass = classCount - 1;

    // Циклічно переміщуємо числа у відповідні класи.
    while (moved < size - 1) {
        while (position >= boundaries[currentClass]) {
            ++position;
            currentClass = classIndex(array[position]);
        }

        int carriedValue = array[position];

        while (position != boundaries[currentClass]) {
            currentClass = classIndex(carriedValue);

            const std::size_t destination =
                --boundaries[currentClass];

            std::swap(carriedValue, array[destination]);
            ++moved;
        }
    }

    record(
        steps,
        "Після перестановки чисел за класами",
        array
    );

    insertionSort(array);
}

// Звичайні виклики сортувань без запису етапів.
void SortingAlgorithms::bucketSort(std::vector<int>& array)
{
    bucketSortImpl(array, nullptr);
}

void SortingAlgorithms::countingSort(std::vector<int>& array)
{
    countingSortImpl(array, nullptr);
}

void SortingAlgorithms::radixSort(std::vector<int>& array)
{
    radixSortImpl(array, nullptr);
}

void SortingAlgorithms::flashSort(std::vector<int>& array)
{
    flashSortImpl(array, nullptr);
}

// Виконує обраний метод на навчальному масиві
// та повертає основні етапи його роботи.
std::vector<SortStep> SortingAlgorithms::demonstrate(
    int algorithm,
    const std::vector<int>& input
)
{
    if (input.empty() || input.size() > 20) {
        throw std::invalid_argument(
            "Для демонстрації потрібно від 1 до 20 чисел."
        );
    }

    validateValues(input);

    auto array = input;
    std::vector<SortStep> steps;

    record(&steps, "Початковий масив", array);

    switch (algorithm) {
    case 0:
        bucketSortImpl(array, &steps);
        break;
    case 1:
        countingSortImpl(array, &steps);
        break;
    case 2:
        radixSortImpl(array, &steps);
        break;
    case 3:
        flashSortImpl(array, &steps);
        break;
    default:
        throw std::invalid_argument(
            "Невідомий метод сортування."
        );
    }

    auto expected = input;
    std::sort(expected.begin(), expected.end());

    if (array != expected) {
        throw std::runtime_error(
            "Демонстрація не пройшла перевірку результату."
        );
    }

    record(
        &steps,
        "Кінцевий масив. Результат перевірено за еталоном",
        array
    );

    return steps;
}