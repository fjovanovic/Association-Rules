#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <cmath>
#include <stdexcept>

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
    connect(ui->gridChangeFrequentOutputButton, &QPushButton::clicked, this, &MainWindow::gridOnChangeFrequentButtonClicked);
    connect(ui->gridChangeRareOutputButton, &QPushButton::clicked, this, &MainWindow::gridOnChangeRareButtonClicked);
    connect(ui->gridRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::gridOnRunAlgorithmButtonClicked);
    connect(ui->gridFrequentItemsButton, &QPushButton::clicked, this, &MainWindow::gridOnFrequentItemsButtonClicked);
    connect(ui->gridRareItemsButton, &QPushButton::clicked, this, &MainWindow::gridOnRareItemsButtonClicked);
    connect(ui->gridScreenshotButton, &QPushButton::clicked, this, &MainWindow::gridOnScreenshotButtonClicked);

    connect(ui->freqBrowseButton, &QPushButton::clicked, this, &MainWindow::freqOnBrowseButtonClicked);
    connect(ui->freqChangeButton, &QPushButton::clicked, this, &MainWindow::freqOnChangeButtonClicked);
    connect(ui->freqRunAlgorithmButton, &QPushButton::clicked, this, &MainWindow::freqOnRunAlgorithmButtonClicked);
    connect(ui->freqDrawTransactionButton, &QPushButton::clicked, this, &MainWindow::freqOnDrawTransactionButtonClicked);
    connect(ui->freqDrawFullTreeButton, &QPushButton::clicked, this, &MainWindow::freqOnDrawFullTreeButtonClicked);
    connect(ui->freqForwardButton, &QPushButton::clicked, this, &MainWindow::freqOnForwardButtonClicked);
    connect(ui->pbChooseVec1, &QPushButton::clicked, this, &MainWindow::pbChooseVector1);
    connect(ui->pbChooseVec2, &QPushButton::clicked, this, &MainWindow::pbChooseVector2);
    connect(ui->pbCompute, &QPushButton::clicked, this, &MainWindow::pbCompute);
    connect(ui->pbChooseApr, &QPushButton::clicked, this, &MainWindow::pbChooseApr);
    connect(ui->pbFindRare, &QPushButton::clicked, this, &MainWindow::pbFindRare);
    connect(ui->pbChooseOutput, &QPushButton::clicked, this, &MainWindow::pbChooseOutput);
    connect(ui->freqOpenOutputFileButton, &QPushButton::clicked, this, &MainWindow::freqOnOpenOutputFileButtonClicked);
    connect(ui->freqScreenshotButton, &QPushButton::clicked, this, &MainWindow::freqOnScreenshotButtonClicked);
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


void MainWindow::gridOnScreenshotButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->gridScreenshotButton->setDisabled(true);
    QCoreApplication::processEvents();

    QWidget *mainWindow = this->window();
    _gridTab->onScreenshotButtonClicked(mainWindow);

    setCursor(Qt::ArrowCursor);
    ui->gridScreenshotButton->setDisabled(false);
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

