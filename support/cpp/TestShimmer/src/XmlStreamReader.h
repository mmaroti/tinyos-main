#ifndef XMLSTREAMREADER_H
#define XMLSTREAMREADER_H

#include <QTreeWidget>
#include <QtXml/QXmlStreamReader>

class XmlStreamReader
{
public:
    XmlStreamReader(QTreeWidget *tree);

    bool readFile(const QString &fileName);

private:
    void readMoteidElement();
    void readRecordElement(QTreeWidgetItem *parent);
    void readElement(QTreeWidgetItem *parent, QString element);
    void skipUnknownElement();

    QTreeWidget *treeWidget;
    QXmlStreamReader reader;
};

#endif // XMLSTREAMREADER_H
