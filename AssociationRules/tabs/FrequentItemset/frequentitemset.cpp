#include "frequentitemset.h"


FrequentItemset::FrequentItemset()
{
    QString runPath = QDir::currentPath();

    _inputOpenFilePath = runPath + "/resources/FrequentItemset/";
    _outputOpenFilePath = runPath + "/resources/FrequentItemset/";
    _inputFilePath = runPath + "/resources/FrequentItemset/input4_10000_fruit.txt";
    _outputFilePath = runPath + "/resources/FrequentItemset/output.txt";

    _nodeRadius = 25;
    _levelHeight = 130;

    _editor = new QTextEdit(nullptr);
    _editor->setReadOnly(true);
    _editor->setWindowTitle("Frequent Itemset");
    _editor->resize(400, 800);
}


FrequentItemset::~FrequentItemset()
{
    delete _editor;
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


void FrequentItemset::onRunAlgorithmButtonClicked(QGraphicsScene *scene, const double minSupport)
{
    scene->clear();
    _transactions.clear();
    _sortedTransactions.clear();
    _itemsFrequencies.clear();
    _setsFrequencies.clear();
    _childrenMap.clear();
    _pendingRemovalEllipses.clear();
    _frequentItemsets.clear();
    _nodesSupport.clear();
    _nodesSupportDown.clear();
    _nodePositions.clear();
    _removalColoring = true;
    _treeBuilt = false;
    _currentTrIndex = 0;
    _maxWidth = 0;
    _itemMap.clear();
    _widths.clear();
    _nodePointers.clear();

    bool readFileSuccess = readFile();
    if(!readFileSuccess) {
        return;
    }

    findItemFrequencies();
    sortTransactions();
    findSetsFrequencies();

    _minSupport = minSupport * _sortedTransactions.size();

    removeRareItemsets();
    findChildren();

    _childrenMapUp = _childrenMap;

    configTree(scene);
    drawItemsLegend(scene);
}


void FrequentItemset::onDrawTransactionButtonClicked(QGraphicsScene *scene)
{
    if(_transactions.size() == 0) {
        QMessageBox::critical(nullptr, "Error", "Run the algorithm first");
        return;
    }

    if(_treeBuilt) {
        QMessageBox::critical(nullptr, "Error", "The tree is already built");
        clearTransactionsLegend(scene);
        return;
    }

    if(_currentTrIndex == 0) {
        QBrush legendBrush = QBrush(Qt::white, Qt::SolidPattern);
        QPen textPen = QPen(Qt::black, 2);
        int yOffset = -58;

        int rectX = _rootPos.x() - _nodeRadius - 360;

        QGraphicsRectItem *item = scene->addRect(rectX, -60, 350, 45, textPen, legendBrush);
        QString legendText1 = "Tr" + QString::number(_currentTrIndex + 1) + ": ";
        QString legendText2 = "Tr" + QString::number(_currentTrIndex + 2) + ": ";

        for(int i = 0; i < _sortedTransactions[_currentTrIndex].size(); i++) {
            legendText1 += _itemMap[_sortedTransactions[_currentTrIndex][i]] + ", ";
        }
        legendText1 = legendText1.removeLast().removeLast();

        for(int i = 0; i < _sortedTransactions[_currentTrIndex+1].size(); i++) {
            legendText2 += _itemMap[_sortedTransactions[_currentTrIndex+1][i]] + ", ";
        }
        legendText2 = legendText2.removeLast().removeLast();

        QGraphicsTextItem *itemText1 = scene->addText(legendText1);
        QGraphicsTextItem *itemText2 = scene->addText(legendText2);
        QFont boldFont = itemText1->font();
        boldFont.setBold(true);
        boldFont.setPointSize(9);
        QFont normalFont = itemText2->font();
        normalFont.setPointSize(9);
        itemText1->setFont(boldFont);
        itemText1->setPos(rectX, yOffset);
        itemText1->setDefaultTextColor(Qt::black);
        itemText2->setFont(normalFont);
        itemText2->setPos(rectX, yOffset + 20);
        itemText2->setDefaultTextColor(Qt::black);
    } else {
        QString legendText1 = "Tr" + QString::number(_currentTrIndex + 1) + ": ";
        QString legendText2 = "Tr" + QString::number(_currentTrIndex + 2) + ": ";

        for(int i = 0; i < _sortedTransactions[_currentTrIndex].size(); i++) {
            legendText1 += _itemMap[_sortedTransactions[_currentTrIndex][i]] + ", ";
        }
        legendText1 = legendText1.removeLast().removeLast();

        if(_currentTrIndex != (_sortedTransactions.size() - 1)) {
            for(int i = 0; i < _sortedTransactions[_currentTrIndex+1].size(); i++) {
                legendText2 += _itemMap[_sortedTransactions[_currentTrIndex+1][i]] + ", ";
            }
            legendText2 = legendText2.removeLast().removeLast();
        }

        for(QGraphicsItem *sceneItem : scene->items()) {
            QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(sceneItem);
            QString nodeText1 = "Tr" + QString::number(_currentTrIndex);
            QString nodeText2 = "Tr" + QString::number(_currentTrIndex + 1);

            if(_currentTrIndex == (_sortedTransactions.size() - 1)) {
                if(textItem && textItem->toPlainText().startsWith(nodeText1)) {
                    textItem->setPlainText(legendText1);
                } else if(textItem && textItem->toPlainText().startsWith(nodeText2)) {
                    delete textItem;
                }

                _treeBuilt = true;
            } else {
                if(textItem && textItem->toPlainText().startsWith(nodeText1)) {
                    textItem->setPlainText(legendText1);
                } else if(textItem && textItem->toPlainText().startsWith(nodeText2)) {
                    textItem->setPlainText(legendText2);
                }
            }
        }
    }

    QVector<int> currentTr = _sortedTransactions[_currentTrIndex];
    QVector<int> currentSet;

    for(int item : currentTr) {
        currentSet.append(item);
        int freq = 1;
        if(_nodesSupportDown.contains(currentSet)) {
            // There is a line between parent and child, just update the text
            freq = _nodesSupportDown[currentSet];
            freq++;
            for(QGraphicsItem *sceneItem : scene->items()) {
                QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(sceneItem);
                QString previousNodeText = QString::number(item) + ": " + QString::number(freq - 1);

                QPointF nodePosition = _nodePositions[currentSet];
                double minX = nodePosition.x() - _nodeRadius;
                double maxX = nodePosition.x() + _nodeRadius;
                double minY = nodePosition.y() - _nodeRadius;
                double maxY = nodePosition.y() + _nodeRadius;

                if(textItem && (textItem->toPlainText() == previousNodeText)) {
                    if(
                        (textItem->x() <= maxX) && (textItem->x() >= minX) &&
                        (textItem->y() <= maxY) && (textItem->y() >= minY)
                    ) {
                        QString nodeText = QString::number(item) + ": " + QString::number(freq);
                        textItem->setPlainText(nodeText);
                        textItem->setPos(
                            nodePosition.x() - textItem->boundingRect().width() / 2,
                            nodePosition.y() - textItem->boundingRect().height() / 2
                        );
                        break;
                    }
                }
            }
        } else {
            // Connect parent with child (draw the line)
            QPointF nodePos = _nodePositions[currentSet];
            if(currentSet.size() == 1) {
                // First level connection (root)
                scene->addLine(
                    _rootPos.x(),
                    _rootPos.y() + _nodeRadius,
                    nodePos.x(),
                    nodePos.y() - _nodeRadius,
                    QPen(Qt::white)
                );
            } else {
                QVector<int> parentSet = currentSet;
                parentSet.pop_back();

                if(_nodePositions.contains(parentSet)) {
                    QPointF parentPos = _nodePositions[parentSet];
                    scene->addLine(
                        parentPos.x(),
                        parentPos.y() + _nodeRadius,
                        nodePos.x(),
                        nodePos.y() - _nodeRadius,
                        QPen(Qt::white)
                    );
                }
            }

            QGraphicsEllipseItem *node = scene->addEllipse(
                nodePos.x() - _nodeRadius, nodePos.y() - _nodeRadius,
                _nodeRadius * 2, _nodeRadius * 2,
                QPen(Qt::black), QBrush(Qt::white)
            );

            QString nodeText = QString::number(currentSet.last()) + ": " + QString::number(freq);
            QGraphicsTextItem *text = scene->addText(nodeText);
            QFont font = text->font();
            font.setPointSize(7);
            text->setFont(font);
            text->setPos(
                nodePos.x() - text->boundingRect().width() / 2,
                nodePos.y() - text->boundingRect().height() / 2
            );
            text->setDefaultTextColor(Qt::black);

            _nodePositions[currentSet] = nodePos;
            _nodesSupportDown[currentSet] = 0;
        }

        _nodesSupportDown[currentSet]++;
    }

    _currentTrIndex++;

    if(_currentTrIndex != 1) {
        drawNodesPointers(scene, "down");
    }
}


void FrequentItemset::onDrawFullTreeButtonClicked(QGraphicsScene *scene)
{
    if(_transactions.size() == 0) {
        QMessageBox::critical(nullptr, "Error", "Run the algorithm first");
        return;
    }

    scene->clear();
    _pendingRemovalEllipses.clear();
    _childrenMapUp = _childrenMap;
    _removalColoring = true;

    QGraphicsEllipseItem *rootNode = scene->addEllipse(
        _rootPos.x() - _nodeRadius, _rootPos.y() - _nodeRadius,
        _nodeRadius * 2, _nodeRadius * 2,
        QPen(Qt::black), QBrush(Qt::white)
    );
    QGraphicsTextItem *rootText = scene->addText("null");
    QFont font = rootText->font();
    font.setPointSize(7);
    rootText->setFont(font);
    rootText->setPos(
        _rootPos.x() - rootText->boundingRect().width() / 2,
        _rootPos.y() - rootText->boundingRect().height() / 2
    );
    rootText->setDefaultTextColor(Qt::black);

    drawItemsLegend(scene);

    for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
        QVector<int> parentKey = it.key();
        QPointF parentPos = _nodePositions[parentKey];
        if(parentKey.size() == 1) {
            scene->addLine(
                _rootPos.x(),
                _rootPos.y() + _nodeRadius,
                parentPos.x(),
                parentPos.y() - _nodeRadius,
                QPen(Qt::white)
            );
        }

        for(const QVector<int> &childKey : it.value()) {
            if(_nodePositions.contains(childKey)) {
                QPointF childPos = _nodePositions[childKey];
                scene->addLine(
                    parentPos.x(),
                    parentPos.y() + _nodeRadius,
                    childPos.x(),
                    childPos.y() - _nodeRadius,
                    QPen(Qt::white)
                );
            }
        }
    }

    for(auto it = _nodePositions.begin(); it != _nodePositions.end(); it++) {
        QVector<int> nodeKey = it.key();
        if(nodeKey != QVector<int>{0}) {
            QPointF nodePos = it.value();
            scene->addEllipse(
                nodePos.x() - _nodeRadius,
                nodePos.y() - _nodeRadius,
                _nodeRadius * 2, _nodeRadius * 2,
                QPen(Qt::black),
                QBrush(Qt::white)
            );

            QString nodeText = QString::number(nodeKey.last()) + ": " + QString::number(_nodesSupport[nodeKey]);
            QGraphicsTextItem *textItem = scene->addText(nodeText);
            QFont font = textItem->font();
            font.setPointSize(7);
            textItem->setFont(font);
            textItem->setPos(
                nodePos.x() - textItem->boundingRect().width() / 2,
                nodePos.y() - textItem->boundingRect().height() / 2
            );
            textItem->setDefaultTextColor(Qt::black);
        }
    }

    _treeBuilt = true;
    drawNodesPointers(scene, "full");
}


