/****************************************************************************
** Meta object code from reading C++ file 'blehandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Orion_Client/blehandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'blehandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BLEHandler_t {
    QByteArrayData data[27];
    char stringdata0[434];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BLEHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BLEHandler_t qt_meta_stringdata_BLEHandler = {
    {
QT_MOC_LITERAL(0, 0, 10), // "BLEHandler"
QT_MOC_LITERAL(1, 11, 17), // "deviceListUpdated"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 18), // "serviceListUpdated"
QT_MOC_LITERAL(4, 49, 25), // "characteristicListUpdated"
QT_MOC_LITERAL(5, 75, 11), // "textRecived"
QT_MOC_LITERAL(6, 87, 4), // "text"
QT_MOC_LITERAL(7, 92, 9), // "addDevice"
QT_MOC_LITERAL(8, 102, 20), // "QBluetoothDeviceInfo"
QT_MOC_LITERAL(9, 123, 4), // "info"
QT_MOC_LITERAL(10, 128, 12), // "updateDevice"
QT_MOC_LITERAL(11, 141, 10), // "agentError"
QT_MOC_LITERAL(12, 152, 37), // "QBluetoothDeviceDiscoveryAgen..."
QT_MOC_LITERAL(13, 190, 5), // "error"
QT_MOC_LITERAL(14, 196, 15), // "deviceConnected"
QT_MOC_LITERAL(15, 212, 18), // "deviceDisconnected"
QT_MOC_LITERAL(16, 231, 10), // "addService"
QT_MOC_LITERAL(17, 242, 14), // "QBluetoothUuid"
QT_MOC_LITERAL(18, 257, 10), // "newService"
QT_MOC_LITERAL(19, 268, 15), // "serviceScanDone"
QT_MOC_LITERAL(20, 284, 15), // "controllerError"
QT_MOC_LITERAL(21, 300, 27), // "QLowEnergyController::Error"
QT_MOC_LITERAL(22, 328, 19), // "serviceStateChanged"
QT_MOC_LITERAL(23, 348, 31), // "QLowEnergyService::ServiceState"
QT_MOC_LITERAL(24, 380, 8), // "newState"
QT_MOC_LITERAL(25, 389, 12), // "serviceError"
QT_MOC_LITERAL(26, 402, 31) // "QLowEnergyService::ServiceError"

    },
    "BLEHandler\0deviceListUpdated\0\0"
    "serviceListUpdated\0characteristicListUpdated\0"
    "textRecived\0text\0addDevice\0"
    "QBluetoothDeviceInfo\0info\0updateDevice\0"
    "agentError\0QBluetoothDeviceDiscoveryAgent::Error\0"
    "error\0deviceConnected\0deviceDisconnected\0"
    "addService\0QBluetoothUuid\0newService\0"
    "serviceScanDone\0controllerError\0"
    "QLowEnergyController::Error\0"
    "serviceStateChanged\0QLowEnergyService::ServiceState\0"
    "newState\0serviceError\0"
    "QLowEnergyService::ServiceError"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BLEHandler[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   84,    2, 0x06 /* Public */,
       3,    0,   85,    2, 0x06 /* Public */,
       4,    0,   86,    2, 0x06 /* Public */,
       5,    1,   87,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   90,    2, 0x08 /* Private */,
      10,    0,   93,    2, 0x08 /* Private */,
      11,    1,   94,    2, 0x08 /* Private */,
      14,    0,   97,    2, 0x08 /* Private */,
      15,    0,   98,    2, 0x08 /* Private */,
      16,    1,   99,    2, 0x08 /* Private */,
      19,    0,  102,    2, 0x08 /* Private */,
      20,    1,  103,    2, 0x08 /* Private */,
      22,    1,  106,    2, 0x08 /* Private */,
      25,    1,  109,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,   13,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,   18,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 21,   13,
    QMetaType::Void, 0x80000000 | 23,   24,
    QMetaType::Void, 0x80000000 | 26,   13,

       0        // eod
};

void BLEHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<BLEHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->deviceListUpdated(); break;
        case 1: _t->serviceListUpdated(); break;
        case 2: _t->characteristicListUpdated(); break;
        case 3: _t->textRecived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->addDevice((*reinterpret_cast< const QBluetoothDeviceInfo(*)>(_a[1]))); break;
        case 5: _t->updateDevice(); break;
        case 6: _t->agentError((*reinterpret_cast< QBluetoothDeviceDiscoveryAgent::Error(*)>(_a[1]))); break;
        case 7: _t->deviceConnected(); break;
        case 8: _t->deviceDisconnected(); break;
        case 9: _t->addService((*reinterpret_cast< const QBluetoothUuid(*)>(_a[1]))); break;
        case 10: _t->serviceScanDone(); break;
        case 11: _t->controllerError((*reinterpret_cast< QLowEnergyController::Error(*)>(_a[1]))); break;
        case 12: _t->serviceStateChanged((*reinterpret_cast< QLowEnergyService::ServiceState(*)>(_a[1]))); break;
        case 13: _t->serviceError((*reinterpret_cast< QLowEnergyService::ServiceError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QBluetoothDeviceInfo >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QBluetoothUuid >(); break;
            }
            break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyController::Error >(); break;
            }
            break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyService::ServiceState >(); break;
            }
            break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QLowEnergyService::ServiceError >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (BLEHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BLEHandler::deviceListUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (BLEHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BLEHandler::serviceListUpdated)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (BLEHandler::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BLEHandler::characteristicListUpdated)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (BLEHandler::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BLEHandler::textRecived)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject BLEHandler::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_BLEHandler.data,
    qt_meta_data_BLEHandler,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *BLEHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BLEHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BLEHandler.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BLEHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void BLEHandler::deviceListUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void BLEHandler::serviceListUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void BLEHandler::characteristicListUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void BLEHandler::textRecived(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
