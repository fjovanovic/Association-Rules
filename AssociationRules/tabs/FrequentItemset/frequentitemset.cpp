#include "frequentitemset.h"


FrequentItemset::FrequentItemset()
{
    QString projectRoot = QCoreApplication::applicationDirPath() + "/../";
    QString absoluteRootPath = QDir(projectRoot).absolutePath();

    _inputOpenFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/";
    _outputOpenFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/";
    _inputFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/input4_100.txt";
    _outputFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/output.txt";
}


QString FrequentItemset::getInputFilePath()
{
    return _inputFilePath;
}


QString FrequentItemset::getOutputFilePath()
{
    return _outputFilePath;
}


QString FrequentItemset::onBrowseButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "Select File",
        _inputOpenFilePath,
        "Text files (*.txt)"
    );

    if(filePath.isEmpty()) {
        return _inputFilePath;
    }

    _inputFilePath = filePath;

    return filePath;
}


QString FrequentItemset::onChangeButtonClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "Select File",
        _outputOpenFilePath,
        "Text files (*.txt)"
    );

    if(filePath.isEmpty()) {
        return _outputFilePath;
    }

    _outputFilePath = filePath;

    return filePath;
}


void FrequentItemset::onRunAlgorithmButtonClicked(QGraphicsScene *scene)
{
    // TODO
}


void FrequentItemset::freqOnForwardButtonClicked()
{
    // TODO
}


void FrequentItemset::freqOnBackButtonClicked()
{
    // TODO
}