void FrequentItemset::onForwardButtonClicked(QGraphicsScene *scene)
{
    if(_childrenMap.size() == 0) {
        QMessageBox::critical(nullptr, "Error", "Run the algorithm first");
        return;
    }

    if(!_treeBuilt) {
        QMessageBox::critical(nullptr, "Error", "The tree needs to be build first");
        return;
    }

    if(_childrenMapUp.size() == 0) {
        QMessageBox::critical(nullptr, "Error", "Algorithm is done, open file to check frequent itemsets");
        return;
    }

    if(_childrenMapUp.size() == _childrenMap.size()) {
        clearTransactionsLegend(scene);
    }

    int maxDepth = 0;
    for(auto it = _childrenMapUp.cbegin(); it != _childrenMapUp.cend(); it++) {
        int keySize = it.key().size();
        maxDepth = std::max(maxDepth, keySize);
    }

    QVector<QVector<int>> deepestNodes;
    for(auto it = _childrenMapUp.cbegin(); it != _childrenMapUp.cend(); it++) {
        if(it.key().size() == maxDepth) {
            deepestNodes.append(it.key());
        }
    }

    QVector<int> nodeToRemove;
    int minFrequency = INT_MAX;
    for(auto it = deepestNodes.cbegin(); it != deepestNodes.cend(); it++) {
        int frequency = _itemsFrequencies.value(it->last(), INT_MAX);
        if(frequency < minFrequency) {
            minFrequency = frequency;
            nodeToRemove = *it;
        }
    }

    QVector<QVector<int>> nodesToRemove;
    for(auto it = _childrenMapUp.cbegin(); it != _childrenMapUp.cend(); it++) {
        if(it.key().last() == nodeToRemove.last() && it.value().isEmpty()) {
            nodesToRemove.append(it.key());
        }
    }

    QString message = "Paths:\n";
    QMap<QVector<int>, int> paths;
    for(QVector<int> &node : nodesToRemove) {
        paths[node] = _nodesSupport[node];

        for(int i : node) {
            message += QString::number(i);
        }
        message += "\n";
    }

    QVector<QVector<int>> candidates;
    int lastElement = nodeToRemove.last();
    QVector<QVector<int>> nodesToRemoveCopy = nodesToRemove;
    message += "\nCandidates:\n";
    for(int i = 0; i < nodesToRemoveCopy.size(); i++) {
        nodesToRemoveCopy[i].pop_back();

        int nodeSize = nodesToRemoveCopy[i].size();
        int numCombinations = pow(2, nodeSize);
        for(int j = 0; j < numCombinations; j++) {
            QVector<int> combination;
            for(int k = 0; k < nodeSize; k++) {
                if(j & (1 << k)) {
                    combination.push_back(nodesToRemoveCopy[i][k]);
                }
            }

            combination.push_back(lastElement);

            auto foundIt = std::find(candidates.begin(), candidates.end(), combination);
            if(foundIt == candidates.end()) {
                candidates.push_back(combination);
                QVector<int> sortedCombination = combination;
                std::sort(sortedCombination.begin(), sortedCombination.end());
                message += "{";
                for(int i : combination) {
                    message += QString::number(i) + ", ";
                }
                message = message.removeLast().removeLast() + "}  #SUP: " + QString::number(_setsFrequencies[sortedCombination]) + "\n";
            }
        }
    }

    message += "\nFrequent itemsets:\n";
    for(QVector<int> &set : candidates) {
        QVector<int> sortedSet = set;
        std::sort(sortedSet.begin(), sortedSet.end());
        auto foundIt = _frequentItemsets.find(sortedSet);
        if((_minSupport - _setsFrequencies[sortedSet] < 0.001) && foundIt == _frequentItemsets.end()) {
            message += "{";
            for(int i : set) {
                message += QString::number(i) + ", ";
            }
            message = message.removeLast().removeLast() + "}  #SUP: " + QString::number(_setsFrequencies[sortedSet]) + "\n";
            _frequentItemsets[sortedSet] = _setsFrequencies[sortedSet];
        }
    }

    if(_removalColoring) {
        for(const auto &node : nodesToRemove) {
            QPointF pos = _nodePositions[node];
            QGraphicsEllipseItem *nodeToRemove = nullptr;
            QGraphicsTextItem *textToRemove = nullptr;
            QList<QGraphicsItem*> itemsInArea = scene->items(
                QRectF(
                    pos.x() - _nodeRadius, pos.y() - _nodeRadius,
                    _nodeRadius * 2, _nodeRadius * 2
                )
            );

            for(QGraphicsItem *item : itemsInArea) {
                if(auto ellipse = dynamic_cast<QGraphicsEllipseItem*>(item)) {
                    nodeToRemove = ellipse;
                } else if(auto text = dynamic_cast<QGraphicsTextItem*>(item)) {
                    textToRemove = text;
                }
            }

            nodeToRemove->setBrush(Qt::red);

            _pendingRemovalEllipses.append(nodeToRemove);
        }

        _editor->setText(message);
        _editor->show();

        _removalColoring = false;
        
        drawNodesPointers(scene, "up");
    } else {
        for(const auto &node : nodesToRemove) {
            for(auto &parent : _childrenMapUp.keys()) {
                if(_childrenMapUp[parent].contains(node)) {
                    _childrenMapUp[parent].removeOne(node);
                    break;
                }
            }
            _childrenMapUp.remove(node);
        }

        for(auto &ellipse : _pendingRemovalEllipses) {
            QList<QGraphicsItem*> items = scene->items(
                QRectF(
                    ellipse->rect().center().x() - _nodeRadius,
                    ellipse->rect().center().y() - _nodeRadius,
                    _nodeRadius * 2, _nodeRadius * 2
                )
            );

            for(QGraphicsItem *item : items) {
                if(auto *line = dynamic_cast<QGraphicsLineItem*>(item)) {
                    scene->removeItem(line);
                    delete line;
                } else if(auto *text = dynamic_cast<QGraphicsTextItem*>(item)) {
                    scene->removeItem(text);
                    delete text;
                }
            }

            scene->removeItem(ellipse);
            delete ellipse;
        }

        _pendingRemovalEllipses.clear();
        _removalColoring = true;
    }

    if(_childrenMapUp.size() == 0) {
        QVector<QVector<int>> frequentItemsetVector = _frequentItemsets.keys();
        std::sort(frequentItemsetVector.begin(), frequentItemsetVector.end(),
            [](const QVector<int> &a, const QVector<int> &b) {
                if(a.size() == b.size()) {
                    return a < b;
                }

                return a.size() < b.size();
            }
        );

        saveFile(frequentItemsetVector);

        QMessageBox::information(nullptr, "Success", "Open file to check frequent itemsets");
        return;
    }
}


