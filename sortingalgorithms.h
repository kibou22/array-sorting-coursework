#ifndef SORTINGALGORITHMS_H
#define SORTINGALGORITHMS_H

#include <string>
#include <vector>

// Один етап демонстрації:
// description — пояснення виконаної дії;
// values — числа, які показуються на цьому етапі.
struct SortStep
{
    std::string description;
    std::vector<int> values;
};

// Алгоритми сортування цілих чисел у діапазоні 0–50000.
class SortingAlgorithms
{
public:
    // Сортує array за зростанням блочним методом.
    static void bucketSort(std::vector<int>& array);

    // Сортує array за зростанням методом підрахунку.
    static void countingSort(std::vector<int>& array);

    // Сортує array за зростанням порозрядним методом.
    static void radixSort(std::vector<int>& array);

    // Сортує array за зростанням методом Flash Sort.
    static void flashSort(std::vector<int>& array);

    // Виконує сортування із записом основних етапів.
    // algorithm: 0 — блочне, 1 — підрахунком,
    //            2 — порозрядне, 3 — флеш-сортування.
    // input — навчальний масив із 1–20 чисел.
    // Повертає послідовність етапів демонстрації.
    static std::vector<SortStep> demonstrate(
        int algorithm,
        const std::vector<int>& input
    );

private:
    // Реалізації сортувань зі змогою записувати етапи.
    // array — масив, який потрібно впорядкувати.
    // steps — список етапів; nullptr вимикає їх запис.
    static void bucketSortImpl(
        std::vector<int>& array,
        std::vector<SortStep>* steps
    );

    static void countingSortImpl(
        std::vector<int>& array,
        std::vector<SortStep>* steps
    );

    static void radixSortImpl(
        std::vector<int>& array,
        std::vector<SortStep>* steps
    );

    static void flashSortImpl(
        std::vector<int>& array,
        std::vector<SortStep>* steps
    );

    // Додає опис і поточні числа до списку етапів.
    // Якщо steps == nullptr, нічого не записує.
    static void record(
        std::vector<SortStep>* steps,
        const std::string& description,
        const std::vector<int>& array
    );

    // Перевіряє належність чисел діапазону 0–50000.
    static void validateValues(const std::vector<int>& array);

    // Стабільно сортує за розрядом exponent:
    // 1 — одиниці, 10 — десятки тощо.
    static void countingSortByDigit(
        std::vector<int>& array,
        int exponent
    );

    // Допоміжне сортування вставками.
    static void insertionSort(std::vector<int>& array);
};

#endif // SORTINGALGORITHMS_H