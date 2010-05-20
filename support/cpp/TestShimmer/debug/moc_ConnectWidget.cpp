/****************************************************************************
** Meta object code from reading C++ file 'ConnectWidget.h'
**
** Created: Tue May 18 15:51:52 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/ConnectWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ConnectWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConnectWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      33,   15,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      62,   58,   14,   14, 0x09,
      94,   14,   14,   14, 0x08,
     121,   14,   14,   14, 0x08,
     151,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ConnectWidget[] = {
    "ConnectWidget\0\0portName,baudRate\0"
    "portChanged(QString,int)\0msg\0"
    "onReceiveMessage(ActiveMessage)\0"
    "on_refreshButton_clicked()\0"
    "on_disconnectButton_clicked()\0"
    "on_connectButton_clicked()\0"
};

const QMetaObject ConnectWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ConnectWidget,
      qt_meta_data_ConnectWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ConnectWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ConnectWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ConnectWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConnectWidget))
        return static_cast<void*>(const_cast< ConnectWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ConnectWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: portChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: onReceiveMessage((*reinterpret_cast< const ActiveMessage(*)>(_a[1]))); break;
        case 2: on_refreshButton_clicked(); break;
        case 3: on_disconnectButton_clicked(); break;
        case 4: on_connectButton_clicked(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ConnectWidget::portChanged(QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
