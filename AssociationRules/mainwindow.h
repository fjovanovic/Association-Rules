#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMessageBox>

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

    Grid *gridTab;

public slots:
    void gridOnBrowseButtonClicked();
    void gridOnRunAlgorithmClicked();
    void gridOnChangeButtonClicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
