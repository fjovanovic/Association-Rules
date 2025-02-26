#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _gridTab = new Grid();
    MainWindow::gridConfig();

    connect(ui->gridBrowseButton, &QPushButton::clicked, this, &MainWindow::gridOnBrowseButtonClicked);
    connect(ui->gridChangeFrequentOutputButton, &QPushButton::clicked, this, &MainWindow::gridOnChangeFrequentButtonClicked);
    connect(ui->gridChangeRareOutputButton, &QPushButton::clicked, this, &MainWindow::gridOnChangeRareButtonClicked);
    connect(ui->gridRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::gridOnRunAlgorithmButtonClicked);
    connect(ui->gridFrequentItemsButton, &QPushButton::clicked, this, &MainWindow::gridOnFrequentItemsButtonClicked);
    connect(ui->gridRareItemsButton, &QPushButton::clicked, this, &MainWindow::gridOnRareItemsButtonClicked);
}


void MainWindow::gridConfig()
{
    QString inputFilePath = _gridTab->getInputFilePath();
    QString outputFrequentFilePath = _gridTab->getFrequentOutputFilePath();
    QString outputRareFilePath = _gridTab->getRareOutputFilePath();
    ui->gridInputFileLine->setText(inputFilePath);
    ui->gridFrequentOutputFileLine->setText(outputFrequentFilePath);
    ui->gridRareOutputFileLine->setText(outputRareFilePath);

    QTableWidgetItem *headerItem1 = new QTableWidgetItem(QString("Parameter"), QTableWidgetItem::Type);
    ui->gridParametersTable->setHorizontalHeaderItem(0, headerItem1);

    QTableWidgetItem *headerItem2 = new QTableWidgetItem(QString("Value"), QTableWidgetItem::Type);
    ui->gridParametersTable->setHorizontalHeaderItem(1, headerItem2);

    QTableWidgetItem *headerItem3 = new QTableWidgetItem(QString("Example"), QTableWidgetItem::Type);
    ui->gridParametersTable->setHorizontalHeaderItem(2, headerItem3);

    QTableWidgetItem *headerItem4 = new QTableWidgetItem(QString(""), QTableWidgetItem::Type);
    ui->gridParametersTable->setItem(0, 1, headerItem4);

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

    _gridScene = new QGraphicsScene();
    ui->gridGraphicsView->setScene(_gridScene);
    ui->gridGraphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


void MainWindow::gridOnBrowseButtonClicked()
{
    QString filePath = _gridTab->onBrowseButtonClicked();
    ui->gridInputFileLine->setText(filePath);
}


void MainWindow::gridOnChangeFrequentButtonClicked()
{
    QString filePath = _gridTab->onChangeFrequentButtonClicked();
    ui->gridFrequentOutputFileLine->setText(filePath);
}

void MainWindow::gridOnChangeRareButtonClicked()
{
    QString filePath = _gridTab->onChangeRareButtonClicked();
    ui->gridRareOutputFileLine->setText(filePath);
}


void MainWindow::gridOnRunAlgorithmButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->gridRunAlgorithmButton->setDisabled(true);
    QCoreApplication::processEvents();

    QTableWidgetItem *minSupCell = ui->gridParametersTable->item(0, 1);
    if(minSupCell) {
        QString cellText = minSupCell->text().trimmed().replace(",", ".");
        if(cellText == "") {
            QMessageBox::critical(this, "Error", "Minimum support not entered");

            setCursor(Qt::ArrowCursor);
            ui->gridRunAlgorithmButton->setDisabled(false);

            return;
        }

        bool success;
        float minSup = cellText.toFloat(&success);

        if(success) {
            if(minSup <= 0 || minSup > 1) {
                QMessageBox::critical(this, "Error", "Minimum support must be in scope (0, 1]");

                setCursor(Qt::ArrowCursor);
                ui->gridRunAlgorithmButton->setDisabled(false);

                return;
            }

            _gridTab->onRunAlgorithmButtonClicked(_gridScene, minSup);
        } else {
            QMessageBox::critical(this, "Error", "Minimum support must be number (float or integer)");
        }
    } else {
        QMessageBox::critical(this, "Error", "Minimum support not entered");
    }

    setCursor(Qt::ArrowCursor);
    ui->gridRunAlgorithmButton->setDisabled(false);
}


void MainWindow::gridOnFrequentItemsButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->gridFrequentItemsButton->setDisabled(true);
    QCoreApplication::processEvents();

    _gridTab->onFrequentItemsButtonClicked();

    setCursor(Qt::ArrowCursor);
    ui->gridFrequentItemsButton->setDisabled(false);
}


void MainWindow::gridOnRareItemsButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->gridRareItemsButton->setDisabled(true);
    QCoreApplication::processEvents();

    _gridTab->onRareItemsButtonClicked();

    setCursor(Qt::ArrowCursor);
    ui->gridRareItemsButton->setDisabled(false);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete _gridTab;
    delete _gridScene;
}
