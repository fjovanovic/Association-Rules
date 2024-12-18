#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    frequentItemsetTab = new FrequentItemset();
    MainWindow::frequentItemsetConfig();

    connect(ui->freqBrowseButton, &QPushButton::clicked, this, &MainWindow::freqOnBrowseButtonClicked);
    connect(ui->freqChangeButton, &QPushButton::clicked, this, &MainWindow::freqOnChangeButtonClicked);
    connect(ui->freqRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::freqOnRunAlgorithmButtonClicked);
    connect(ui->freqForwardButton, &QPushButton::clicked, this, &MainWindow::freqOnForwardButtonClicked);
    connect(ui->freqBackButton, &QPushButton::clicked, this, &MainWindow::freqOnBackButtonClicked);
}


void MainWindow::frequentItemsetConfig()
{
    QString inputFilePath = frequentItemsetTab->getInputFilePath();
    QString outputFilePath = frequentItemsetTab->getOutputFilePath();
    ui->freqInputFileLine->setText(inputFilePath);
    ui->freqOutputFileLine->setText(outputFilePath);

    QTableWidgetItem *headerItem1 = new QTableWidgetItem(QString("Parameter"), QTableWidgetItem::Type);
    ui->freqParametersTable->setHorizontalHeaderItem(0, headerItem1);

    QTableWidgetItem *headerItem2 = new QTableWidgetItem(QString("Value"), QTableWidgetItem::Type);
    ui->freqParametersTable->setHorizontalHeaderItem(1, headerItem2);

    QTableWidgetItem *headerItem3 = new QTableWidgetItem(QString("Example"), QTableWidgetItem::Type);
    ui->freqParametersTable->setHorizontalHeaderItem(2, headerItem3);

    QTableWidgetItem *parameterCell = ui->freqParametersTable->item(0, 0);
    if(parameterCell) {
        parameterCell->setFlags(parameterCell->flags() & ~Qt::ItemIsEditable);
    } else {
        QMessageBox::critical(this, "Error", "Problem with getting parameter cell");
    }

    QTableWidgetItem *exampleCell = ui->freqParametersTable->item(0, 2);
    if(exampleCell) {
        exampleCell->setFlags(exampleCell->flags() & ~Qt::ItemIsEditable);
    } else {
        QMessageBox::critical(this, "Error", "Problem with getting example cell");
    }

    ui->freqParametersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    int rowCount = ui->freqParametersTable->rowCount();
    int columnCount = ui->freqParametersTable->columnCount();

    for(int row = 0; row < rowCount; row++) {
        for(int col = 0; col < columnCount; col++) {
            QTableWidgetItem *item = ui->freqParametersTable->item(row, col);
            if(item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

}


void MainWindow::freqOnBrowseButtonClicked()
{
    QString filePath = frequentItemsetTab->onBrowseButtonClicked();
    ui->freqInputFileLine->setText(filePath);
}


void MainWindow::freqOnChangeButtonClicked()
{
    QString filePath = frequentItemsetTab->onChangeButtonClicked();
    ui->freqOutputFileLine->setText(filePath);
}


void MainWindow::freqOnRunAlgorithmButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->freqRunAlgorithmButton->setDisabled(true);
    QCoreApplication::processEvents();

    QTableWidgetItem *minSupCell = ui->freqParametersTable->item(0, 1);
    if(minSupCell) {
        QString cellText = minSupCell->text().trimmed().replace(",", ".");
        if(cellText == "") {
            QMessageBox::critical(this, "Error", "Minimum support not entered");

            setCursor(Qt::ArrowCursor);
            ui->freqRunAlgorithmButton->setDisabled(false);

            return;
        }

        bool success;
        float minSup = cellText.toFloat(&success);

        if(success) {
            if(minSup <= 0 || minSup > 1) {
                QMessageBox::critical(this, "Error", "Minimum support must be in scope (0, 1]");

                setCursor(Qt::ArrowCursor);
                ui->freqRunAlgorithmButton->setDisabled(false);

                return;
            }

            if(ui->freqGraphicsView->scene() != nullptr) {
                delete ui->freqGraphicsView->scene();
            }

            frequentItemsetScene = new QGraphicsScene();
            frequentItemsetScene->setSceneRect(0, 0, 800, 600);
            ui->freqGraphicsView->setScene(frequentItemsetScene);
            ui->freqGraphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);

            frequentItemsetTab->onRunAlgorithmButtonClicked(frequentItemsetScene, minSup);
        } else {
            QMessageBox::critical(this, "Error", "Minimum support must be number (float or integer)");
        }
    } else {
        QMessageBox::critical(this, "Error", "Minimum support not entered");
    }

    setCursor(Qt::ArrowCursor);
    ui->freqRunAlgorithmButton->setDisabled(false);
}


void MainWindow::freqOnForwardButtonClicked()
{
    // TODO
}


void MainWindow::freqOnBackButtonClicked()
{
    // TODO
}


MainWindow::~MainWindow()
{
    delete ui;
    delete frequentItemsetTab;
    delete frequentItemsetScene;
}
