#ifndef GRID_H
#define GRID_H

#include <QString>
#include <QCoreApplication>
#include <QMessageBox>
#include <QVector>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QPen>
#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QMap>
#include <QGuiApplication>
#include <QScreen>

#include <cmath>


class Grid
{
public:
    // Constructors
    Grid();

    // Getters
    QString getInputFilePath();
    QString getFrequentOutputFilePath();
    QString getRareOutputFilePath();

    // Slots
    QString onBrowseButtonClicked();
    QString onChangeFrequentButtonClicked();
    QString onChangeRareButtonClicked();
    void onRunAlgorithmButtonClicked(QGraphicsScene *scene, const double minSupport);
    void onFrequentItemsButtonClicked();
    void onRareItemsButtonClicked();
    void onScreenshotButtonClicked(QWidget *mainWindow);

private:
    // Fields
    int _gridWidth;
    QBrush _closedAndMaximalItemsetsBrush;
    QBrush _maximalItemsetsBrush;
    QBrush _closedItemsetsBrush;
    QBrush _frequentItemsetsBrush;
    QBrush _rareItemsetsBrush;
    QPen _textPen;
    QString _inputFilePath;
    QString _outputFrequentFilePath;
    QString _outputRareFilePath;
    QString _inputOpenFilePath;
    QString _outputOpenFilePath;
    QVector<QVector<int>> _transactions;
    QMap<int, QString> _itemMap;

    // Functions
    bool readFile(QSet<int> &gridItems);
    QVector<QVector<int>> findAllSets(const QSet<int> &gridItems);
    QVector<int> findAllSupports(const QVector<QVector<int>> &gridSets);
    QMap<QVector<int>, QVector<QVector<int>>> findChildren(QVector<QVector<int>> &gridSets);
    QMap<QVector<int>, int> generateFrequentItemsets(const double minSupport);
    QVector<QVector<int>> findClosedItemsets(const QMap<QVector<int>, int> &frequentItemsets);
    QVector<QVector<int>> findMaximalItemsets(const QMap<QVector<int>, int> &frequentItemsets);
    QVector<QVector<int>> findClosedAndMaximalItemsets(
        const QVector<QVector<int>> &closedItemsets,
        const QVector<QVector<int>> &maximalItemsets
    );
    void drawGrid(
        QGraphicsScene *scene,
        const QVector<QVector<int>> &gridSets,
        const QMap<QVector<int>, QVector<QVector<int>>> &childrenMap,
        const QVector<int> &gridSupports,
        const QVector<QVector<int>> &frequentKeys,
        const QVector<QVector<int>> &closedItemsets,
        const QVector<QVector<int>> &maximalItemsets,
        const QVector<QVector<int>> &closedAndMaximalItemsets
    );
    bool saveFile(
        const QVector<QVector<int>> &gridSets,
        const QVector<int> &gridSupports,
        const QMap<QVector<int>, int> &frequentItemsets,
        const QVector<QVector<int>> &closedItemsets,
        const QVector<QVector<int>> &maximalItemsets,
        const QVector<QVector<int>> &closedAndMaximalItemsets
    );
    QVector<QVector<int>> generateSubsets(const QVector<int> &set);
    QVector<QPair<QVector<int>, int>> sortBySetSize(const QMap<QVector<int>, int> &frequentItemsets);
};


#endif // GRID_H
