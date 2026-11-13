#include "mainwindow.h"
#include "sortingalgorithms.h"

#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSaveFile>
#include <QSpinBox>
#include <QStatusBar>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <chrono>
#include <exception>
#include <stdexcept>

namespace
{
// Перетворює масив на текст із розділенням комами.
QString arrayToText(const std::vector<int>& array)
{
    QStringList numbers;
    numbers.reserve(static_cast<qsizetype>(array.size()));

    for (int value : array) {
        numbers.append(QString::number(value));
    }

    return numbers.join(", ");
}
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Дослідження алгоритмів сортування");
    resize(1000, 800);

    // Центральна область та компонування головного вікна.
    auto* centralWidget = new QWidget(this);
    auto* layout = new QVBoxLayout(centralWidget);

    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    auto* title = new QLabel(
        "Упорядкування масивів",
        centralWidget
    );
    title->setStyleSheet(
        "font-size: 26px; font-weight: bold;"
    );

    auto* description = new QLabel(
        "Згенеруй масив, обери метод і запусти сортування.\n"
        "Результат можна зберегти через меню «Файл».\n"
        "Значення чисел: від 0 до 50 000 включно.",
        centralWidget
    );
    description->setWordWrap(true);

    // Параметри генерації.
    auto* sizeLabel = new QLabel(
        "Кількість елементів:",
        centralWidget
    );

    sizeInput = new QSpinBox(centralWidget);
    sizeInput->setRange(100, 50000);
    sizeInput->setValue(100);
    sizeInput->setMinimumWidth(120);

    auto* generateButton = new QPushButton(
        "Згенерувати масив",
        centralWidget
    );
    generateButton->setMinimumHeight(40);

    auto* controlsLayout = new QHBoxLayout;
    controlsLayout->addWidget(sizeLabel);
    controlsLayout->addWidget(sizeInput);
    controlsLayout->addWidget(generateButton);
    controlsLayout->addStretch();

    // Поле початкового масиву.
    arrayOutput = new QPlainTextEdit(centralWidget);
    arrayOutput->setReadOnly(true);
    arrayOutput->setPlaceholderText(
        "Тут з'являться згенеровані числа."
    );

    // Список вибору алгоритму.
    algorithmInput = new QComboBox(centralWidget);
    algorithmInput->addItem("Блочне сортування");
    algorithmInput->addItem("Сортування підрахунком");
    algorithmInput->addItem("Порозрядне сортування");
    algorithmInput->addItem("Флеш-сортування");

    sortButton = new QPushButton(
        "Сортувати",
        centralWidget
    );
    sortButton->setMinimumHeight(40);
    sortButton->setEnabled(false);

    auto* demoButton = new QPushButton(
        "Показати етапи",
        centralWidget
    );
    demoButton->setMinimumHeight(40);

    // Поле результату.
    sortedOutput = new QPlainTextEdit(centralWidget);
    sortedOutput->setReadOnly(true);
    sortedOutput->setPlaceholderText(
        "Тут з'явиться відсортований масив."
    );

    timeLabel = new QLabel(
        "Час сортування: ще не виміряно.",
        centralWidget
    );
    timeLabel->setWordWrap(true);

    // Розташування елементів у головному вікні.
    layout->addWidget(title);
    layout->addWidget(description);
    layout->addLayout(controlsLayout);

    layout->addWidget(
        new QLabel("Початковий масив:", centralWidget)
    );
    layout->addWidget(arrayOutput, 1);

    layout->addWidget(
        new QLabel("Метод сортування:", centralWidget)
    );
    layout->addWidget(algorithmInput);

    auto* actionsLayout = new QHBoxLayout;
    actionsLayout->addWidget(sortButton);
    actionsLayout->addWidget(demoButton);

    layout->addLayout(actionsLayout);

    layout->addWidget(
        new QLabel("Відсортований масив:", centralWidget)
    );
    layout->addWidget(sortedOutput, 1);
    layout->addWidget(timeLabel);

    setCentralWidget(centralWidget);

    // Підключення кнопок до відповідних дій.
    connect(
        generateButton,
        &QPushButton::clicked,
        this,
        &MainWindow::generateArray
    );