double MainWindow::mahalanobisDistance(const QVector<double> &vec1, const QVector<double> &vec2)
{
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must have the same dimension.");
    }

    int n = vec1.size();

    // 1. Napravi 2xN matricu od vektora
    QVector<QVector<double>> data = {vec1, vec2};

    // 2. Izračunaj srednju vrednost po koloni
    QVector<double> mean(n, 0.0);
    for (int i = 0; i < n; ++i) {
        mean[i] = (vec1[i] + vec2[i]) / 2.0;
    }

    // 3. Izračunaj kovarijacionu matricu (n x n)
    QVector<QVector<double>> covMatrix(n, QVector<double>(n, 0.0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < 2; ++k) {  // 2 vektora
                sum += (data[k][i] - mean[i]) * (data[k][j] - mean[j]);
            }
            covMatrix[i][j] = sum / 1.0;  // delimo sa (2 - 1) = 1
        }
    }

    // 4. Inverzija kovarijacione matrice (pretpostavimo dijagonalnu za jednostavnost)
    QVector<QVector<double>> invCovMatrix(n, QVector<double>(n, 0.0));
    for (int i = 0; i < n; ++i) {
        if (covMatrix[i][i] == 0.0) {
            throw std::runtime_error("Covariance matrix is singular.");
        }
        invCovMatrix[i][i] = 1.0 / covMatrix[i][i];
    }

    // 5. diff = vec1 - vec2
    QVector<double> diff(n);
    for (int i = 0; i < n; ++i) {
        diff[i] = vec1[i] - vec2[i];
    }

    // 6. temp = invCovMatrix * diff
    QVector<double> temp(n, 0.0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            temp[i] += invCovMatrix[i][j] * diff[j];
        }
    }

    // 7. distance = sqrt(diff^T * temp)
    double distance = 0.0;
    for (int i = 0; i < n; ++i) {
        distance += diff[i] * temp[i];
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

double MainWindow::jaccardCoefficient(const QVector<double> &vec1, const QVector<double> &vec2)
{
    if (vec1.size() != vec2.size()) {
        qWarning() << "Vektori nisu iste dužine!";
            return -1.0;  // Greška
    }

    int m11 = 0; // Broj pozicija gde su oba 1
    int m10 = 0; // Broj pozicija gde je vec1=1, vec2=0
    int m01 = 0; // Broj pozicija gde je vec1=0, vec2=1

    for (int i = 0; i < vec1.size(); ++i) {
        if (vec1[i] == 1.0 && vec2[i] == 1.0) {
            m11++;
        } else if (vec1[i] == 1.0 && vec2[i] == 0.0) {
            m10++;
        } else if (vec1[i] == 0.0 && vec2[i] == 1.0) {
            m01++;
        }
        // (0, 0) se ignoriše
    }

    int denominator = m11 + m10 + m01;
    if (denominator == 0) {
        return 0.0;  // Da se izbegne deljenje nulom
    }

    return static_cast<double>(m11) / denominator;
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
#include <QtAlgorithms>
uint qHash(const QSet<int> &key, uint seed = 0) {
    uint result = seed;
    for (int val : key)
        result ^= qHash(val, seed);
    return result;
}
bool containsAll(const QSet<int> &transaction, const QSet<int> &itemset) {
    for (int x : itemset)
        if (!transaction.contains(x))
            return false;
    return true;
}

bool containsSet(const QVector<QSet<int>> &list, const QSet<int> &set) {
    for (const QSet<int> &s : list) {
        if (s == set)
            return true;
    }
    return false;
}

// Provera da li su svi (k-1)-podskupovi kandidata retki
bool allSubsetsRare(const QSet<int> &candidate, const QVector<QSet<int>> &prevRare) {
    for (int item : candidate) {
        QSet<int> subset = candidate;
        subset.remove(item);
        if (!containsSet(prevRare, subset))
            return false;
    }
    return true;
}

void MainWindow::findRareItemsets(const QString &filename)
{
    int MIN_SUPPORT = ui->leMinSupp->text().toInt();
    QString output1 = ui->lePbOutputRare->text();
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Ne mogu da otvorim fajl!";
        return;
    }

    QTextStream in(&file);
    QVector<QSet<int>> transactions;
    QSet<int> allItems;

    // 1. Učitavanje transakcija
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList items = line.split(" ", Qt::SkipEmptyParts);
        QSet<int> transaction;
        for (const QString &item : items) {
            int value = item.toInt();
            transaction.insert(value);
            allItems.insert(value);
        }
        transactions.append(transaction);
    }
    file.close();

    QList<int> allItemList = allItems.values();
    std::sort(allItemList.begin(), allItemList.end());

    QVector<QSet<int>> prevRare;    // Retki skupovi veličine k-1
    QVector<QSet<int>> currentRare; // Retki skupovi veličine k
    QHash<QSet<int>, int> supportCount;

    // --- Iteracija k = 1 (pojedinačne stavke) ---
    for (int item : allItemList) {
        QSet<int> candidate;
        candidate.insert(item);

        int count = 0;
        for (const QSet<int> &t : transactions)
            if (containsAll(t, candidate))
            {
                ++count;
               // qDebug()<<candidate<<"JUHUU";
            }
        qDebug()<<candidate<<" "<<count<<" "<<MIN_SUPPORT<<" -----------";
        if (count < MIN_SUPPORT) {
//qDebug()<<candidate<<"sssssssssss   "<<count;
            prevRare.append(candidate);
            supportCount[candidate] = count;
        }
    }
   // for(auto& x: prevRare)
       // qDebug()<<x<<"-----------";
    // Iterativno generisanje retkih skupova veličine k >= 2
    int k = 2;
    while (!prevRare.isEmpty()) {
        currentRare.clear();

        // Generiši k-kandidate spajanjem retkih (k-1)-skupova
        int size = prevRare.size();

        for (int i = 0; i < size; ++i) {
            for (int j = i+1; j < size; ++j) {
                QList<int> items1 = prevRare[i].values();
                QList<int> items2 = prevRare[j].values();

                std::sort(items1.begin(), items1.end());
                std::sort(items2.begin(), items2.end());

                // Proveri da li prve k-2 stavke jednaki
                bool canJoin = true;
                for (int idx = 0; idx < k-2; ++idx) {
                    if (items1[idx] != items2[idx]) {
                        canJoin = false;
                        break;
                    }
                }
                if (!canJoin)
                    continue;

                // Kreiraj kandidat k-skupa (ujedini)
                QSet<int> candidate = prevRare[i];
                candidate.unite(prevRare[j]);

                // Pruning: proveri da li su svi (k-1) podskupovi kandidata retki
                if (!allSubsetsRare(candidate, prevRare))
                    continue;

                // Proveri podršku kandidata
                int count = 0;
                for (const QSet<int> &t : transactions)
                    if (containsAll(t, candidate)) ++count;

                if (count > 0 && count < MIN_SUPPORT) {
                    currentRare.append(candidate);
                    supportCount[candidate] = count;
                }
            }
        }

        prevRare = currentRare;
        k++;
    }

    // Ispis svih retkih skupova
    qDebug() << "Retki skupovi stavki (pojavljuju se manje od" << MIN_SUPPORT << "puta):";
    for (auto it = supportCount.begin(); it != supportCount.end(); ++it) {
        qDebug() << it.key() << " -> Pojavljivanja:" << it.value();
    }
    QFile outputFile(output1);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Ne mogu da otvorim fajl za pisanje rezultata:" << output1;
        return;
    }
    QTextStream out(&outputFile);

    out << "Retki skupovi stavki (pojavljuju se manje od " << MIN_SUPPORT << " puta):\n";

    for (auto it = supportCount.begin(); it != supportCount.end(); ++it) {
        QString setStr;
        for (int item : it.key()) {
            setStr += QString::number(item) + " ";
        }
        setStr = setStr.trimmed();

        QString line = QString("%1 -> Pojavljivanja: %2").arg(setStr).arg(it.value());
        qDebug() << line;
        out << line << "\n";
    }

    outputFile.close();
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


