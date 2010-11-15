#ifndef FLATFILEMODEL_H
#define FLATFILEMODEL_H

#include <QAbstractTableModel>
#include <QFile>
#include <QDateTime>
#include <QFileInfo>
#include <QVector>

class FlatFileModel : public QAbstractTableModel
   {
   public:
       FlatFileModel(const QString &fileName,
                     QObject *parent = 0);

       QVariant data(const QModelIndex &index,
                     int role) const;
       QVariant headerData(int section,
                           Qt::Orientation orientation,
                           int role) const;
       int rowCount(const QModelIndex &parent) const;
       int columnCount(const QModelIndex &) const
               { return 5; }

   private:
       bool updateRequired() const;
       void updateOffsets() const;

       mutable QFile file;
       mutable QDateTime modified;
       mutable QVector<int> offsets;
   };

#endif // FLATFILEMODEL_H