    connect(
        sortButton,
        &QPushButton::clicked,
        this,
        &MainWindow::sortArray
    );

    // Демонстрація працює на окремому малому прикладі.
    // Основний масив і результат сортування не змінюються.
    connect(
        demoButton,
        &QPushButton::clicked,
        this,
        [this]() {
            try {
                const std::vector<int> example = {
                    42, 15, 37, 8, 26, 49, 11, 15, 0, 32
                };

                const auto steps =
                    SortingAlgorithms::demonstrate(
                        algorithmInput->currentIndex(),
                        example
                    );

                QString text =
                    "Навчальний приклад із 10 чисел.\n"
                    "Генерація основних масивів працює "
                    "з розмірами 100–50000.\n\n";

                for (std::size_t i = 0; i < steps.size(); ++i) {
                    text += QString(
                        "Етап %1. %2\n%3\n\n"
                    )
                        .arg(
                            static_cast<qulonglong>(i + 1)
                        )
                        .arg(
                            QString::fromUtf8(
                                steps[i].description.c_str()
                            )
                        )
                        .arg(
                            arrayToText(steps[i].values)
                        );
                }

                // Окреме вікно з текстом етапів.
                QDialog dialog(this);
                dialog.setWindowTitle(
                    "Етапи: " + algorithmInput->currentText()
                );
                dialog.resize(850, 550);

                auto* demoLayout = new QVBoxLayout(&dialog);

                auto* output = new QPlainTextEdit(&dialog);
                output->setReadOnly(true);
                output->setPlainText(text);

                demoLayout->addWidget(output);

                auto* buttons = new QDialogButtonBox(
                    QDialogButtonBox::Close,
                    &dialog
                );

                connect(
                    buttons,
                    &QDialogButtonBox::rejected,
                    &dialog,
                    &QDialog::reject
                );

                demoLayout->addWidget(buttons);

                dialog.exec();
            }
            catch (const std::exception& error) {
                QMessageBox::critical(
                    this,
                    "Помилка демонстрації",
                    QString::fromUtf8(error.what())
                );
            }
        }
    );

    // На macOS меню показується у верхній панелі екрана.
    auto* fileMenu = menuBar()->addMenu("Файл");

    saveAction = fileMenu->addAction(
        "Зберегти результат…"
    );
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setEnabled(false);

    connect(
        saveAction,
        &QAction::triggered,
        this,
        &MainWindow::saveResult
    );

    fileMenu->addSeparator();

    auto* exitAction = fileMenu->addAction("Вийти");

    connect(
        exitAction,
        &QAction::triggered,
        this,
        &QWidget::close
    );

    auto* arrayMenu = menuBar()->addMenu("Масив");
    auto* generateAction = arrayMenu->addAction(
        "Згенерувати"
    );

    connect(
        generateAction,
        &QAction::triggered,
        this,
        &MainWindow::generateArray
    );

    statusBar()->showMessage(
        "Спочатку згенеруй масив або переглянь етапи."
    );
}

void MainWindow::generateArray()
{
    try {
        originalArray = generator.generate(
            sizeInput->value()
        );

        arrayOutput->setPlainText(
            arrayToText(originalArray)
        );

        // Попередній результат не відповідає новому масиву.
        sortedArray.clear();
        sortedOutput->clear();

        lastAlgorithm.clear();
        lastTimeMs = 0.0;
        hasResult = false;

        saveAction->setEnabled(false);
        sortButton->setEnabled(true);

        timeLabel->setText(
            "Час сортування: ще не виміряно."
        );

        statusBar()->showMessage(
            QString("Згенеровано %1 елементів.")
                .arg(
                    static_cast<qulonglong>(
                        originalArray.size()
                    )
                )
        );
    }
    catch (const std::exception& error) {
        QMessageBox::critical(
            this,
            "Помилка генерації",
            QString::fromUtf8(error.what())
        );
    }
}

