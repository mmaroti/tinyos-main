#ifndef RECORDLINE_HPP
#define RECORDLINE_HPP

#include <QDateTime>
#include <QString>

class RecordLine {

public:

    RecordLine();

    RecordLine(int mote_id,
               int rec_id,
               const char* length,
               const QDateTime& downloaded,
               const QDateTime& recorded);

    int mote_id() const;
    int record_id() const;
    const QString& length() const;
    const QDateTime& downloaded() const;
    const QDateTime& recorded() const;

private:

    int mote;
    int record;
    QString computed_length;
    QDateTime download;
    QDateTime date_recorded;

};

#endif // RECORDLINE_HPP