void FrequentItemset::onOpenOutputFileButtonClicked()
{
    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(_outputFilePath))) {
        QMessageBox::critical(nullptr, "Error", "Unable to read from output file");
    }
}


void FrequentItemset::onScreenshotButtonClicked(QWidget *mainWindow)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if(!screen) {
        QMessageBox::critical(nullptr, "Error", "Could not get primary screen");
        return;
    }

    QPixmap pixmap = screen->grabWindow(mainWindow->winId());
    QString filename = QFileDialog::getSaveFileName(
        nullptr,
        "Save Screenshot",
        QDir::homePath() + "/screenshot_" + QDateTime::currentDateTime().toString("dd_MM_yyyy__HH_mm_ss") + ".png",
        "Images (*.png *.xpm *.jpg)"
    );

    if(!filename.isEmpty()) {
        pixmap.save(filename);
    }

}


bool FrequentItemset::readFile()
{
    QFile file(_inputFilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Unable to open the input file");
        return false;
    }

    QTextStream in(&file);
    int itemCounter = 1;
    while(!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if(line.isEmpty()) {
            QMessageBox::critical(nullptr, "Error", "Invalid input file: Empty line");
            return false;
        }

        QStringList items = line.split(", ", Qt::SkipEmptyParts);
        QVector<int> transaction;
        for(const QString &item : items) {
            int itemInt;
            if(!_itemMap.values().contains(item)) {
                itemInt = itemCounter++;
                _itemMap[itemInt] = item;
            } else {
                itemInt = _itemMap.key(item);
            }

            transaction.append(itemInt);
        }

        std::sort(transaction.begin(), transaction.end());
        _transactions.append(transaction);
    }

    file.close();
    return true;
}


