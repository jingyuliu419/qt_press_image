/****************************************************************************
** Meta object code from reading C++ file 'presssensor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../appdesign_4/presssensor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'presssensor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PressSensor_t {
    QByteArrayData data[14];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PressSensor_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PressSensor_t qt_meta_stringdata_PressSensor = {
    {
QT_MOC_LITERAL(0, 0, 11), // "PressSensor"
QT_MOC_LITERAL(1, 12, 8), // "islisten"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 4), // "isOK"
QT_MOC_LITERAL(4, 27, 17), // "updateUIpressData"
QT_MOC_LITERAL(5, 45, 18), // "newConnection_slot"
QT_MOC_LITERAL(6, 64, 13), // "readRead_slot"
QT_MOC_LITERAL(7, 78, 10), // "isFinished"
QT_MOC_LITERAL(8, 89, 2), // "ok"
QT_MOC_LITERAL(9, 92, 8), // "new_slot"
QT_MOC_LITERAL(10, 101, 14), // "handleAdoptCmd"
QT_MOC_LITERAL(11, 116, 5), // "state"
QT_MOC_LITERAL(12, 122, 14), // "startListening"
QT_MOC_LITERAL(13, 137, 9) // "closeFile"

    },
    "PressSensor\0islisten\0\0isOK\0updateUIpressData\0"
    "newConnection_slot\0readRead_slot\0"
    "isFinished\0ok\0new_slot\0handleAdoptCmd\0"
    "state\0startListening\0closeFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PressSensor[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   65,    2, 0x0a /* Public */,
       6,    0,   66,    2, 0x0a /* Public */,
       7,    1,   67,    2, 0x0a /* Public */,
       9,    0,   70,    2, 0x0a /* Public */,
      10,    1,   71,    2, 0x0a /* Public */,
      12,    0,   74,    2, 0x0a /* Public */,
      13,    0,   75,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QVariant,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Char,   11,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PressSensor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PressSensor *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->islisten((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateUIpressData((*reinterpret_cast< QVariant(*)>(_a[1]))); break;
        case 2: _t->newConnection_slot(); break;
        case 3: _t->readRead_slot(); break;
        case 4: _t->isFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->new_slot(); break;
        case 6: _t->handleAdoptCmd((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 7: _t->startListening(); break;
        case 8: _t->closeFile(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PressSensor::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PressSensor::islisten)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (PressSensor::*)(QVariant );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PressSensor::updateUIpressData)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PressSensor::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PressSensor.data,
    qt_meta_data_PressSensor,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PressSensor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PressSensor::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PressSensor.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PressSensor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void PressSensor::islisten(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void PressSensor::updateUIpressData(QVariant _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
