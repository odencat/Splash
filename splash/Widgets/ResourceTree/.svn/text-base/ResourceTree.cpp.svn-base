/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "ResourceTree/ResourceTree.h"
#include <fstream>
#include <sstream>

ResourceTree::ResourceTree(QWidget *parent)
    : QTreeWidget(parent)
{
    QStringList labels;
    labels << tr("Animation Files");

    header()->setResizeMode(QHeaderView::Stretch);
    setHeaderLabels(labels);

    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu(const QPoint &)));
    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void ResourceTree::contextMenu(const QPoint &pos)
{
    QMenu *menu = new QMenu;

    menu->addAction(tr("&Add"), this, SLOT(addItem()));
    menu->addAction(tr("&Cut"), this, SLOT(cutItem()));
    menu->addAction(tr("&Copy"), this, SLOT(copyFrame()));
    menu->addAction(tr("&Paste"), this, SLOT(pasteFrame()));

    menu->exec(this->mapToGlobal(pos));
}

void ResourceTree::addItem()
{
}

bool ResourceTree::read(QString path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    std::ifstream ifs(path.toStdString().c_str());
    std::string inputXmlText((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    QString xmlText = QString::fromStdString(inputXmlText);

    if (!domDocument.setContent(xmlText, &errorStr, &errorLine,
                                &errorColumn)) {
        QMessageBox::information(window(), tr("ANI Project"),
                                 tr("Parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return false;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "ani") {
        QMessageBox::information(window(), tr("Resouce File"),
                                 tr("The file is not an ANI file."));
        return false;
    } else if (root.hasAttribute("version")
               && root.attribute("version") != "1.0") {
        QMessageBox::information(window(), tr("Resouce File"),
                                 tr("The file is not an ANI version 1.0 "
                                    "file."));
        return false;
    }

    clear();

    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
               this, SLOT(updateDomElement(QTreeWidgetItem *, int)));

    QDomElement child = root.firstChildElement("Directory");
    while (!child.isNull()) {
        parseFolderElement(child);
        child = child.nextSiblingElement("Directory");
    }

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
            this, SLOT(updateDomElement(QTreeWidgetItem *, int)));

    return true;
}

bool ResourceTree::write(QIODevice *device)
{
    const int IndentSize = 4;

    QTextStream out(device);
    domDocument.save(out, IndentSize);
    return true;
}

void ResourceTree::updateDomElement(QTreeWidgetItem *item, int column)
{
    QDomElement element = domElementForItem.value(item);
    if (!element.isNull()) {
        if (column == 0) {
            element.setAttribute("path", item->text(0));
        }
    }
}

void ResourceTree::parseFolderElement(const QDomElement &element,
                                  QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item = createItem(element, parentItem);


    QString pathName = element.attributes().namedItem("path").nodeValue();

    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setIcon(0, folderIcon);
    item->setText(0, pathName);

//    bool folded = (element.attribute("folded") != "no");
//    setItemExpanded(item, !folded);

    QDomElement child = element.firstChildElement();
    while (!child.isNull()) {
        if (child.tagName() == "Directory") {
            parseFolderElement(child, item);
        } else if (child.tagName() == "animation") {
            QTreeWidgetItem *childItem = createItem(child, item);

            QString idName = child.attributeNode(QString("id")).nodeName();

            childItem->setFlags(item->flags() | Qt::ItemIsEditable);
            childItem->setIcon(0, bookmarkIcon);
            childItem->setText(0, idName);
        }
        child = child.nextSiblingElement();
    }
}

QTreeWidgetItem *ResourceTree::createItem(const QDomElement &element,
                                      QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item;
    if (parentItem) {
        item = new QTreeWidgetItem(parentItem);
    } else {
        item = new QTreeWidgetItem(this);
    }
    domElementForItem.insert(item, element);
    return item;
}