void FrequentItemset::findItemFrequencies()
{
    for(const QVector<int> &transaction : _transactions) {
        for(int item : transaction) {
            _itemsFrequencies[item]++;
        }
    }
}


void FrequentItemset::sortTransactions()
{
    _sortedTransactions = _transactions;
    for(QVector<int> &transaction : _sortedTransactions) {
        std::stable_sort(transaction.begin(), transaction.end(),
            [this](int a, int b) {
                return _itemsFrequencies.value(a, 0) > _itemsFrequencies.value(b, 0);
            }
        );
    }
}


void FrequentItemset::findSetsFrequencies()
{
    QVector<int> items = _itemsFrequencies.keys();
    int numItems = items.size();
    int totalCombinations = pow(2, numItems);

    for(int mask = 1; mask < totalCombinations; mask++) {
        QVector<int> combination;

        for(int i = 0; i < numItems; i++) {
            if(mask & (1 << i)) {
                combination.append(items[i]);
            }
        }

        int count = 0;
        for(const QVector<int> &transaction : _transactions) {
            bool isSubset = std::all_of(
                combination.begin(), combination.end(),
                [&transaction](int item) {
                    return transaction.contains(item);
                }
            );

            if(isSubset) {
                count++;
            }
        }

        _setsFrequencies[combination] = count;
    }
}


