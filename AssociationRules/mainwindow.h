#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "tabs/Grid/grid.h"


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

private:
    Ui::MainWindow *ui;
    Grid *_gridTab;
    QGraphicsScene *_gridScene;

    void gridConfig();
};
#endif // MAINWINDOW_H
