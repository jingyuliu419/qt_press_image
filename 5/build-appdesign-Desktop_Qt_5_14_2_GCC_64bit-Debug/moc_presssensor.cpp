/****************************************************************************
** Meta object code from reading C++ file 'presssensor.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../22222222/appdesign_4/presssensor.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
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
    QByteArrayData data[16];
    char stringdata0[154];
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
QT_MOC_LITERAL(5, 45, 3), // "sum"
QT_MOC_LITERAL(6, 49, 12), // "QVector<int>"
QT_MOC_LITERAL(7, 62, 4), // "data"
QT_MOC_LITERAL(8, 67, 18), // "newConnection_slot"
QT_MOC_LITERAL(9, 86, 13), // "readRead_slot"
QT_MOC_LITERAL(10, 100, 10), // "isFinished"
QT_MOC_LITERAL(11, 111, 2), // "ok"
QT_MOC_LITERAL(12, 114, 8), // "new_slot"
QT_MOC_LITERAL(13, 123, 14), // "handleAdoptCmd"
QT_MOC_LITERAL(14, 138, 5), // "state"
QT_MOC_LITERAL(15, 144, 9) // "closeFile"

    },
    "PressSensor\0islisten\0\0isOK\0updateUIpressData\0"
    "sum\0QVector<int>\0data\0newConnection_slot\0"
    "readRead_slot\0isFinished\0ok\0new_slot\0"
    "handleAdoptCmd\0state\0closeFile"
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
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,
       5,    1,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    0,   68,    2, 0x0a /* Public */,
       9,    0,   69,    2, 0x0a /* Public */,
      10,    1,   70,    2, 0x0a /* Public */,
      12,    0,   73,    2, 0x0a /* Public */,
      13,    1,   74,    2, 0x0a /* Public */,
      15,    0,   77,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::QVariant,    2,
    QMetaType::Int, 0x80000000 | 6,    7,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Char,   14,
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
        case 2: { int _r = _t->sum((*reinterpret_cast< const QVector<int>(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        case 3: _t->newConnection_slot(); break;
        case 4: _t->readRead_slot(); break;
        case 5: _t->isFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->new_slot(); break;
        case 7: _t->handleAdoptCmd((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 8: _t->closeFile(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<int> >(); break;
            }
            break;
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
        {
            using _t = int (PressSensor::*)(const QVector<int> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&PressSensor::sum)) {
                *result = 2;
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
            qt_static_metacall(this, _c, _id, _a);
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

// SIGNAL 2
int PressSensor::sum(const QVector<int> & _t1)
{
    int _t0{};
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t0))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
    return _t0;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
