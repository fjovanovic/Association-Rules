#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gridTab = new Grid(
        ui->gridParametersTable,
        ui->gridOutputFileLine,
        ui->gridGraphicsView
    );

    connect(ui->gridBrowseButton, &QPushButton::clicked, this, &MainWindow::gridOnBrowseButtonClicked);
    connect(ui->gridChangeButton, &QPushButton::clicked, this, &MainWindow::gridOnChangeButtonClicked);
    connect(ui->gridRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::gridOnRunAlgorithmClicked);
}


void MainWindow::gridOnBrowseButtonClicked()
{
    QString filePath = gridTab->onBrowseButtonClicked(this);
    ui->gridInputFileLine->setText(filePath);

    gridTab->loadFile(filePath);
}


void MainWindow::gridOnChangeButtonClicked()
{
    QString filePath = gridTab->onChangeButtonClicked(this);
    ui->gridOutputFileLine->setText(filePath);
}


void MainWindow::gridOnRunAlgorithmClicked()
{
    QTableWidgetItem *minSupCell = ui->gridParametersTable->item(0, 1);
    if(minSupCell) {
        QString cellText = minSupCell->text().trimmed();
        cellText.replace(",", ".");
        if(cellText == "") {
            QMessageBox::critical(nullptr, "Error", "Minimum support not entered");
            return;
        }

        bool success;
        float minSup = cellText.toFloat(&success);

        if(success) {
            if(minSup <= 0 || minSup > 1) {
                QMessageBox::critical(nullptr, "Error", "Minimum support must be in scope (0, 1]");
                return;
            }
            gridTab->onRunAlgorithmClicked(minSup);
        } else {
            QMessageBox::critical(nullptr, "Error", "Minimum support must be number (float or integer)");
        }
    } else {
        QMessageBox::critical(nullptr, "Error", "Problem with getting parameter cell");
    }
}


MainWindow::~MainWindow()
{
    delete ui;
    delete gridTab;
}
