#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Association Rules");

    _gridTab = new Grid();
    MainWindow::gridConfig();
    _frequentItemsetTab = new FrequentItemset();
    MainWindow::frequentItemsetConfig();

    connect(ui->gridBrowseButton, &QPushButton::clicked, this, &MainWindow::gridOnBrowseButtonClicked);
    connect(ui->gridChangeButton, &QPushButton::clicked, this, &MainWindow::gridOnChangeButtonClicked);
    connect(ui->gridRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::gridOnRunAlgorithmButtonClicked);

    connect(ui->freqBrowseButton, &QPushButton::clicked, this, &MainWindow::freqOnBrowseButtonClicked);
    connect(ui->freqChangeButton, &QPushButton::clicked, this, &MainWindow::freqOnChangeButtonClicked);
    connect(ui->freqRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::freqOnRunAlgorithmButtonClicked);
    connect(ui->freqForwardButton, &QPushButton::clicked, this, &MainWindow::freqOnForwardButtonClicked);
    connect(ui->pbChooseVec1, &QPushButton::clicked, this, &MainWindow::pbChooseVector1);
    connect(ui->pbChooseVec2, &QPushButton::clicked, this, &MainWindow::pbChooseVector2);
    connect(ui->pbCompute, &QPushButton::clicked, this, &MainWindow::pbCompute);
}


void MainWindow::gridConfig()
{
    QString inputFilePath = _gridTab->getInputFilePath();
    QString outputFilePath = _gridTab->getOutputFilePath();
    ui->gridInputFileLine->setText(inputFilePath);
    ui->gridOutputFileLine->setText(outputFilePath);

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


void MainWindow::gridOnChangeButtonClicked()
{
    QString filePath = _gridTab->onChangeButtonClicked();
    ui->gridOutputFileLine->setText(filePath);
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


void MainWindow::frequentItemsetConfig()
{
    QString inputFilePath = _frequentItemsetTab->getInputFilePath();
    QString outputFilePath = _frequentItemsetTab->getOutputFilePath();
    ui->freqInputFileLine->setText(inputFilePath);
    ui->freqOutputFileLine->setText(outputFilePath);

    QTableWidgetItem *headerItem1 = new QTableWidgetItem(QString("Parameter"), QTableWidgetItem::Type);
    ui->freqParametersTable->setHorizontalHeaderItem(0, headerItem1);

    QTableWidgetItem *headerItem2 = new QTableWidgetItem(QString("Value"), QTableWidgetItem::Type);
    ui->freqParametersTable->setHorizontalHeaderItem(1, headerItem2);

    QTableWidgetItem *headerItem3 = new QTableWidgetItem(QString("Example"), QTableWidgetItem::Type);
    ui->freqParametersTable->setHorizontalHeaderItem(2, headerItem3);

    QTableWidgetItem *headerItem4 = new QTableWidgetItem(QString(""), QTableWidgetItem::Type);
    ui->freqParametersTable->setItem(0, 1, headerItem4);

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

    _frequentItemsetScene = new QGraphicsScene();
    ui->freqGraphicsView->setScene(_frequentItemsetScene);
}

double MainWindow::minkowskiDistance(const QVector<double> &vec1, const QVector<double> &vec2, double p)
{
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same dimension.");
    }

    double sum = 0.0;
    for (int i = 0; i < vec1.size(); ++i) {
        sum += std::pow(std::abs(vec1[i] - vec2[i]), p);

    }
    return std::pow(sum, 1.0 / p);
}

double MainWindow::mahalanobisDistance(const QVector<double> &vec1, const QVector<double> &vec2, const QVector<QVector<double> > &covMatrix)
{
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same dimension.");
    }
    if (covMatrix.size() != vec1.size() || covMatrix[0].size() != vec1.size()) {
        throw std::invalid_argument("Covariance matrix dimensions must match vector size.");
    }

    QVector<double> diff(vec1.size());
    for (int i = 0; i < vec1.size(); ++i) {
        diff[i] = vec1[i] - vec2[i];
    }

    QVector<double> temp(vec1.size(), 0.0);
    for (int i = 0; i < vec1.size(); ++i) {
        for (int j = 0; j < vec1.size(); ++j) {
            temp[i] += diff[j] * covMatrix[j][i];
        }
    }

    double distance = 0.0;
    for (int i = 0; i < vec1.size(); ++i) {
        distance += temp[i] * diff[i];
    }

    return std::sqrt(distance);
}

