#ifndef FREQUENTITEMSET_H
#define FREQUENTITEMSET_H

#include <QString>
#include <QCoreApplication>
#include <QFileDialog>
#include <QGraphicsScene>


class FrequentItemset
{
public:
    // Constructors
    FrequentItemset();

    // Getters
    QString getInputFilePath();
    QString getOutputFilePath();

    // Slots
    QString onBrowseButtonClicked();
    QString onChangeButtonClicked();
    void onRunAlgorithmButtonClicked(QGraphicsScene *scene);
    void freqOnForwardButtonClicked();
    void freqOnBackButtonClicked();

private:
    QString _inputFilePath;
    QString _outputFilePath;
    QString _inputOpenFilePath;
    QString _outputOpenFilePath;
};

#endif // FREQUENTITEMSET_H
