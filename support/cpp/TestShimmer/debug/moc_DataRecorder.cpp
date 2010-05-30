/****************************************************************************
** Meta object code from reading C++ file 'DataRecorder.h'
**
** Created: Tue May 25 23:58:32 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/DataRecorder.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DataRecorder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DataRecorder[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      28,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   45,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DataRecorder[] = {
    "DataRecorder\0\0sampleAdded()\0"
    "samplesCleared()\0msg\0"
    "onReceiveMessage(ActiveMessage)\0"
};

const QMetaObject DataRecorder::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_DataRecorder,
      qt_meta_data_DataRecorder, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DataRecorder::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DataRecorder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DataRecorder::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DataRecorder))
        return static_cast<void*>(const_cast< DataRecorder*>(this));
    return QObject::qt_metacast(_clname);
}

int DataRecorder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sampleAdded(); break;
        case 1: samplesCleared(); break;
        case 2: onReceiveMessage((*reinterpret_cast< const ActiveMessage(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void DataRecorder::sampleAdded()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void DataRecorder::samplesCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