void MainWindow::freqOnDrawTransactionButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->freqDrawTransactionButton->setDisabled(true);
    QCoreApplication::processEvents();

    _frequentItemsetTab->onDrawTransactionButtonClicked(_frequentItemsetScene);

    setCursor(Qt::ArrowCursor);
    ui->freqDrawTransactionButton->setDisabled(false);
}


void MainWindow::freqOnDrawFullTreeButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->freqDrawFullTreeButton->setDisabled(true);
    QCoreApplication::processEvents();

    _frequentItemsetTab->onDrawFullTreeButtonClicked(_frequentItemsetScene);

    setCursor(Qt::ArrowCursor);
    ui->freqDrawFullTreeButton->setDisabled(false);
}


void MainWindow::freqOnForwardButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->freqForwardButton->setDisabled(true);
    QCoreApplication::processEvents();

    _frequentItemsetTab->onForwardButtonClicked(_frequentItemsetScene);

    setCursor(Qt::ArrowCursor);
    ui->freqForwardButton->setDisabled(false);
}


void MainWindow::freqOnOpenOutputFileButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->freqOpenOutputFileButton->setDisabled(true);
    QCoreApplication::processEvents();

    _frequentItemsetTab->onOpenOutputFileButtonClicked();

    setCursor(Qt::ArrowCursor);
    ui->freqOpenOutputFileButton->setDisabled(false);
}


void MainWindow::freqOnScreenshotButtonClicked()
{
    setCursor(Qt::WaitCursor);
    ui->freqScreenshotButton->setDisabled(true);
    QCoreApplication::processEvents();

    QWidget *mainWindow = this->window();
    _frequentItemsetTab->onScreenshotButtonClicked(mainWindow);

    setCursor(Qt::ArrowCursor);
    ui->freqScreenshotButton->setDisabled(false);
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
        int dim = vec1.size();

        // Kreiranje identitetske matrice dim x dim
        QVector<QVector<double>> identityMatrix(dim, QVector<double>(dim, 0.0));
        for (int i = 0; i < dim; ++i) {
            identityMatrix[i][i] = 1.0;
        }
        double res = MainWindow::mahalanobisDistance(vec1, vec2);
        ui->leRes->setText(QString::number(res));
    }
    if (dist == "Cosine Distance"){
        double res = MainWindow::cosineDistance(vec1, vec2);
        ui->leRes->setText(QString::number(res));
    }
    if (dist == "Hamming Distance"){
        int res = MainWindow::hammingDistance(vec1, vec2);
        ui->leRes->setText(QString::number(res));

    }
    if (dist == "Jaccard coefficient"){
    int res = MainWindow::jaccardCoefficient(vec1, vec2);
    ui->leRes->setText(QString::number(res));

    }
}

void MainWindow::pbChooseApr()
{
    QString filePath = _frequentItemsetTab->onBrowseButtonClicked();
    ui->leApr->setText(filePath);
}

void MainWindow::pbFindRare()
{
    MainWindow::findRareItemsets(ui->leApr->text());
}

void MainWindow::pbChooseOutput()
{
    QString filePath = _frequentItemsetTab->onBrowseButtonClicked();
    ui->lePbOutputRare->setText(filePath);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete _gridTab;
    delete _gridScene;
    delete _frequentItemsetTab;
    delete _frequentItemsetScene;
}
