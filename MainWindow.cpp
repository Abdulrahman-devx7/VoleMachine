#include "MainWindow.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QAbstractItemView>
#include <QRegularExpression>
#include <QStringList>

static QString hexValue(unsigned value, int width)
{
    return QString("%1").arg(value, width, 16, QLatin1Char('0')).toUpper();
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), pcLabel(new QLabel), irLabel(new QLabel),
    registersTable(new QTableWidget(4, 4)), memoryTable(new QTableWidget(16, 16)), screenOutput(new QTextEdit),
    screenModeSelector(new QComboBox)
{
    setWindowTitle("Vole Machine Simulator");
    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    auto* buttons = new QHBoxLayout;
    auto* loadButton = new QPushButton("Load Program");
    auto* stepButton = new QPushButton("Step");
    auto* runButton = new QPushButton("Run");
    auto* stopButton = new QPushButton("Stop");
    auto* resetButton = new QPushButton("Reset");
    buttons->addWidget(loadButton); buttons->addWidget(stepButton); buttons->addWidget(runButton);
    buttons->addWidget(stopButton); buttons->addWidget(resetButton); root->addLayout(buttons);

    auto* state = new QHBoxLayout;
    state->addWidget(new QLabel("PC:")); state->addWidget(pcLabel);
    state->addSpacing(20); state->addWidget(new QLabel("IR:")); state->addWidget(irLabel);
    state->addStretch(); root->addLayout(state);

    auto* tables = new QHBoxLayout;
    auto* regGroup = new QGroupBox("Registers");
    auto* regLayout = new QVBoxLayout(regGroup); regLayout->addWidget(registersTable);
    auto* memGroup = new QGroupBox("Memory (hex addresses)");
    auto* memLayout = new QVBoxLayout(memGroup); memLayout->addWidget(memoryTable);
    tables->addWidget(regGroup, 1); tables->addWidget(memGroup, 3); root->addLayout(tables);
    screenOutput->setReadOnly(true);
    screenModeSelector->addItems({"Text", "Hexadecimal", "Decimal"});
    auto* outputGroup = new QGroupBox("Screen Output");
    auto* outputLayout = new QVBoxLayout(outputGroup);
    auto* outputModeLayout = new QHBoxLayout;
    outputModeLayout->addWidget(new QLabel("Display mode:"));
    outputModeLayout->addWidget(screenModeSelector);
    outputModeLayout->addStretch();
    outputLayout->addLayout(outputModeLayout);
    outputLayout->addWidget(screenOutput);
    root->addWidget(outputGroup);
    setCentralWidget(central);

    for (int i = 0; i < 16; ++i) {
        registersTable->setItem(i / 4, i % 4, new QTableWidgetItem);
        registersTable->item(i / 4, i % 4)->setTextAlignment(Qt::AlignCenter);
    }
    registersTable->horizontalHeader()->hide(); registersTable->verticalHeader()->hide();
    for (int i = 0; i < 256; ++i) {
        memoryTable->setItem(i / 16, i % 16, new QTableWidgetItem);
        memoryTable->item(i / 16, i % 16)->setTextAlignment(Qt::AlignCenter);
    }
    memoryTable->horizontalHeader()->hide(); memoryTable->verticalHeader()->hide();
    memoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    registersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadProgram);
    connect(stepButton, &QPushButton::clicked, this, &MainWindow::step);
    connect(runButton, &QPushButton::clicked, this, &MainWindow::run);
    connect(stopButton, &QPushButton::clicked, &timer, &QTimer::stop);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::reset);
    connect(screenModeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateDisplay);
    connect(&timer, &QTimer::timeout, this, &MainWindow::step);
    timer.setInterval(100);
    updateDisplay();
}

void MainWindow::loadProgram()
{
    const QString path = QFileDialog::getOpenFileName(this, "Load Vole Program", QString(), "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) return;
    bool ok = false;
    const QString addressText = QInputDialog::getText(this, "Load Address", "Starting address (hexadecimal, 00–FF):",
                                                       QLineEdit::Normal, "00", &ok).trimmed();
    if (!ok) return;
    static const QRegularExpression addressPattern("^(?:0[xX])?[0-9A-Fa-f]{1,2}$");
    if (!addressPattern.match(addressText).hasMatch()) {
        QMessageBox::warning(this, "Invalid address", "Enter a hexadecimal address from 00 to FF.");
        return;
    }
    QString digits = addressText;
    if (digits.startsWith("0x", Qt::CaseInsensitive)) digits.remove(0, 2);
    const int address = digits.toInt(&ok, 16);
    if (!ok) return;
    timer.stop();
    std::string error;
    if (!machine.LoadProgramFile(path.toStdString(), static_cast<uint8_t>(address), error))
        QMessageBox::warning(this, "Load failed", QString::fromStdString(error));
    updateDisplay();
}

void MainWindow::step()
{
    if (!machine.IsHalted()) machine.ExecuteOneStep();
    if (machine.IsHalted()) timer.stop();
    updateDisplay();
}

void MainWindow::run()
{
    if (machine.IsHalted()) return;
    machine.BeginExecution();
    timer.start();
}

void MainWindow::reset()
{
    timer.stop();
    machine.ResetMachine();
    updateDisplay();
}

void MainWindow::updateDisplay()
{
    pcLabel->setText("0x" + hexValue(machine.GetPC(), 2));
    irLabel->setText("0x" + hexValue(machine.GetIR(), 4));
    for (int i = 0; i < 16; ++i) registersTable->item(i / 4, i % 4)->setText("R" + hexValue(i, 1) + ": " + hexValue(machine.GetRegisterValue(i), 2));
    for (int i = 0; i < 256; ++i) memoryTable->item(i / 16, i % 16)->setText(hexValue(machine.GetMemoryValue(i), 2));
    const std::string output = machine.GetScreenOutput();
    if (screenModeSelector->currentIndex() == 1) {
        QStringList bytes;
        for (unsigned char byte : output) bytes.append(hexValue(byte, 2));
        screenOutput->setPlainText(bytes.join(' '));
    } else if (screenModeSelector->currentIndex() == 2) {
        QStringList values;
        for (unsigned char byte : output) values.append(QString::number(static_cast<unsigned int>(byte)));
        screenOutput->setPlainText(values.join(' '));
    } else {
        screenOutput->setPlainText(QString::fromLocal8Bit(output.data(), static_cast<int>(output.size())));
    }
}
