/****************************************************************************
** Meta object code from reading C++ file 'plotmatrix.h'
**
** Created: Sun Jun 5 22:42:40 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../plotmatrix.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plotmatrix.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PlotMatrix[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PlotMatrix[] = {
    "PlotMatrix\0\0scaleDivChanged()\0"
};

const QMetaObject PlotMatrix::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_PlotMatrix,
      qt_meta_data_PlotMatrix, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PlotMatrix::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PlotMatrix::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PlotMatrix::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PlotMatrix))
        return static_cast<void*>(const_cast< PlotMatrix*>(this));
    return QFrame::qt_metacast(_clname);
}

int PlotMatrix::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: scaleDivChanged(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