double MainWindow::cosineDistance(const QVector<double> &vec1, const QVector<double> &vec2)
{
    if (vec1.size() != vec2.size()) {
        qWarning() << "Vektori nisu iste duzine!";
        return -1.0;  // Greška
    }

    // Skalarni proizvod vektora
    double dotProduct = 0.0;
    for (int i = 0; i < vec1.size(); ++i) {
        dotProduct += vec1[i] * vec2[i];
    }

    // Norme vektora
    double normVec1 = 0.0;
    double normVec2 = 0.0;
    for (int i = 0; i < vec1.size(); ++i) {
        normVec1 += vec1[i] * vec1[i];
        normVec2 += vec2[i] * vec2[i];
    }

    normVec1 = std::sqrt(normVec1);
    normVec2 = std::sqrt(normVec2);

    // Kosinusna sličnost
    double cosineSimilarity = dotProduct / (normVec1 * normVec2);

    // Kosinusno rastojanje
    double cosineDistance = 1.0 - cosineSimilarity;

    return cosineDistance;
}

int MainWindow::hammingDistance(const QVector<double> &vec1, const QVector<double> &vec2)
{
    if (vec1.size() != vec2.size()) {
        qWarning() << "Vektori nisu iste duzine!";
        return -1;  // Greška
    }

    int distance = 0;
    for (int i = 0; i < vec1.size(); ++i) {
        // Pretpostavljamo da su vrednosti 0 ili 1, pa samo proveravamo razliku
        if (vec1[i] != vec2[i]) {
            distance++;  // Povećaj rastojanje ako su vrednosti različite
        }
    }

    return distance;
}

QVector<double> MainWindow::parseVector(const QString &filePath)
{
    QVector<double> vector;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error opening file:" << filePath;
        return vector;
    }

    QTextStream stream(&file);
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList numbers = line.split(",");
        for (const QString& num : numbers) {
            bool ok;
            double value = num.toDouble(&ok);
            qDebug()<<value;
            if (ok) {
                vector.append(value);
            } else {
                qWarning() << "Failed to parse number:" << num;
            }
        }
    }

    file.close();
    return vector;
}


void MainWindow::freqOnBrowseButtonClicked()
{
    QString filePath = _frequentItemsetTab->onBrowseButtonClicked();
    ui->freqInputFileLine->setText(filePath);
}


void MainWindow::freqOnChangeButtonClicked()
{
    QString filePath = _frequentItemsetTab->onChangeButtonClicked();
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

            _frequentItemsetTab->onRunAlgorithmButtonClicked(_frequentItemsetScene, minSup);
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
    setCursor(Qt::WaitCursor);
    _frequentItemsetTab->onForwardButtonClicked(_frequentItemsetScene);
    setCursor(Qt::ArrowCursor);
}

void MainWindow::pbChooseVector1()
{
    QString filePath = _frequentItemsetTab->onBrowseButtonClicked();
    ui->leVec1->setText(filePath);
}

void MainWindow::pbChooseVector2()
{
    QString filePath = _frequentItemsetTab->onBrowseButtonClicked();
    ui->leVec2->setText(filePath);
}

void MainWindow::pbCompute()
{
    QString dist = ui->comboBox->currentText();
    QString path1 = ui->leVec1->text();
    QString path2 = ui->leVec2->text();
    QVector<double> vec1 = MainWindow::parseVector(path1);
    QVector<double> vec2 = MainWindow::parseVector(path2);
    ui->label->setText(QString::number(vec1[0]));
    double p = ui->leParam->text().toDouble();
    if (dist == "Minkowski Distance"){
        double res = MainWindow::minkowskiDistance(vec1, vec2, p);
        ui->leRes->setText(QString::number(res));
    }
    if (dist == "Mahalanobis Distance"){

    }
    if (dist == "Cosine Distance"){
        double res = MainWindow::cosineDistance(vec1, vec2);
        ui->leRes->setText(QString::number(res));
    }
    if (dist == "Hamming Distance"){
        int res = MainWindow::hammingDistance(vec1, vec2);
        ui->leRes->setText(QString::number(res));

    }

}


MainWindow::~MainWindow()
{
    delete ui;
    delete _gridTab;
    delete _gridScene;
    delete _frequentItemsetTab;
    delete _frequentItemsetScene;
}