void FrequentItemset::removeRareItemsets()
{
    for(QVector<int> &transaction : _sortedTransactions) {
        auto it = transaction.begin();
        while(it != transaction.end()) {
            if(_minSupport - _itemsFrequencies[*it] > 0.001) {
                it = transaction.erase(it);
            } else {
                it++;
            }
        }
    }
}


void FrequentItemset::findChildren()
{
    for(QVector<int> &transaction : _sortedTransactions) {
        QVector<int> current;

        for(int item : transaction) {
            current.push_back(item);

            if(_childrenMap.find(current) == _childrenMap.end()) {
                _childrenMap[current] = {};
            }

            if(current.size() < transaction.size()) {
                QVector<int> child = current;
                child.push_back(transaction[current.size()]);

                if(!childExists(_childrenMap[current], child)) {
                    _childrenMap[current].push_back(child);
                }
            }
        }
    }
}


bool FrequentItemset::childExists(const QVector<QVector<int>> &childrenList, const QVector<int> &child)
{
    for(auto &existingChild : childrenList) {
        if(existingChild == child) {
            return true;
        }
    }

    return false;
}


struct Node {
    QVector<QPair<int, Node*>> items;
    QVector<int> node;
};


void addToTree(Node* node, const QVector<int>& tr)
{
    QVector<int> currentTr;
    for(int item : tr) {
        currentTr.push_back(item);

        Node* nextNode = nullptr;
        for(auto& p : node->items) {
            if(p.first == item) {
                nextNode = p.second;
                break;
            }
        }

        if(!nextNode) {
            nextNode = new Node();
            nextNode->node = currentTr;
            node->items.push_back({item, nextNode});
        }

        node = nextNode;
    }
}


