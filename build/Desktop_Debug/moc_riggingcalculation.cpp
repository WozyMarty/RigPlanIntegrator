/****************************************************************************
** Meta object code from reading C++ file 'riggingcalculation.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/models/riggingcalculation.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'riggingcalculation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN18RiggingCalculationE_t {};
} // unnamed namespace

template <> constexpr inline auto RiggingCalculation::qt_create_metaobjectdata<qt_meta_tag_ZN18RiggingCalculationE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RiggingCalculation",
        "nameChanged",
        "",
        "descriptionChanged",
        "resultChanged",
        "id",
        "result",
        "validChanged",
        "collapsedChanged",
        "parameterChanged",
        "calcId",
        "paramIndex",
        "value",
        "statusChanged",
        "Status",
        "status",
        "name",
        "valid",
        "collapsed",
        "Empty",
        "Incomplete",
        "Computed",
        "Error"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'nameChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'descriptionChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'resultChanged'
        QtMocHelpers::SignalData<void(int, double)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::Double, 6 },
        }}),
        // Signal 'validChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'collapsedChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'parameterChanged'
        QtMocHelpers::SignalData<void(int, int, double)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 10 }, { QMetaType::Int, 11 }, { QMetaType::Double, 12 },
        }}),
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void(enum Status)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'id'
        QtMocHelpers::PropertyData<int>(5, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Constant),
        // property 'name'
        QtMocHelpers::PropertyData<QString>(16, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'result'
        QtMocHelpers::PropertyData<double>(6, QMetaType::Double, QMC::DefaultPropertyFlags, 2),
        // property 'valid'
        QtMocHelpers::PropertyData<bool>(17, QMetaType::Bool, QMC::DefaultPropertyFlags, 3),
        // property 'collapsed'
        QtMocHelpers::PropertyData<bool>(18, QMetaType::Bool, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Status'
        QtMocHelpers::EnumData<enum Status>(14, 14, QMC::EnumFlags{}).add({
            {   19, Status::Empty },
            {   20, Status::Incomplete },
            {   21, Status::Computed },
            {   22, Status::Error },
        }),
    };
    return QtMocHelpers::metaObjectData<RiggingCalculation, qt_meta_tag_ZN18RiggingCalculationE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RiggingCalculation::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18RiggingCalculationE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18RiggingCalculationE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18RiggingCalculationE_t>.metaTypes,
    nullptr
} };

void RiggingCalculation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RiggingCalculation *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->nameChanged(); break;
        case 1: _t->descriptionChanged(); break;
        case 2: _t->resultChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2]))); break;
        case 3: _t->validChanged(); break;
        case 4: _t->collapsedChanged(); break;
        case 5: _t->parameterChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[3]))); break;
        case 6: _t->statusChanged((*reinterpret_cast<std::add_pointer_t<enum Status>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RiggingCalculation::*)()>(_a, &RiggingCalculation::nameChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RiggingCalculation::*)()>(_a, &RiggingCalculation::descriptionChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RiggingCalculation::*)(int , double )>(_a, &RiggingCalculation::resultChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RiggingCalculation::*)()>(_a, &RiggingCalculation::validChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RiggingCalculation::*)()>(_a, &RiggingCalculation::collapsedChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RiggingCalculation::*)(int , int , double )>(_a, &RiggingCalculation::parameterChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (RiggingCalculation::*)(Status )>(_a, &RiggingCalculation::statusChanged, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->id(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->name(); break;
        case 2: *reinterpret_cast<double*>(_v) = _t->result(); break;
        case 3: *reinterpret_cast<bool*>(_v) = _t->isValid(); break;
        case 4: *reinterpret_cast<bool*>(_v) = _t->isCollapsed(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setName(*reinterpret_cast<QString*>(_v)); break;
        case 4: _t->setCollapsed(*reinterpret_cast<bool*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *RiggingCalculation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RiggingCalculation::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18RiggingCalculationE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RiggingCalculation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void RiggingCalculation::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RiggingCalculation::descriptionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RiggingCalculation::resultChanged(int _t1, double _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void RiggingCalculation::validChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RiggingCalculation::collapsedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void RiggingCalculation::parameterChanged(int _t1, int _t2, double _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3);
}

// SIGNAL 6
void RiggingCalculation::statusChanged(Status _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
