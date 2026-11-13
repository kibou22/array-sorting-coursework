#include "sortingalgorithms.h"
#include "arraygenerator.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Тип функції, що сортує масив цілих чисел.
using SortFunction = void (*)(std::vector<int>&);

int main()
{
    // Результати одночасно показуються в терміналі
    // та записуються у текстовий файл.
    std::ofstream report("test_results.txt");

    if (!report) {
        std::cerr << "Не вдалося створити test_results.txt\n";
        return 1;
    }

    int passed = 0;
    int failed = 0;

    // Записує результат однієї перевірки.
    const auto record = [&](const std::string& name, bool success) {
        const std::string line =
            std::string(success ? "[OK] " : "[FAIL] ") + name;

        std::cout << line << '\n';
        report << line << '\n';

        if (success) {
            ++passed;
        } else {
            ++failed;
        }
    };

    const std::vector<std::pair<std::string, SortFunction>> algorithms = {
        {"Bucket Sort", &SortingAlgorithms::bucketSort},
        {"Counting Sort", &SortingAlgorithms::countingSort},
        {"Radix Sort", &SortingAlgorithms::radixSort},
        {"Flash Sort", &SortingAlgorithms::flashSort}
    };

    // Невеликі набори перевіряють саме методи сортування.
    // Обмеження від 100 елементів стосується генерації в програмі.
    std::vector<std::pair<std::string, std::vector<int>>> cases = {
        {"Невпорядковані числа", {42, 15, 37, 8, 26, 49, 11}},
        {"Повторення", {4, 2, 2, 1, 3, 4, 2}},
        {"Впорядкований масив", {1, 5, 12, 24, 36, 50}},
        {"Зворотний порядок", {50, 40, 30, 20, 10, 0}},
        {"Однакові числа", {25, 25, 25, 25, 25, 25}},
        {"Граничні значення", {50000, 0, 125, 49999, 1, 25000}},
        {"Усі нулі", {0, 0, 0, 0, 0}},
        {"Порожній масив", {}},
        {"Один елемент", {17}},
        {"Два елементи", {50000, 0}}
    };

    // Фіксоване початкове значення робить тести відтворюваними.
    std::mt19937 engine(14);
    std::uniform_int_distribution<int> distribution(0, 50000);

    std::vector<int> largeArray(50000);

    for (int& value : largeArray) {
        value = distribution(engine);
    }

    cases.push_back({"50000 випадкових чисел", largeArray});

    // Перевіряємо всі методи на тих самих контрольних даних.
    for (const auto& algorithm : algorithms) {
        const std::string& algorithmName = algorithm.first;
        const SortFunction sort = algorithm.second;

        for (const auto& testCase : cases) {
            auto actual = testCase.second;
            auto expected = testCase.second;

            std::sort(expected.begin(), expected.end());

            bool success = false;

            try {
                sort(actual);
                success = (actual == expected);
            }
            catch (const std::exception&) {
                success = false;
            }

            record(
                algorithmName + ": " + testCase.first,
                success
            );
        }

        // Некоректні значення мають викликати invalid_argument.
        const std::vector<std::pair<std::string, std::vector<int>>>
            invalidCases = {
                {"Відхилення від'ємного числа", {2, -1, 7}},
                {"Відхилення числа понад 50000", {0, 50001, 7}}
            };

        for (const auto& testCase : invalidCases) {
            auto data = testCase.second;
            bool rejected = false;

            try {
                sort(data);
            }
            catch (const std::invalid_argument&) {
                rejected = true;
            }
            catch (...) {
                rejected = false;
            }

            record(
                algorithmName + ": " + testCase.first,
                rejected
            );
        }
    }

    // Додаткові випадкові масиви різної довжини.
    std::uniform_int_distribution<int> sizeDistribution(0, 300);

    for (const auto& algorithm : algorithms) {
        bool success = true;

        for (int test = 0; test < 200; ++test) {
            std::vector<int> data(sizeDistribution(engine));

            // Чергуємо широкий діапазон і багато повторень.
            for (int& value : data) {
                value = (test % 2 == 0)
                    ? distribution(engine)
                    : distribution(engine) % 5;
            }

            auto expected = data;
            std::sort(expected.begin(), expected.end());

            try {
                algorithm.second(data);

                if (data != expected) {
                    success = false;
                    break;
                }
            }
            catch (...) {
                success = false;
                break;
            }
        }

        record(
            algorithm.first + ": 200 додаткових випадкових наборів",
            success
        );
    }

    // Перевіряємо генератор.
    ArrayGenerator generator;

    for (int size : {100, 50000}) {
        bool success = false;

        try {
            const auto data = generator.generate(size);

            success =
                data.size() == static_cast<std::size_t>(size)
                && std::all_of(
                    data.begin(),
                    data.end(),
                    [](int value) {
                        return value >= 0 && value <= 50000;
                    }
                );
        }
        catch (...) {
            success = false;
        }

        record(
            "Генератор: розмір і діапазон для "
                + std::to_string(size) + " елементів",
            success
        );
    }

    // Кожна пара — size та maxValue.
    const std::vector<std::pair<int, int>> invalidParameters = {
        {99, 50000},
        {50001, 50000},
        {100, -1},
        {100, 50001}
    };

    for (const auto& parameters : invalidParameters) {
        bool rejected = false;

        try {
            generator.generate(parameters.first, parameters.second);
        }
        catch (const std::invalid_argument&) {
            rejected = true;
        }
        catch (...) {
            rejected = false;
        }

        record(
            "Генератор: відхилення параметрів size="
                + std::to_string(parameters.first)
                + ", maxValue="
                + std::to_string(parameters.second),
            rejected
        );
    }

    const std::string summary =
        "\nУспішно: " + std::to_string(passed)
        + "\nПомилок: " + std::to_string(failed) + "\n";

    std::cout << summary;
    report << summary;

    report.close();

    if (!report) {
        std::cerr << "Помилка запису результатів тестування.\n";
        return 1;
    }

    return failed == 0 ? 0 : 1;
}