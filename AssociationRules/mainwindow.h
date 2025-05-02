#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScreen>

#include "tabs/Grid/grid.h"
#include "tabs/FrequentItemset/frequentitemset.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void gridOnBrowseButtonClicked();
    void gridOnChangeFrequentButtonClicked();
    void gridOnChangeRareButtonClicked();
    void gridOnRunAlgorithmButtonClicked();
    void gridOnFrequentItemsButtonClicked();
    void gridOnRareItemsButtonClicked();
    void gridOnScreenshotButtonClicked();

    void freqOnBrowseButtonClicked();
    void freqOnChangeButtonClicked();
    void freqOnRunAlgorithmButtonClicked();
    void freqOnDrawTransactionButtonClicked();
    void freqOnDrawFullTreeButtonClicked();
    void freqOnForwardButtonClicked();
    void freqOnOpenOutputFileButtonClicked();
    void freqOnScreenshotButtonClicked();

private:
    Ui::MainWindow *ui;
    Grid *_gridTab;
    QGraphicsScene *_gridScene;
    FrequentItemset *_frequentItemsetTab;
    QGraphicsScene *_frequentItemsetScene;

    void gridConfig();
    void frequentItemsetConfig();
};


#endif // MAINWINDOW_H
