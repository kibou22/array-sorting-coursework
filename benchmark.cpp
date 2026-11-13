#include "sortingalgorithms.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using SortFunction = void (*)(std::vector<int>&);

// Назва методу та функція, яка його реалізує.
struct Algorithm
{
    std::string name;
    SortFunction sort;
};

int main()
{
    try {
        const std::array<Algorithm, 4> algorithms = {{
            {"Bucket Sort", &SortingAlgorithms::bucketSort},
            {"Counting Sort", &SortingAlgorithms::countingSort},
            {"Radix Sort", &SortingAlgorithms::radixSort},
            {"Flash Sort", &SortingAlgorithms::flashSort}
        }};

        const std::array<int, 6> sizes = {
            100, 1000, 5000, 10000, 25000, 50000
        };

        constexpr int repetitions = 10;

        // Фіксоване початкове значення забезпечує
        // відтворюваність вхідних даних.
        std::mt19937 engine(14);
        std::uniform_int_distribution<int> distribution(0, 50000);

        // Кожен рядок містить середній час чотирьох методів.
        std::vector<std::array<double, 4>> results;

        // Попередній запуск кожного методу перед вимірюваннями.
        std::vector<int> warmup(1000);

        for (int& value : warmup) {
            value = distribution(engine);
        }

        auto warmupExpected = warmup;
        std::sort(warmupExpected.begin(), warmupExpected.end());

        for (const auto& algorithm : algorithms) {
            auto data = warmup;
            algorithm.sort(data);

            if (data != warmupExpected) {
                throw std::runtime_error(
                    "Помилка попередньої перевірки: " + algorithm.name
                );
            }
        }

        for (int size : sizes) {
            std::array<double, 4> totalTime{};

            for (int repeat = 0; repeat < repetitions; ++repeat) {
                // Для кожного повторення створюється новий масив.
                std::vector<int> original(size);

                for (int& value : original) {
                    value = distribution(engine);
                }

                auto expected = original;
                std::sort(expected.begin(), expected.end());

                // Змінюємо порядок запуску методів,
                // щоб один метод не був завжди першим.
                for (int offset = 0; offset < 4; ++offset) {
                    const int index = (repeat + offset) % 4;
                    const auto& algorithm = algorithms[index];

                    // Усі методи отримують однакові початкові числа.
                    // Копіювання не входить у виміряний час.
                    auto data = original;

                    const auto start =
                        std::chrono::steady_clock::now();

                    algorithm.sort(data);

                    const auto finish =
                        std::chrono::steady_clock::now();

                    const double milliseconds =
                        std::chrono::duration<double, std::milli>(
                            finish - start
                        ).count();

                    // Перевірка також не входить у виміряний час.
                    if (data != expected) {
                        throw std::runtime_error(
                            "Неправильний результат: "
                            + algorithm.name
                            + ", розмір "
                            + std::to_string(size)
                        );
                    }

                    totalTime[index] += milliseconds;
                }
            }

            std::array<double, 4> averages{};

            for (std::size_t i = 0; i < averages.size(); ++i) {
                averages[i] = totalTime[i] / repetitions;
            }

            results.push_back(averages);

            std::cout << "Перевірено розмір: "
                      << size << '\n';
        }

        // Створюємо файли після успішного завершення вимірювань.
        std::ofstream report("benchmark_results.txt");
        std::ofstream csv("benchmark_results.csv");

        if (!report || !csv) {
            throw std::runtime_error(
                "Не вдалося створити файли результатів."
            );
        }

        // Крапка як десятковий роздільник у CSV.
        csv.imbue(std::locale::classic());

        report << std::fixed << std::setprecision(6);
        csv << std::fixed << std::setprecision(6);

        report << "ПОРІВНЯННЯ МЕТОДІВ СОРТУВАННЯ\n\n";
        report << "Діапазон значень: 0–50000\n";
        report << "Повторень для кожного розміру: "
               << repetitions << "\n";
        report << "Показник: середній час виконання, мс\n";
        report << "Генерація, копіювання, перевірка та запис "
                  "не входять у час.\n";
        report << "Усі отримані масиви збіглися з еталонними.\n\n";

        report << "Розмір\tBucket Sort\tCounting Sort"
                  "\tRadix Sort\tFlash Sort\n";

        csv << "size,bucket_ms,counting_ms,radix_ms,flash_ms\n";

        for (std::size_t row = 0; row < sizes.size(); ++row) {
            report << sizes[row];
            csv << sizes[row];

            for (double time : results[row]) {
                report << '\t' << time;
                csv << ',' << time;
            }

            report << '\n';
            csv << '\n';
        }

        report.close();
        csv.close();

        if (!report || !csv) {
            throw std::runtime_error(
                "Не вдалося повністю записати результати."
            );
        }

        std::cout
            << "\nГотово. Створено:\n"
            << "benchmark_results.txt\n"
            << "benchmark_results.csv\n";
    }
    catch (const std::exception& error) {
        std::cerr << "Помилка: " << error.what() << '\n';
        return 1;
    }

    return 0;
}