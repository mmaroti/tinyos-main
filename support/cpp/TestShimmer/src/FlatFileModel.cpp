#include "FlatFileModel.h"

FlatFileModel::FlatFileModel(const QString &fileName, QObject *parent)
        : QAbstractTableModel(parent)
{
    file.setFileName(fileName);
    updateOffsets();
}

void FlatFileModel::updateOffsets() const
{
    const int ChunkSize = 50;
    offsets.clear();
    QFileInfo finfo(file);
    qint64 size = finfo.size();
    if (!size || !file.open(QIODevice::ReadOnly))
        return;
    modified = finfo.lastModified();
    offsets.append(0);
    qint64 offset = 0;
    while (size) {
        QByteArray bytes = file.read(ChunkSize);
        if (bytes.isEmpty())
            break;
        size -= bytes.size();
        qint64 i = bytes.indexOf("\nMOTEID: ");
        if (i != -1)
            offsets.append(offset + i);
        offset += bytes.size();
    }
    file.close();
    offsets.append(finfo.size());
}

QVariant FlatFileModel::data(const QModelIndex &index, int role) const
{
   if (updateRequired()) updateOffsets();

   if (!index.isValid() || index.row() < 0
           || index.row() >= offsets.size() - 1
           || role != Qt::DisplayRole)
       return QVariant();

   if (!file.open(QIODevice::ReadOnly))
       return QVariant();
   qint64 offset = offsets.at(index.row());
   qint64 length = offsets.at(index.row() + 1) - offset;
   file.seek(offset);
   QByteArray bytes = file.read(length);
   file.close();
   if (bytes.size() != length)
       return QVariant();

   QString record = QLatin1String(bytes.data());
   QString key;
   switch (index.column()) {
       case 0: key = "MOTEID: "; break;
       case 1: key = "\n#: "; break;
       case 2: key = "\nLENGTH: "; break;
       case 3: key = "\nDATE of DOWNLOAD: "; break;
       case 4: key = "\nDATE of RECORDING: "; break;
       case 5: key = "\n\n\t"; break;
       default: return QVariant();
   }
   int i = record.indexOf(key);
   if (i != -1) {
       i += key.size();
       if (index.column() != 5) {
           int j = record.indexOf("\n", i);
           if (j != -1)
               return record.mid(i, j - i + 1);
       } else
           return record.mid(i).replace("\n\t", "\n");
   }
   return QVariant();
}

QVariant FlatFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   if (role != Qt::DisplayRole)
       return QVariant();

   if (orientation == Qt::Horizontal) {
       switch (section) {
       case 0: return tr("MOTEID");
       case 1: return tr("#");
       case 2: return tr("LENGTH");
       case 3: return tr("DATE of DOWNLOAD");
       case 4: return tr("DATE of RECORDING");
       default: return QVariant();
       }
   } else
       return QString("%1").arg(section + 1);
}

int FlatFileModel::rowCount(const QModelIndex &) const
{
    if (updateRequired()) updateOffsets();
    return offsets.size() - 1;
}

bool FlatFileModel::updateRequired() const
{
    return modified != QFileInfo(file).lastModified();
}
