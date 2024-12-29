#include "frequentitemset.h"


FrequentItemset::FrequentItemset()
{
    QString projectRoot = QCoreApplication::applicationDirPath() + "/../";
    QString absoluteRootPath = QDir(projectRoot).absolutePath();

    _inputOpenFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/";
    // _inputOpenFilePath = "/home/filip/Desktop";
    _outputOpenFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/";
    // _inputFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/input.txt";
    _inputFilePath = "/home/filip/Desktop/input.txt";
    // _inputFilePath = "/home/filip/Desktop/input.txt";
    _outputFilePath = absoluteRootPath + "/AssociationRules/resources/Frequent Itemset/output.txt";

    _nodeRadius = 25;


    toLetter.insert(1, "A");
    toLetter.insert(2, "B");
    toLetter.insert(3, "C");
    toLetter.insert(4, "D");
    toLetter.insert(5, "E");
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
    _removalColoring = true;

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

    _currentChildrenMap = _childrenMap;

    drawTree(scene);
}


void FrequentItemset::onForwardButtonClicked(QGraphicsScene *scene)
{
    if(_childrenMap.size() == 0) {
        QMessageBox::critical(nullptr, "Error", "Run the algorithm first");
        return;
    }

    if(_currentChildrenMap.size() == 0) {
        qDebug() << "FREQUENT ITEMSETS:";
        for(auto it = _frequentItemsets.begin(); it != _frequentItemsets.end(); it++) {
            QVector<int> node = it.key();
            int support = it.value();
            QString row = "[";
            for(int item : node) {
                row += toLetter[item] + ", ";
            }
            row = row.removeLast().removeLast() + "]: " + QString::number(support);
            qDebug() << row;
        }

        return;


        // QMessageBox::critical(nullptr, "Error", "No more nodes to remove");
        // return;
    }
    qDebug() << "---------------------------BEGIN---------------------------";

    // Finding nodes to remove at max depth with with lowest frequency
    int maxDepth = 0;
    for(const auto &key : _currentChildrenMap.keys()) {
        int keySize = key.size();
        maxDepth = std::max(maxDepth, keySize);
    }

    QVector<QVector<int>> deepestNodes;
    for(const auto &key : _currentChildrenMap.keys()) {
        if(key.size() == maxDepth) {
            deepestNodes.append(key);
        }
    }

    QVector<int> nodeToRemove;
    int minFrequency = INT_MAX;
    for(const auto &node : deepestNodes) {
        int frequency = _itemsFrequencies.value(node.last(), INT_MAX);
        if(frequency < minFrequency) {
            minFrequency = frequency;
            nodeToRemove = node;
        }
    }

    QVector<QVector<int>> nodesToRemove;
    for(const auto &key : _currentChildrenMap.keys()) {
        if(key.last() == nodeToRemove.last() && _currentChildrenMap.value(key).isEmpty()) {
            nodesToRemove.append(key);
        }
    }

    int endingItem = nodesToRemove.last().last();

    // 1. Get paths to the leaf nodes
    QMap<QVector<int>, int> paths;
    for(QVector<int> &node : nodesToRemove) {
        paths[node] = _nodesSupport[node];
    }

    qDebug() << "Paths:";
    for(auto it = paths.begin(); it != paths.end(); it++) {
        QVector<int> key = it.key();
        int val = it.value();

        QString row = "";
        for(int node : key) {
            row += toLetter[node];
        }
        row += ": " + QString::number(val);
        qDebug() << row;
    }


    // 2. Count of each item in paths
    QMap<int, int> countItem;
    for(auto it = paths.begin(); it != paths.end(); it++) {
        QVector<int> key = it.key();
        for(int node : key) {
            countItem[node] += _itemsFrequencies[node];
        }
    }

    // TODO: ovde dodaj iteme u string za ispis fajla, i ako neki ne zadovoljava uslov minSupport ispisi nesto pored njega

    qDebug() << "Count items:";
    for(auto it = countItem.begin(); it != countItem.end(); ) {
        int key = it.key();
        double support = static_cast<double>(it.value());
        qDebug() << toLetter[key] << ": " << QString::number(support);
        if(_minSupport - support > 0.001) {
            it = countItem.erase(it);
        } else {
            it++;
        }
    }

    // 3. Generate candidates and calculate their support
    QVector<int> pathItems = countItem.keys();
    QVector<QVector<int>> candidates;

    // int numCombinations = 1 << pathItems.size();

    // for(int mask = 1; mask < numCombinations; mask++) {
    //     QVector<int> combination;

    //     for(int i = 0; i < pathItems.size(); i++) {
    //         if(mask & (1 << i)) {
    //             combination.append(pathItems[i]);
    //         }
    //     }

    //     if(!combination.isEmpty() && combination.last() == endingItem) {
    //         candidates.append(combination);
    //     }
    // }
    QString nodeTxt = "Node to remove: ";
    for(int i : nodeToRemove) {
        nodeTxt += toLetter[i];
    }
    qDebug() << nodeTxt;
    // if(nodeToRemove.size() > 0) {
    //     candidates.append(QVector<int>({endingItem}));
    // }


    int lastElement = nodeToRemove.last(); // Poslednji element

    // // Iteriraj kroz početne pozicije niza
    // for (int start = 0; start < nodeToRemove.size(); ++start) {
    //     QVector<int> current;

    //     // Proširuj podniz počevši od trenutne pozicije
    //     for (int i = start; i < nodeToRemove.size(); ++i) {
    //         current.append(nodeToRemove[i]);

    //         // Dodaj samo podnizove koji se završavaju poslednjim elementom
    //         if (current.last() == lastElement) {
    //             candidates.append(current);
    //         }
    //     }
    // }



    for(int i = 0; i < nodesToRemove.size(); i++) {
        // candidates.append(nodesToRemove.last().last());
        for (int j = 0; j < nodeToRemove.size() - 1; j++) {
            QVector<int> current;

            // Dodajemo elemente od trenutne pozicije do kraja niza
            for (int k = j; k < nodeToRemove.size() - 1; ++j) {
                current.append(nodeToRemove[k]);
                QVector<int> nodeToAdd = current;
                nodeToAdd.append(lastElement);

                // Dodaj trenutnu kombinaciju u rezultat
                candidates.append(nodeToAdd);
            }
        }
    }

    // QMap<QVector<int>, int> candidatesSupport;
    // for(const QVector<int> &transaction : _sortedTransactions) {
    //     for(const QVector<int> &candidate : candidates) {
    //         bool isIncluded = std::includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end());
    //         if(isIncluded) {
    //             candidatesSupport[candidate]++;
    //         }
    //     }
    // }

    qDebug() << "Candidates:";
    for(QVector<int> set : candidates) {
        QString row = "[";
        for(int i : set) {
            row += toLetter[i] + ", ";
        }
        row = row.removeLast().removeLast() + "]: " + QString::number(_setsFrequencies[set]);
        qDebug() << row;
    }

    qDebug() << "Frequent itemsets:";
    for(QVector<int> set : candidates) {
        QString row = "[";
        for(int i : set) {
            row += toLetter[i] + ", ";
        }
        row = row.removeLast().removeLast() + "]: " + QString::number(_setsFrequencies[set]);
        if(_minSupport - _setsFrequencies[set] < 0.001) {
            qDebug() << row;
            _frequentItemsets[set] = _setsFrequencies[set];
        }
    }

    qDebug() << "--------------------------------------------------------";

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

            if(nodeToRemove) {
                scene->removeItem(nodeToRemove);
                delete nodeToRemove;
            }

            if(textToRemove) {
                scene->removeItem(textToRemove);
                delete textToRemove;
            }

            QGraphicsEllipseItem *ellipse = scene->addEllipse(
                pos.x() - _nodeRadius, pos.y() - _nodeRadius,
                _nodeRadius * 2, _nodeRadius * 2,
                QPen(Qt::black), QBrush(Qt::red)
            );
            QString nodeText = toLetter[node.last()] + ": " + QString::number(_nodesSupport[node]);
            QGraphicsTextItem *text = scene->addText(nodeText);
            QFont font = text->font();
            font.setPointSize(7);
            text->setFont(font);
            text->setPos(
                pos.x() - text->boundingRect().width() / 2,
                pos.y() - text->boundingRect().height() / 2
            );
            text->setDefaultTextColor(Qt::black);
            _pendingRemovalEllipses.append(ellipse);
        }

        _removalColoring = false;
    } else {
        for(const auto &node : nodesToRemove) {
            for(auto &parent : _currentChildrenMap.keys()) {
                if(_currentChildrenMap[parent].contains(node)) {
                    _currentChildrenMap[parent].removeOne(node);
                    break;
                }
            }
            _currentChildrenMap.remove(node);
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
}


bool FrequentItemset::readFile()
{
    QFile file(_inputFilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Unable to open the input file");
        return false;
    }

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if(line.isEmpty()) {
            QMessageBox::critical(nullptr, "Error", "Invalid input file: Empty line");
            return false;
        }

        QStringList items = line.split(" ", Qt::SkipEmptyParts);
        QVector<int> transaction;
        for(const QString &item : items) {
            bool isNumber = false;
            int itemInt = item.toInt(&isNumber);

            if(!isNumber) {
                QMessageBox::critical(nullptr, "Error", "Invalid input file: Non-numeric value");
                return false;
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
        std::sort(transaction.begin(), transaction.end(),
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

    // for(auto it = _setsFrequencies.begin(); it != _setsFrequencies.end(); it++) {
    //     QVector<int> tr = it.key();
    //     int supp = it.value();
    //     QString row = "[";
    //     for(int item : tr) {
    //         row += toLetter[item] + ", ";
    //     }
    //     row = row.removeLast().removeLast() + "]: " + QString::number(supp);
    //     qDebug() << row;
    // }
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
            // QString currentStr = "Ubacujem roditelja: [";
            // for(int i : current) {
            //     currentStr += toLetter[i] + ", ";
            // }
            // currentStr = currentStr.removeLast().removeLast() + "]";
            // qDebug() << currentStr;

            if(_childrenMap.find(current) == _childrenMap.end()) {
                _childrenMap[current] = {};
            }

            if(current.size() < transaction.size()) {
                QVector<int> child = current;
                child.push_back(transaction[current.size()]);

                if(!childExists(_childrenMap[current], child)) {
                    _childrenMap[current].push_back(child);

                    // QString row = "Ubacujem dete: [";
                    // for(int i : child) {
                    //     row += toLetter[i] + ", ";
                    // }
                    // row = row.removeLast().removeLast() + "]";
                    // qDebug() << row;
                }
            }
        }
    }

    // qDebug() << "Deca nakon unosa";

    // for (auto it = _childrenMap.begin(); it != _childrenMap.end(); ++it) {
    //     const QVector<int> &key = it.key();
    //     const QVector<QVector<int>> &value = it.value();

    //     // Ispis ključa
    //     QString keyString = "Key: [";
    //     for (int item : key) {
    //         keyString += toLetter[item] + " ";
    //     }
    //     keyString += "]";

    //     qDebug() << keyString;

    //     // Ispis vrednosti
    //     qDebug() << "  Children:";
    //     for (const QVector<int> &child : value) {
    //         QString childString = "    [";
    //         for (int item : child) {
    //             childString += toLetter[item] + " ";
    //         }
    //         childString += "]";
    //         qDebug() << childString;
    //     }
    // }
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


void FrequentItemset::drawTree(QGraphicsScene *scene)
{
    QMap<int, int> widths;
    for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
        QVector<int> node = it.key();
        int keySize = node.size();
        widths[keySize]++;
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
                widths[valueSize]++;
                _nodesSupport[child] = 0;
            }
        }
    }

    int maxWidth = 0;
    for(auto it = widths.begin(); it != widths.end(); it++) {
        maxWidth = std::max(maxWidth, it.value());
    }

    for(QVector<int> &transaction : _sortedTransactions) {
        QVector<int> current;
        for(int item : transaction) {
            current.push_back(item);
            _nodesSupport[current]++;
        }
    }

    // for (auto it = _nodesSupport.begin(); it != _nodesSupport.end(); ++it) {
    //     QVector<int> &key = const_cast<QVector<int>&>(it.key());

    //     // Sortiraj elemente ključa u mestu prema _itemsFrequencies
    //     std::sort(key.begin(), key.end(),
    //         [this](int a, int b) {
    //             return _itemsFrequencies.value(a, 0) > _itemsFrequencies.value(b, 0);
    //         }
    //     );
    // }

    // qDebug() << "nodesSupport posle sortiranja:";
    // for(auto it = _nodesSupport.begin(); it != _nodesSupport.end(); ++it) {
    //     QString row = "[";
    //     QVector<int> tr = it.key();
    //     int supp = it.value();
    //     for(int i : tr) {
    //         row += toLetter[i] + ", ";
    //     }
    //     row = row.removeLast().removeLast() + "]: " + QString::number(supp);
    //     qDebug() << row;
    // }
    // for
    // std::sort(
    //     _nodesSupport.begin(), _nodesSupport.end(),
    //     [this](int a, int b) {
    //         return _itemsFrequencies.value(a, 0) > _itemsFrequencies.value(b, 0);
    //     }
    // );

    maxWidth *= 100;
    const int levelHeight = 130;
    int currentY = 0;

    QPointF rootPos(maxWidth / 2.0, currentY);
    scene->addEllipse(
        rootPos.x() - _nodeRadius, rootPos.y() - _nodeRadius,
        _nodeRadius * 2, _nodeRadius * 2,
        QPen(Qt::black), QBrush(Qt::white)
    );
    QGraphicsTextItem *rootText = scene->addText("null");
    QFont font = rootText->font();
    font.setPointSize(7);
    rootText->setFont(font);
    rootText->setPos(
        rootPos.x() - rootText->boundingRect().width() / 2,
        rootPos.y() - rootText->boundingRect().height() / 2
    );
    rootText->setDefaultTextColor(Qt::black);

    currentY += levelHeight;

    for(auto it = widths.begin(); it != widths.end(); it++) {
        int level = it.key();
        int nodeCount = it.value();

        QVector<QPointF> positions;

        // Raspodela čvorova na nivou
        for(int i = 0; i < nodeCount; i++) {
            double xPosition = (maxWidth / static_cast<double>(nodeCount)) * (i + 0.5);
            positions.append(QPointF(xPosition, currentY));
        }

        // Sačuvaj pozicije za čvorove trenutnog nivoa
        int index = 0;
        for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
            const QVector<int> &childKey = it.key();
            if(childKey.size() == level) {
                _nodePositions[childKey] = positions[index++];
                if(index >= positions.size()) {
                    break;
                }
            }
        }

        // Nacrtaj čvorove za trenutni nivo
        for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
            const QVector<int> &childKey = it.key();
            if(childKey.size() == level) {
                QPointF pos = _nodePositions[childKey];
                scene->addEllipse(
                    pos.x() - _nodeRadius, pos.y() - _nodeRadius,
                    _nodeRadius * 2, _nodeRadius * 2,
                    QPen(Qt::black), QBrush(Qt::white)
                );

                // Dodaj tekst sa poslednjim elementom ključa
                // QString nodeText = QString::number(childKey.last()) + ": " + QString::number(_setsFrequencies[childKey]);
                // QString itemStr = "[";
                // for(int i : childKey) {
                //     itemStr += toLetter[i] + ", ";
                // }
                // itemStr = itemStr.removeLast().removeLast() + "] : " + QString::number(_nodesSupport[childKey]);
                // qDebug() << "Crtam: " << itemStr;
                // QVector<int>
                QString nodeText = toLetter[childKey.last()] + ": " + QString::number(_nodesSupport[childKey]);
                QGraphicsTextItem *text = scene->addText(nodeText);
                QFont font = text->font();
                font.setPointSize(7);
                text->setFont(font);
                text->setPos(
                    pos.x() - text->boundingRect().width() / 2,
                    pos.y() - text->boundingRect().height() / 2
                );
                text->setDefaultTextColor(Qt::black);
            }
        }

        currentY += levelHeight;
    }

    for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
        const QVector<int> &childKey = it.key();
        if(childKey.size() == 1) {
            QPointF childPos = _nodePositions[childKey];
            scene->addLine(
                rootPos.x(), rootPos.y() + _nodeRadius,
                childPos.x(), childPos.y() - _nodeRadius,
                QPen(Qt::white)
            );
        }
    }

    for(auto it = _childrenMap.begin(); it != _childrenMap.end(); it++) {
        const QVector<int> &parent = it.key();
        const QVector<QVector<int>> &children = it.value();

        if(!_nodePositions.contains(parent)) {
            continue;
        }

        QPointF parentPos = _nodePositions[parent];
        for(const QVector<int> &child : children) {
            if(_nodePositions.contains(child)) {
                QPointF childPos = _nodePositions[child];
                scene->addLine(
                    parentPos.x(), parentPos.y() + _nodeRadius,
                    childPos.x(), childPos.y() - _nodeRadius,
                    QPen(Qt::white)
                );
            }
        }
    }
}
