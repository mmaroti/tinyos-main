#include "XmlStreamReader.h"
#include "QFile"
#include <cstdio>
#include <stdio.h>
#include <QTextStream>
#include <QDebug>

XmlStreamReader::XmlStreamReader(QTreeWidget *tree)
{
    treeWidget = tree;
}

bool XmlStreamReader::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Error: Cannot read file " << qPrintable(fileName)
                  << ": " << qPrintable(file.errorString());
                  //<< std::endl;
        return false;
    }

    reader.setDevice(&file);
    reader.readNext();
       while (!reader.atEnd()) {
           if (reader.isStartElement()) {
               if (reader.name() == "sdownload") {
                   readMoteidElement();
               } else {
                   reader.raiseError(QObject::tr("Not a Shimmer data file"));
               }
           } else {
               reader.readNext();
           }
       }
       file.close();
           if (reader.hasError()) {
               qDebug() << "Error: Failed to parse file "
                         << qPrintable(fileName) << ": "
                         << qPrintable(reader.errorString());// << std::endl;
               return false;
           } else if (file.error() != QFile::NoError) {
               qDebug() << "Error: Cannot read file " << qPrintable(fileName)
                         << ": " << qPrintable(file.errorString());
                         //<< std::endl;
               return false;
           }
           return true;
       }


void XmlStreamReader::readMoteidElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "moteid") {
                readRecordElement(treeWidget->invisibleRootItem());
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void XmlStreamReader::readRecordElement(QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(0, reader.attributes().value("num").toString());

    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            if (reader.name() == "record") {
                readRecordElement(item);
            } else if (reader.name() == "length") {
                readElement(item, "length");
            } else if (reader.name() == "tor") {
                readElement(item, "tor");
            } else if (reader.name() == "tod") {
                readElement(item, "tod");
            } else {
                skipUnknownElement();
            }
        } else {
            reader.readNext();
        }
    }
}

void XmlStreamReader::readElement(QTreeWidgetItem *parent, QString element)
{
    QString text = reader.readElementText();
    if (reader.isEndElement())
        reader.readNext();
    if(element == "length") {
        parent->setText(2, text);
    } else if (element == "tor") {
        parent->setText(3, text);
    } else if (element == "tod") {
        parent->setText(4, text);
    }

}

void XmlStreamReader::skipUnknownElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }

        if (reader.isStartElement()) {
            skipUnknownElement();
        } else {
            reader.readNext();
        }
    }
}

