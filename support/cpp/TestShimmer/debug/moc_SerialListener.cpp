/****************************************************************************
** Meta object code from reading C++ file 'SerialListener.h'
**
** Created: Tue May 18 15:51:53 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/SerialListener.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SerialListener.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SerialListener[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   16,   15,   15, 0x05,
      66,   50,   15,   15, 0x05,
     104,   96,   15,   15, 0x25,

 // slots: signature, parameters, type, tag, flags
     148,  130,   15,   15, 0x0a,
     175,   15,   15,   15, 0x08,
     195,  189,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SerialListener[] = {
    "SerialListener\0\0msg\0receiveMessage(ActiveMessage)\0"
    "message,timeout\0showNotification(QString,int)\0"
    "message\0showNotification(QString)\0"
    "portName,baudRate\0onPortChanged(QString,int)\0"
    "onReadyRead()\0event\0timerEvent(QTimerEvent*)\0"
};

const QMetaObject SerialListener::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SerialListener,
      qt_meta_data_SerialListener, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SerialListener::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SerialListener::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SerialListener::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SerialListener))
        return static_cast<void*>(const_cast< SerialListener*>(this));
    return QObject::qt_metacast(_clname);
}

int SerialListener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: receiveMessage((*reinterpret_cast< const ActiveMessage(*)>(_a[1]))); break;
        case 1: showNotification((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: showNotification((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: onPortChanged((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: onReadyRead(); break;
        case 5: timerEvent((*reinterpret_cast< QTimerEvent*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SerialListener::receiveMessage(const ActiveMessage & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SerialListener::showNotification(const QString & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
