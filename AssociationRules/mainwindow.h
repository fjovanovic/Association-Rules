#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QGraphicsScene>

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

    FrequentItemset *frequentItemsetTab;

public slots:
    void freqOnBrowseButtonClicked();
    void freqOnChangeButtonClicked();
    void freqOnRunAlgorithmButtonClicked();
    void freqOnForwardButtonClicked();
    void freqOnBackButtonClicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *frequentItemsetScene;

    void frequentItemsetConfig();
};


#endif // MAINWINDOW_H