void printAllTree(Node* node, QVector<QVector<QVector<int>>>& rows)
{
    if(!node) {
        return;
    }

    if(!node->node.empty()) {
        int rowIndex = node->node.size() - 1;
        if(rows.size() <= rowIndex) {
            rows.resize(rowIndex + 1);
        }

        rows[rowIndex].push_back(node->node);
    }

    for(auto& p: node->items) {
        printAllTree(p.second, rows);
    }
}


void FrequentItemset::configTree(QGraphicsScene *scene)
{
    for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
        QVector<int> node = it.key();
        int keySize = node.size();
        _widths[keySize]++;
        _nodesSupport[node] = 0;

        for(const QVector<int> &child : it.value()) {
            bool existsInKeys = false;
            int valueSize = child.size();

            for(auto keyIt = _childrenMap.begin(); keyIt != _childrenMap.end(); keyIt++) {
                if(keyIt.key() == child) {
                    existsInKeys = true;
                    break;
                }
            }

            if(!existsInKeys) {
                _widths[valueSize]++;
                _nodesSupport[child] = 0;
            }
        }
    }

    for(QVector<int> transaction : _sortedTransactions) {
        QVector<int> currentSet;
        for(int item : transaction) {
            currentSet.append(item);
            _nodesSupport[currentSet]++;
        }
    }

    _maxWidth = 0;
    for(auto it = _widths.begin(); it != _widths.end(); it++) {
        _maxWidth = std::max(_maxWidth, it.value());
    }

    // (X, Y, width, height)
    // X = (100 = 2*x + 2*_nodeRadius, x being dist from space for 1 node to circle)
    //      100 is space for one node (_maxWidth * 100)
    //      x = (_maxWidth - 2*_nodeRadius) / 2 - 0.5
    // Y = legend.y() - 1;
    // width = (_maxWidth * 100) - sceneX + ((_maxWidth * 100) - legend.x() + legend.width()) + 1
    // height = (numOfNodeLevels * _levelHeight) + (2 * _nodeRadius) + legend.y() - _nodeRadius + 1.5;
    double sceneX = 50 - _nodeRadius - 0.5;
    double sceneY = -61;
    double sceneWidth = (_maxWidth * 100) - sceneX + 21;
    double sceneHeight = (_widths.size() * static_cast<double>(_levelHeight)) + (2 * static_cast<double>(_nodeRadius)) + 60 - _nodeRadius + 1.5;

    scene->setSceneRect(sceneX, sceneY, sceneWidth, sceneHeight);

    _maxWidth *= 100;
    int currentY = 0;

    _rootPos = QPointF(_maxWidth / 2.0, currentY);
    scene->addEllipse(
        _rootPos.x() - _nodeRadius, _rootPos.y() - _nodeRadius,
        _nodeRadius * 2, _nodeRadius * 2,
        QPen(Qt::black), QBrush(Qt::white)
    );
    QGraphicsTextItem *rootText = scene->addText("null");
    QFont font = rootText->font();
    font.setPointSize(7);
    rootText->setFont(font);
    rootText->setPos(
        _rootPos.x() - rootText->boundingRect().width() / 2,
        _rootPos.y() - rootText->boundingRect().height() / 2
    );
    rootText->setDefaultTextColor(Qt::black);

    currentY = _levelHeight;
    QVector<QPointF> positions;

    for(auto it = _widths.begin(); it != _widths.end(); it++) {
        int level = it.key();
        int nodeCount = it.value();

        for(int i = 0; i < nodeCount; i++) {
            double xPosition = (_maxWidth / static_cast<double>(nodeCount)) * (i + 0.5);
            positions.append(QPointF(xPosition, currentY));
        }

        currentY += _levelHeight;
    }

    Node *node = new Node();
    for(const QVector<int> tr : _sortedTransactions) {
        addToTree(node, tr);
    }

    QVector<QVector<QVector<int>>> rows;

    printAllTree(node, rows);

    int index = 0;
    for(int i = 0; i < rows.size(); i++) {
        for(int j = 0; j < rows[i].size(); j++) {
            _nodePositions[rows[i][j]] = positions[index];
            index++;
        }
    }
}


