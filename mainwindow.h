#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <vector>

#include "arraygenerator.h"

class QSpinBox;
class QPlainTextEdit;
class QPushButton;
class QLabel;
class QComboBox;
class QAction;

// Головне вікно програми дослідження сортувань.
class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    // Генерує та показує початковий масив.
    void generateArray();

    // Сортує копію початкового масиву та перевіряє результат.
    void sortArray();

    // Зберігає дані останнього успішного сортування.
    void saveResult();

    QSpinBox* sizeInput;
    QPlainTextEdit* arrayOutput;
    QPlainTextEdit* sortedOutput;
    QPushButton* sortButton;
    QLabel* timeLabel;
    QComboBox* algorithmInput;
    QAction* saveAction;

    ArrayGenerator generator;

    std::vector<int> originalArray;
    std::vector<int> sortedArray;

    // Дані саме виконаного сортування, незалежно
    // від подальших змін вибору в списку.
    QString lastAlgorithm;
    double lastTimeMs = 0.0;
    bool hasResult = false;
};

#endif // MAINWINDOW_H