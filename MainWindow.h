#pragma once
#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include <QLabel>
#include <QTextEdit>
#include <QComboBox>
#include "VoleMachine.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
private slots:
    void loadProgram();
    void step();
    void run();
    void reset();
    void updateDisplay();
private:
    VoleMachine machine;
    QTimer timer;
    QLabel* pcLabel;
    QLabel* irLabel;
    QTableWidget* registersTable;
    QTableWidget* memoryTable;
    QTextEdit* screenOutput;
    QComboBox* screenModeSelector;
};