void FrequentItemset::saveFile(const QVector<QVector<int>> &frequentItemsets)
{
    QFile file(_outputFilePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Unable to open the output file");
        return;
    }

    QTextStream out(&file);
    QString message = "";
    const int itemsetWidth = 20;
    const int supportWidth = 30;

    for(QVector<int> set : frequentItemsets) {
        int support = _frequentItemsets[set];
        double supportPerc = static_cast<double>(support) / _sortedTransactions.size() * 100;
        message = "{";
        for(int num : set) {
            message += QString::number(num) + ", ";
        }
        message = message.removeLast().removeLast() + "}";

        out << message.leftJustified(itemsetWidth)
            << QString("#SUP: %1 (%2%)")
               .arg(support)
               .arg(supportPerc)
               .leftJustified(supportWidth)
            << "\n";
    }

    file.close();
}


void FrequentItemset::drawItemsLegend(QGraphicsScene *scene)
{
    QBrush legendBrush = QBrush(Qt::white, Qt::SolidPattern);
    QPen textPen = QPen(Qt::black, 2);
    int yOffset = 10;
    int legentItemsRectHeight = (3 * yOffset) + ((_itemMap.size() - 1) * 20) - 5;
    int legendX = (_maxWidth <= 300) ? 300 : (_maxWidth - 110);
    QGraphicsRectItem *legendItemsRect = scene->addRect(
        legendX,
        -60,
        130,
        legentItemsRectHeight,
        textPen,
        legendBrush
    );

    for(auto it = _itemMap.begin(); it != _itemMap.end(); it++) {
        int val = _itemsFrequencies.value(it.key());
        QString legendText = QString::number(it.key()) + ": " + it.value() + " (" + QString::number(val) + ")";
        QGraphicsTextItem *itemText = scene->addText(legendText);
        QFont font = itemText->font();
        font.setPointSize(9);
        itemText->setFont(font);
        itemText->setPos(legendX, yOffset - 70);
        itemText->setDefaultTextColor(Qt::black);
        yOffset += 20;
    }
}


void FrequentItemset::clearTransactionsLegend(QGraphicsScene *scene)
{
    for(QGraphicsItem *sceneItem : scene->items()) {
        QGraphicsRectItem *rectItem = dynamic_cast<QGraphicsRectItem*>(sceneItem);
        QGraphicsTextItem *textItem = dynamic_cast<QGraphicsTextItem*>(sceneItem);

        if(rectItem) {
            QPointF rectPos = rectItem->scenePos() + rectItem->rect().topLeft();
            int rectX = _rootPos.x() - _nodeRadius - 300;
            if(rectPos.x() == rectX && rectPos.y() == -60) {
                delete rectItem;
            }
        } else if(textItem && (textItem->toPlainText().startsWith("Tr"))) {
            delete textItem;
        }
    }
}


void FrequentItemset::configNodePointers()
{
    _nodePointers.clear();

    QVector<int> emptyNode = {0};
    for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
        QVector<int> parent = it.key();
        QVector<QVector<int>> children = it.value();
        _nodePointers[parent] = emptyNode;

        for(QVector<int> child : children) {
            if(!_nodePointers.contains(child)) {
                _nodePointers[child] = emptyNode;
            }
        }
    }
}