void MainWindow::sortArray()
{
    if (originalArray.empty()) {
        QMessageBox::warning(
            this,
            "Немає даних",
            "Спочатку згенеруй масив."
        );
        return;
    }

    hasResult = false;
    saveAction->setEnabled(false);

    try {
        using SortFunction = void (*)(std::vector<int>&);
        SortFunction sortFunction = nullptr;

        switch (algorithmInput->currentIndex()) {
        case 0:
            sortFunction = &SortingAlgorithms::bucketSort;
            break;
        case 1:
            sortFunction = &SortingAlgorithms::countingSort;
            break;
        case 2:
            sortFunction = &SortingAlgorithms::radixSort;
            break;
        case 3:
            sortFunction = &SortingAlgorithms::flashSort;
            break;
        default:
            throw std::logic_error(
                "Не обрано метод сортування."
            );
        }

        // Сортуємо копію, залишаючи початкові дані незмінними.
        sortedArray = originalArray;

        const auto start =
            std::chrono::steady_clock::now();

        sortFunction(sortedArray);

        const auto finish =
            std::chrono::steady_clock::now();

        const double milliseconds =
            std::chrono::duration<double, std::milli>(
                finish - start
            ).count();

        // Перевірка не входить у виміряний час.
        auto expected = originalArray;
        std::sort(expected.begin(), expected.end());

        if (sortedArray != expected) {
            throw std::runtime_error(
                "Результат не збігається з еталонним."
            );
        }

        sortedOutput->setPlainText(
            arrayToText(sortedArray)
        );

        lastAlgorithm = algorithmInput->currentText();
        lastTimeMs = milliseconds;
        hasResult = true;

        saveAction->setEnabled(true);

        timeLabel->setText(
            QString("%1. Час: %2 мс")
                .arg(lastAlgorithm)
                .arg(lastTimeMs, 0, 'f', 6)
        );

        statusBar()->showMessage(
            "Масив відсортовано. "
            "Результат збігається з еталонним."
        );
    }
    catch (const std::exception& error) {
        sortedArray.clear();
        sortedOutput->clear();

        lastAlgorithm.clear();
        lastTimeMs = 0.0;

        timeLabel->setText(
            "Сортування не завершено."
        );

        statusBar()->showMessage(
            "Помилка сортування."
        );

        QMessageBox::critical(
            this,
            "Помилка сортування",
            QString::fromUtf8(error.what())
        );
    }
}

void MainWindow::saveResult()
{
    if (!hasResult) {
        QMessageBox::warning(
            this,
            "Немає результату",
            "Спочатку виконай сортування."
        );
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Зберегти результат",
        "sorting_result.txt",
        "Текстові файли (*.txt);;Усі файли (*)"
    );

    if (fileName.isEmpty()) {
        return;
    }

    if (!fileName.endsWith(".txt", Qt::CaseInsensitive)) {
        fileName += ".txt";
    }

    QString report;

    report += "РЕЗУЛЬТАТ СОРТУВАННЯ\n\n";
    report += "Метод: " + lastAlgorithm + "\n";

    report += QString("Кількість елементів: %1\n")
        .arg(
            static_cast<qulonglong>(
                originalArray.size()
            )
        );

    report += QString("Час виконання: %1 мс\n")
        .arg(lastTimeMs, 0, 'f', 6);

    report +=
        "Перевірка: результат збігається з еталонним.\n\n";

    report += "Початковий масив:\n";
    report += arrayToText(originalArray);

    report += "\n\nВідсортований масив:\n";
    report += arrayToText(sortedArray);
    report += "\n";

    const QByteArray data = report.toUtf8();
    QSaveFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(
            this,
            "Помилка збереження",
            "Не вдалося відкрити файл:\n"
                + file.errorString()
        );
        return;
    }

    if (file.write(data) != data.size()) {
        const QString error = file.errorString();
        file.cancelWriting();

        QMessageBox::critical(
            this,
            "Помилка збереження",
            "Не вдалося записати дані:\n" + error
        );
        return;
    }

    if (!file.commit()) {
        QMessageBox::critical(
            this,
            "Помилка збереження",
            "Не вдалося завершити збереження:\n"
                + file.errorString()
        );
        return;
    }

    statusBar()->showMessage(
        "Результат збережено: " + fileName
    );
}