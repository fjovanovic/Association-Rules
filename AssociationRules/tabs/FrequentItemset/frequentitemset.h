#ifndef FREQUENTITEMSET_H
#define FREQUENTITEMSET_H

#include <QString>
#include <QCoreApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QMap>
#include <QTextEdit>
#include <QDesktopServices>


class FrequentItemset
{
public:
    // Constructors
    FrequentItemset();

    // Destructors
    ~FrequentItemset();

    // Getters
    QString getInputFilePath();
    QString getOutputFilePath();

    // Slots
    QString onBrowseButtonClicked();
    QString onChangeButtonClicked();
    void onRunAlgorithmButtonClicked(QGraphicsScene *scene, const double minSupport);
    void onDrawTransactionButtonClicked(QGraphicsScene *scene);
    void onDrawFullTreeButtonClicked(QGraphicsScene *scene);
    void onForwardButtonClicked(QGraphicsScene *scene);
    void onOpenOutputFileButtonClicked();

private:
    // Fields
    QString _inputFilePath;
    QString _outputFilePath;
    QString _inputOpenFilePath;
    QString _outputOpenFilePath;
    int _nodeRadius;
    int _maxWidth;
    int _currentTrIndex;
    int _levelHeight;
    QPointF _rootPos;
    bool _treeBuilt;
    QTextEdit *_editor;
    QMap<QVector<int>, int> _nodesSupport;
    QMap<QVector<int>, int> _nodesSupportDown;
    QVector<QVector<int>> _transactions;
    QMap<int, int> _itemsFrequencies;
    QMap<QVector<int>, int> _setsFrequencies;
    QVector<QVector<int>> _sortedTransactions;
    QMap<QVector<int>, QVector<QVector<int>>> _childrenMap;
    QMap<QVector<int>, QVector<QVector<int>>> _childrenMapUp;
    QMap<QVector<int>, QPointF> _nodePositions;
    QVector<QGraphicsEllipseItem*> _pendingRemovalEllipses;
    bool _removalColoring;
    double _minSupport;
    QMap<QVector<int>, int> _frequentItemsets;
    QMap<int, QString> _itemMap;
    QMap<int, int> _widths;
    QMap<QVector<int>, QVector<int>> _nodePointers;     // NEW

    // Functions
    bool readFile();
    void findItemFrequencies();
    void sortTransactions();
    void findSetsFrequencies();
    void removeRareItemsets();
    void findChildren();
    bool childExists(const QVector<QVector<int>> &childrenList, const QVector<int> &child);
    void configTree(QGraphicsScene *scene);
    void saveFile(const QVector<QVector<int>> &frequentItemsets);
    void drawItemsLegend(QGraphicsScene *scene);
    void clearTransactionsLegend(QGraphicsScene *scene);
    void configNodePointers();
    void drawNodesPointers(QGraphicsScene *scene, const QString path);
};


#endif // FREQUENTITEMSET_H