void FrequentItemset::drawNodesPointers(QGraphicsScene *scene, const QString path)
{
    configNodePointers();

    QMap<int, QVector<QVector<int>>> nodeMatrix;

    if(path == "down") {
        for(auto it = _nodesSupportDown.begin(); it != _nodesSupportDown.end(); it++) {
            QVector<int> node = it.key();
            nodeMatrix[node.last()].append(node);
        }
    } else if(path == "full") {
        for(const auto &node : _nodePositions.keys()) {
            int lastElement = node.last();
            nodeMatrix[lastElement].append(node);
        }
    } else if(path == "up") {
        for(auto it = _childrenMapUp.begin(); it != _childrenMapUp.end(); it++) {
            QVector<int> parent = it.key();
            int lastElement = parent.last();
            nodeMatrix[lastElement].append(parent);
        }
    }

    for(auto it = nodeMatrix.begin(); it != nodeMatrix.end(); it++) {
        QVector<QVector<int>> &nodes = it.value();
        std::sort(nodes.begin(), nodes.end(),
            [this](const QVector<int> &a, const QVector<int> &b) {
                return _nodePositions[a].x() < _nodePositions[b].x();
            }
        );
    }

    for(auto it = nodeMatrix.begin(); it != nodeMatrix.end(); it++) {
        QVector<QVector<int>> nodes = it.value();

        for(int i = 0; i < nodes.size() - 1; i++) {
            _nodePointers[nodes[i]] = nodes[i+1];
        }
    }

    QPen dashedPen(Qt::red);
    dashedPen.setStyle(Qt::DashLine);
    dashedPen.setDashPattern({5, 5});
    QVector<int> emptyVector = {0};

    // Check if some pointers needs to be removed
    for(QGraphicsItem *item : scene->items()) {
        QGraphicsLineItem *lineItem = dynamic_cast<QGraphicsLineItem*>(item);
        if(lineItem && (lineItem->pen().color() == Qt::red)) {
            QLineF line = lineItem->line();
            QPointF left = QPointF(line.x1(), line.y1());
            QPointF right = QPointF(line.x2(), line.y2());

            bool found = false;
            for(auto it = _nodePointers.begin(); it != _nodePointers.end(); it++) {
                QVector<int> leftNode = it.key();
                QVector<int> rightNode = it.value();
                QPointF leftNodePos = _nodePositions[leftNode];
                QPointF rightNodePos = _nodePositions[rightNode];

                if(
                    ((leftNodePos.x() + 25) == left.x()) &&
                    ((leftNodePos.y()) == left.y()) &&
                    ((rightNodePos.x() - 25) == right.x()) &&
                    ((rightNodePos.y() == right.y()))
                ) {
                    found = true;
                    break;
                }
            }

            if(!found) {
                delete lineItem;
            }
        }
    }

    for(auto it = _nodePointers.begin(); it != _nodePointers.end(); it++) {
        QVector<int> leftNode = it.key();
        QVector<int> rightNode = it.value();
        QPointF leftNodePos = _nodePositions[leftNode];
        QPointF rightNodePos = _nodePositions[rightNode];

        if(leftNode != rightNode) {
            bool found = false;
            for(QGraphicsItem *item : scene->items()) {
                QGraphicsLineItem *lineItem = dynamic_cast<QGraphicsLineItem*>(item);
                if(lineItem && (lineItem->pen().color() == Qt::red)) {
                    QLineF line = lineItem->line();
                    QPointF left = QPointF(line.x1(), line.y1());
                    QPointF right = QPointF(line.x2(), line.y2());
                    if(
                        ((leftNodePos.x() + 25) == left.x()) &&
                        ((leftNodePos.y()) == left.y()) &&
                        ((rightNodePos.x() - 25) == right.x()) &&
                        ((rightNodePos.y() == right.y()))
                    ) {
                        found = true;
                        break;
                    }
                }
            }

            if(!found && (rightNode != emptyVector)) {
                QGraphicsLineItem *lineItem = scene->addLine(
                    leftNodePos.x() + _nodeRadius,
                    leftNodePos.y(),
                    rightNodePos.x() - _nodeRadius,
                    rightNodePos.y(),
                    dashedPen
                );
                lineItem->setZValue(-1);
            }
        }
    }
}
