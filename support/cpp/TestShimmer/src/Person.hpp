#ifndef PERSON_HPP
#define PERSON_HPP

#include <QDate>

class Person {

public:

    Person() : ID(-1), Name("not set"), born(QDate()) { }

    Person(qint64 id, const QString& name, const QDate& birth) : ID(id), Name(name), born(birth) { }

    qint64 id() const { return ID; }

    const QString name() const { return Name; }

    const QDate birth() const { return born; }

private:

    qint64 ID;

    QString Name;

    QDate born;

};

#endif // PERSON_HPP
