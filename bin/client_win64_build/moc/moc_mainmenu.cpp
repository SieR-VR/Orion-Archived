/****************************************************************************
** Meta object code from reading C++ file 'mainmenu.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Orion_Client/mainmenu.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainmenu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_mainMenu_t {
    QByteArrayData data[19];
    char stringdata0[302];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_mainMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_mainMenu_t qt_meta_stringdata_mainMenu = {
    {
QT_MOC_LITERAL(0, 0, 8), // "mainMenu"
QT_MOC_LITERAL(1, 9, 16), // "updateDeviceList"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 17), // "updateServiceList"
QT_MOC_LITERAL(4, 45, 24), // "updateCharacteristicList"
QT_MOC_LITERAL(5, 70, 10), // "dataUpdate"
QT_MOC_LITERAL(6, 81, 9), // "writeText"
QT_MOC_LITERAL(7, 91, 4), // "text"
QT_MOC_LITERAL(8, 96, 18), // "on_btnScan_clicked"
QT_MOC_LITERAL(9, 115, 21), // "on_btnConnect_clicked"
QT_MOC_LITERAL(10, 137, 24), // "on_btnDisconnect_clicked"
QT_MOC_LITERAL(11, 162, 20), // "on_btnNotify_clicked"
QT_MOC_LITERAL(12, 183, 18), // "on_btnRead_clicked"
QT_MOC_LITERAL(13, 202, 33), // "on_cbServices_currentIndexCha..."
QT_MOC_LITERAL(14, 236, 5), // "index"
QT_MOC_LITERAL(15, 242, 18), // "driverMemoryCreate"
QT_MOC_LITERAL(16, 261, 16), // "driverDataUpdate"
QT_MOC_LITERAL(17, 278, 18), // "orion::trackerData"
QT_MOC_LITERAL(18, 297, 4) // "data"

    },
    "mainMenu\0updateDeviceList\0\0updateServiceList\0"
    "updateCharacteristicList\0dataUpdate\0"
    "writeText\0text\0on_btnScan_clicked\0"
    "on_btnConnect_clicked\0on_btnDisconnect_clicked\0"
    "on_btnNotify_clicked\0on_btnRead_clicked\0"
    "on_cbServices_currentIndexChanged\0"
    "index\0driverMemoryCreate\0driverDataUpdate\0"
    "orion::trackerData\0data"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_mainMenu[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    0,   82,    2, 0x08 /* Private */,
       6,    1,   83,    2, 0x08 /* Private */,
       8,    0,   86,    2, 0x08 /* Private */,
       9,    0,   87,    2, 0x08 /* Private */,
      10,    0,   88,    2, 0x08 /* Private */,
      11,    0,   89,    2, 0x08 /* Private */,
      12,    0,   90,    2, 0x08 /* Private */,
      13,    1,   91,    2, 0x08 /* Private */,
      15,    0,   94,    2, 0x08 /* Private */,
      16,    1,   95,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Bool,
    QMetaType::Void, 0x80000000 | 17,   18,

       0        // eod
};

void mainMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<mainMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateDeviceList(); break;
        case 1: _t->updateServiceList(); break;
        case 2: _t->updateCharacteristicList(); break;
        case 3: _t->dataUpdate(); break;
        case 4: _t->writeText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->on_btnScan_clicked(); break;
        case 6: _t->on_btnConnect_clicked(); break;
        case 7: _t->on_btnDisconnect_clicked(); break;
        case 8: _t->on_btnNotify_clicked(); break;
        case 9: _t->on_btnRead_clicked(); break;
        case 10: _t->on_cbServices_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: { bool _r = _t->driverMemoryCreate();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 12: _t->driverDataUpdate((*reinterpret_cast< const orion::trackerData(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject mainMenu::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_mainMenu.data,
    qt_meta_data_mainMenu,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *mainMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *mainMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_mainMenu.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int mainMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
