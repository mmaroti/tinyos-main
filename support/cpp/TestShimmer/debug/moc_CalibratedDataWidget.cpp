/****************************************************************************
** Meta object code from reading C++ file 'CalibratedDataWidget.h'
**
** Created: Wed May 26 01:53:10 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/CalibratedDataWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CalibratedDataWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CalibratedDataWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,
      42,   21,   21,   21, 0x08,
      62,   21,   21,   21, 0x08,
      82,   21,   21,   21, 0x08,
     104,   21,   21,   21, 0x08,
     125,   21,   21,   21, 0x08,
     146,   21,   21,   21, 0x08,
     167,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CalibratedDataWidget[] = {
    "CalibratedDataWidget\0\0on_zAccel_clicked()\0"
    "on_yAccel_clicked()\0on_xAccel_clicked()\0"
    "on_avgAccel_clicked()\0on_xyangle_clicked()\0"
    "on_yzangle_clicked()\0on_zxangle_clicked()\0"
    "on_exportButton_clicked()\0"
};

const QMetaObject CalibratedDataWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CalibratedDataWidget,
      qt_meta_data_CalibratedDataWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CalibratedDataWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CalibratedDataWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CalibratedDataWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CalibratedDataWidget))
        return static_cast<void*>(const_cast< CalibratedDataWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CalibratedDataWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_zAccel_clicked(); break;
        case 1: on_yAccel_clicked(); break;
        case 2: on_xAccel_clicked(); break;
        case 3: on_avgAccel_clicked(); break;
        case 4: on_xyangle_clicked(); break;
        case 5: on_yzangle_clicked(); break;
        case 6: on_zxangle_clicked(); break;
        case 7: on_exportButton_clicked(); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
