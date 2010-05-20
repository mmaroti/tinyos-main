/****************************************************************************
** Meta object code from reading C++ file 'RawDataWidget.h'
**
** Created: Tue May 18 15:51:54 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/RawDataWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RawDataWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_RawDataWidget[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      33,   14,   14,   14, 0x08,
      54,   14,   14,   14, 0x08,
      73,   14,   14,   14, 0x08,
      92,   14,   14,   14, 0x08,
     111,   14,   14,   14, 0x08,
     131,   14,   14,   14, 0x08,
     151,   14,   14,   14, 0x08,
     171,   14,   14,   14, 0x08,
     196,   14,   14,   14, 0x08,
     222,   14,   14,   14, 0x08,
     246,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_RawDataWidget[] = {
    "RawDataWidget\0\0on_temp_clicked()\0"
    "on_voltage_clicked()\0on_zGyro_clicked()\0"
    "on_yGyro_clicked()\0on_xGyro_clicked()\0"
    "on_zAccel_clicked()\0on_yAccel_clicked()\0"
    "on_xAccel_clicked()\0on_clearButton_clicked()\0"
    "on_recordButton_clicked()\0"
    "on_loadButton_clicked()\0on_saveButton_clicked()\0"
};

const QMetaObject RawDataWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_RawDataWidget,
      qt_meta_data_RawDataWidget, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &RawDataWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *RawDataWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *RawDataWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_RawDataWidget))
        return static_cast<void*>(const_cast< RawDataWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int RawDataWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_temp_clicked(); break;
        case 1: on_voltage_clicked(); break;
        case 2: on_zGyro_clicked(); break;
        case 3: on_yGyro_clicked(); break;
        case 4: on_xGyro_clicked(); break;
        case 5: on_zAccel_clicked(); break;
        case 6: on_yAccel_clicked(); break;
        case 7: on_xAccel_clicked(); break;
        case 8: on_clearButton_clicked(); break;
        case 9: on_recordButton_clicked(); break;
        case 10: on_loadButton_clicked(); break;
        case 11: on_saveButton_clicked(); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
