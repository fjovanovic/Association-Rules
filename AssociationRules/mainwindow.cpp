#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    gridTab = new Grid();
    MainWindow::gridConfig();

    connect(ui->gridBrowseButton, &QPushButton::clicked, this, &MainWindow::gridOnBrowseButtonClicked);
    connect(ui->gridChangeButton, &QPushButton::clicked, this, &MainWindow::gridOnChangeButtonClicked);
    connect(ui->gridRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::gridOnRunAlgorithmClicked);
}


void MainWindow::gridConfig()
{
    QString inputFilePath = gridTab->getInputFilePath();
    QString outputFilePath = gridTab->getOutputFilePath();
    ui->gridInputFileLine->setText(inputFilePath);
    ui->gridOutputFileLine->setText(outputFilePath);

    QTableWidgetItem *headerItem1 = new QTableWidgetItem(QString("Parameter"), QTableWidgetItem::Type);
    ui->gridParametersTable->setHorizontalHeaderItem(0, headerItem1);

    QTableWidgetItem *headerItem2 = new QTableWidgetItem(QString("Value"), QTableWidgetItem::Type);
    ui->gridParametersTable->setHorizontalHeaderItem(1, headerItem2);

    QTableWidgetItem *headerItem3 = new QTableWidgetItem(QString("Example"), QTableWidgetItem::Type);
    ui->gridParametersTable->setHorizontalHeaderItem(2, headerItem3);

    QTableWidgetItem *parameterCell = ui->gridParametersTable->item(0, 0);
    if(parameterCell) {
        parameterCell->setFlags(parameterCell->flags() & ~Qt::ItemIsEditable);
    } else {
        QMessageBox::critical(this, "Error", "Problem with getting parameter cell");
    }

    QTableWidgetItem *exampleCell = ui->gridParametersTable->item(0, 2);
    if(exampleCell) {
        exampleCell->setFlags(exampleCell->flags() & ~Qt::ItemIsEditable);
    } else {
        QMessageBox::critical(this, "Error", "Problem with getting example cell");
    }

    gridScene = new QGraphicsScene();
    ui->gridGraphicsView->setScene(gridScene);

    ui->gridParametersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    int rowCount = ui->gridParametersTable->rowCount();
    int columnCount = ui->gridParametersTable->columnCount();

    for(int row = 0; row < rowCount; row++) {
        for(int col = 0; col < columnCount; col++) {
            QTableWidgetItem *item = ui->gridParametersTable->item(row, col);
            if(item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
}


void MainWindow::gridOnBrowseButtonClicked()
{
    QString filePath = gridTab->onBrowseButtonClicked(this);
    ui->gridInputFileLine->setText(filePath);
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
            QMessageBox::critical(this, "Error", "Minimum support not entered");
            return;
        }

        bool success;
        float minSup = cellText.toFloat(&success);

        if(success) {
            if(minSup <= 0 || minSup > 1) {
                QMessageBox::critical(this, "Error", "Minimum support must be in scope (0, 1]");
                return;
            }
            gridTab->onRunAlgorithmClicked(this, minSup);
        } else {
            QMessageBox::critical(this, "Error", "Minimum support must be number (float or integer)");
        }
    } else {
        QMessageBox::critical(this, "Error", "Problem with getting parameter cell");
    }
}


MainWindow::~MainWindow()
{
    delete ui;
    delete gridTab;
    delete gridScene;
}
